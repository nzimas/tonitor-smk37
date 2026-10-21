#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
# Copyright (C) 2026 Charles Vestal (fm1_rescue.py, from fm1-x0x); read-only SMK-37 cut for TONITOR
"""smk37_backup: read the whole flash of an M-VAVE SMK-37 through the chip's own update mode (the mask-ROM
"UBOOT", "WL82 UBOOT1.00"), from a Mac, and check what was read. It writes nothing to the keyboard: this file
has no erase, write or run command at all.

From fm1_rescue.py (Charles Vestal; github.com/charlesvestal/fm1-x0x, also in hugelton/felucca-bsp), which
does the same on a Mac for the FM-1 (same chip, same UBOOT); its protocol is FM-1-transporter's
(github.com/kurogedelic/FM-1-transporter, docs/PROTOCOL.md). What changed: everything that writes is gone,
the SMK-37's flash chips are accepted (0xEB6014, and 0x856014 as on some units), the flash is read twice and
the two compared, and the dump is checked against what TONITOR assumes of the SMK-37's flash
(hal/fm1_flash_map.h): the factory calibration "trimdat" at 0xC1224, the stock end marker "usrdata" at
0xEAFF8, and the region table at 0x4060 against TONITOR's package.

  .venv/bin/pip install pyusb libusb-package
  sudo .venv/bin/python tools/smk37_backup.py                      # wait for UBOOT, read twice, check
  .venv/bin/python tools/smk37_backup.py --check DUMP.bin          # check a dump already made (no device)

The keyboard must be in UBOOT: the stock firmware does not go there by itself; TONITOR's update loader does
on the SysEx F0 22 24 35 7D F7 (docs/smk37/backup.md). macOS takes the UBOOT device for a disk; this script
takes it back from macOS (hence sudo). The chip leaves UBOOT about 3 s after the last command: the two reads
run back to back.

JieLi's flash loader (wl82loader.bin) is jl-uboot-tool's (github.com/kagaimiq/jl-uboot-tool), downloaded at a
fixed commit and checked against its hash, as fm1_rescue.py does; it runs in the chip's RAM.
"""
import argparse
import hashlib
import os
import struct
import sys
import time
import urllib.request

VID, PID = 0x4C4A, 0x8057
LOADER_URL = ("https://raw.githubusercontent.com/kagaimiq/jl-uboot-tool/"
              "c32b28e7a04f873f526ba929cb1bae9f05442fe9/data/loaderblobs/usb/wl82loader.bin")
LOADER_SHA256 = "d41da6126760c9d66660bcc0cac8d27d221806c5e369a8036921efe68dca5376"
LOADER_ADDR, LOADER_ARG = 0x1C02000, 0x0001

FLASH_SIZE = 0x100000
IO = 512
CHIP_KEY = 0x980F
FLASH_IDS = {0xEB6014: "the SMK-37 Pro's (as the kit's unit)", 0x856014: "the FM-1's part (as on some SMK-37 units)"}

# what TONITOR's SMK-37 map assumes (hal/fm1_flash_map.h; the kit's docs/01-hardware.md)
TRIM_MARK_OFF, TRIM_MARK = 0xC1224, b"trimdat"
STOCK_MARK_OFF, STOCK_MARK = 0xEAFF8, b"usrdata\0"
RT_OFF, RT_LEN = 0x4060, 0xC0
UNIQUE = [(0xC0000, 0xC1000, "BTIF: Bluetooth pairing"), (0xC1000, 0xC2000, "USRTRIM: factory key, pad and wheel calibration"),
          (0xFF000, 0x100000, "key_mac: the chip's key and MAC")]

# ROM UBOOT1.00 (RAM only)
CMD_WRITE_MEMORY, CMD_JUMP = 0xFB06, 0xFB08
# the loader (reads only)
CMD_READ_FLASH, CMD_READ_KEY, CMD_GET_ONLINE_DEVICE = 0xFD05, 0xFC09, 0xFC0A


class Fail(Exception):
    pass


# ------------------------------------------------------------------ maths ---
def crc16(data, crc=0):
    """CRC-16/XMODEM (poly 0x1021, no reflection)"""
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF if crc & 0x8000 else (crc << 1) & 0xFFFF
    return crc


MAGIC = bytes([0xC3, 0xCF, 0xC0, 0xE8, 0xCE, 0xD2, 0xB0, 0xAE,
               0xC4, 0xE3, 0xA3, 0xAC, 0xD3, 0xF1, 0xC1, 0xD6])


def crc_cipher(buf):
    """JieLi's CrcDecode cipher, key 0xFFFFFFFF (in place)"""
    crc = crc16(b"\xFF\xFF", 0xFFFF)
    for i in range(len(buf)):
        crc = crc16(MAGIC[i % 16:i % 16 + 1], crc)
        buf[i] ^= crc & 0xFF


# -------------------------------------------------------------- transport ---
class Bot:
    """USB mass storage bulk-only transport: one SCSI-style command per call."""

    def __init__(self, dev, usb):
        self.dev, self.usb, self.tag = dev, usb, 1
        cfg = dev.get_active_configuration() if self._configured() else None
        if cfg is None:
            dev.set_configuration()
            cfg = dev.get_active_configuration()
        intf = cfg[(0, 0)]
        self.intf = intf.bInterfaceNumber
        util = usb.util
        self.ep_out = util.find_descriptor(intf, custom_match=lambda e: util.endpoint_direction(
            e.bEndpointAddress) == util.ENDPOINT_OUT and util.endpoint_type(e.bmAttributes) == util.ENDPOINT_TYPE_BULK)
        self.ep_in = util.find_descriptor(intf, custom_match=lambda e: util.endpoint_direction(
            e.bEndpointAddress) == util.ENDPOINT_IN and util.endpoint_type(e.bmAttributes) == util.ENDPOINT_TYPE_BULK)
        if self.ep_out is None or self.ep_in is None:
            raise Fail("no bulk endpoints")
        util.claim_interface(dev, self.intf)

    def _configured(self):
        try:
            self.dev.get_active_configuration()
            return True
        except self.usb.core.USBError:
            return False

    def reset_recovery(self):
        """Bulk-only mass storage reset, then clear both halts (what a host does after a stall)."""
        try:
            self.dev.ctrl_transfer(0x21, 0xFF, 0, self.intf, None, timeout=2000)
        except self.usb.core.USBError:
            pass
        for ep in (self.ep_in, self.ep_out):
            try:
                self.dev.clear_halt(ep)
            except self.usb.core.USBError:
                pass

    def xfer(self, cdb, data_out=None, n_in=0, timeout=5000):
        tag = self.tag
        self.tag = (self.tag + 1) & 0xFFFFFFFF
        length = len(data_out) if data_out is not None else n_in
        flags = 0x80 if n_in else 0x00
        cbw = struct.pack("<4sIIBBB", b"USBC", tag, length, flags, 0, len(cdb)) + bytes(cdb).ljust(16, b"\0")
        self.ep_out.write(cbw, timeout)
        got = b""
        if data_out is not None and len(data_out):
            self.ep_out.write(bytes(data_out), timeout)
        elif n_in:
            got = bytes(self.ep_in.read(n_in, timeout))
        csw = bytes(self.ep_in.read(13, timeout))
        sig, ctag, _, status = struct.unpack("<4sIIB", csw)
        if sig != b"USBS" or ctag != tag:
            raise Fail(f"bad status block ({csw.hex()})")
        if status != 0:
            raise Fail(f"command failed (status {status})")
        return got


def cdb_for(cmd, args=b""):
    return (struct.pack(">H", cmd) + bytes(args)).ljust(16, b"\xFF")


# ----------------------------------------------------------------- device ---
class Uboot:
    def __init__(self, bot):
        self.bot = bot

    def inquiry(self):
        r = self.bot.xfer(bytes([0x12, 0, 0, 0, 36, 0]), n_in=36)
        return r[8:16].decode(errors="replace").strip(), r[16:32].decode(errors="replace").strip()

    def cmd(self, cmd, args=b""):
        r = self.bot.xfer(cdb_for(cmd, args), n_in=16)
        if struct.unpack(">H", r[0:2])[0] != cmd:
            raise Fail(f"command {cmd:04X} answered as {r[0:2].hex()}")
        return r[2:]

    def start_loader(self, blob):
        """the loader into the chip's RAM (not the flash), then run it"""
        for off in range(0, len(blob), IO):                  # shipped already ciphered: send raw
            data = blob[off:off + IO]
            args = struct.pack(">IH", LOADER_ADDR + off, len(data)) + b"\0" + struct.pack("<H", crc16(data))
            self.bot.xfer(cdb_for(CMD_WRITE_MEMORY, args), data_out=data)
        self.cmd(CMD_JUMP, struct.pack(">IH", LOADER_ADDR, LOADER_ARG))

    def chip(self):
        p = self.cmd(CMD_READ_KEY, struct.pack(">I", 0x00AC6900))
        k = bytearray([p[5], p[4]])
        crc_cipher(k)
        key = k[0] | k[1] << 8
        p = self.cmd(CMD_GET_ONLINE_DEVICE)
        return key, p[0], struct.unpack_from("<I", p, 2)[0]

    def read(self, addr, n, say=None):
        out = bytearray()
        while len(out) < n:
            k = min(IO, n - len(out))
            out += self.bot.xfer(cdb_for(CMD_READ_FLASH, struct.pack(">IH", addr + len(out), k)), n_in=k)
            if say and len(out) % 0x40000 == 0:
                say(f"\r    {len(out) * 100 // n:3d} %", end="")
        if say:
            say("")
        return bytes(out)


# ------------------------------------------------------------------ check ---
def package_flash(path):
    """TONITOR's package: its flash.bin (through the installer's own reader)"""
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import fm1_install
    _, image = fm1_install.load_package(path, False)
    fl = next(f for f in fm1_install.package_files(image) if f[0] == 0)
    return image[fl[1]:fl[1] + fl[2]], fm1_install


def check(dump, package=None, say=print):
    """What TONITOR assumes of this unit's flash; returns the problems found (none: the map fits)"""
    bad = []
    if len(dump) != FLASH_SIZE:
        return [f"the dump is {len(dump)} bytes, not {FLASH_SIZE}"]
    t = dump[TRIM_MARK_OFF:TRIM_MARK_OFF + len(TRIM_MARK)]
    say(f"  calibration marker at {TRIM_MARK_OFF:#07x}: {t!r} " + ("ok" if t == TRIM_MARK else "MISSING"))
    if t != TRIM_MARK:
        bad.append("no 'trimdat' at 0xC1224: the calibration is not where TONITOR's map protects it")
    s = dump[STOCK_MARK_OFF:STOCK_MARK_OFF + len(STOCK_MARK)]
    say(f"  stock end marker at {STOCK_MARK_OFF:#07x}: {s!r} " + ("ok" if s == STOCK_MARK else "(not there)"))
    for lo, hi, what in UNIQUE:
        blank = all(b == 0xFF for b in dump[lo:hi])
        say(f"  {lo:#07x}-{hi - 1:#07x} {what}: sha256 {hashlib.sha256(dump[lo:hi]).hexdigest()[:16]}"
            + (" (blank)" if blank else ""))
    if package:
        flash, fi = package_flash(package)
        same = flash[RT_OFF:RT_OFF + RT_LEN] == dump[RT_OFF:RT_OFF + RT_LEN]
        say(f"  region table at {RT_OFF:#06x}: " + ("the same bytes as TONITOR's package" if same else "DIFFERENT"))
        for name, (a, n) in sorted(fi.region_table(dump).items(), key=lambda x: x[1][0]):
            say(f"    {name:9s} {a:#07x} + {n:#07x}")
        if not same:
            bad.append("the region table differs from TONITOR's package (stock v016's): compare the layout above")
    return bad


# ------------------------------------------------------------------- flow ---
def backup(dev, loader, out_dir, say=print):
    """Everything in one session: the chip leaves UBOOT ~3 s after the last command."""
    vendor, product = dev.inquiry()
    say(f"  device: {vendor} {product}")
    if vendor != "WL82":
        raise Fail(f"not a WL82 UBOOT ({vendor!r})")
    dev.start_loader(loader)
    key, typ, fid = dev.chip()
    say(f"  chip key {key:04X}, flash type {typ}, id {fid:06X}: " + FLASH_IDS.get(fid, "AN UNKNOWN PART"))
    if key != CHIP_KEY or typ != 3 or fid not in FLASH_IDS:
        raise Fail("not the SMK-37's chip and 1 MiB flash (key 980F, type 3, id EB6014 or 856014); nothing read")
    dumps = []
    for n in (1, 2):
        say(f"  reading the whole flash, pass {n} of 2 ...")
        t0 = time.time()
        dumps.append(dev.read(0, FLASH_SIZE, say))
        say(f"    {time.time() - t0:.1f} s, sha256 {hashlib.sha256(dumps[-1]).hexdigest()}")
    stamp = time.strftime("%Y%m%d-%H%M%S")
    paths = []
    for n, d in enumerate(dumps, 1):
        p = os.path.join(out_dir, f"smk37-dump-{stamp}-{n}.bin")
        with open(p, "wb") as f:
            f.write(d)
        paths.append(p)
    if dumps[0] != dumps[1]:
        raise Fail("the two reads differ: do not trust them; read again (" + ", ".join(paths) + ")")
    say("  the two reads are the same: " + ", ".join(paths))
    return dumps[0], paths


def get_loader(path):
    if path:
        blob = open(path, "rb").read()
    else:
        print("downloading JieLi's flash loader (wl82loader.bin, jl-uboot-tool) ...")
        with urllib.request.urlopen(LOADER_URL, timeout=30) as r:
            blob = r.read()
    if hashlib.sha256(blob).hexdigest() != LOADER_SHA256:
        raise Fail("wl82loader.bin: unexpected contents")
    return blob


def backend(usb):
    """libusb: pip's libusb-package (no Homebrew needed), else the system's or Homebrew's"""
    try:
        import libusb_package
        b = libusb_package.get_libusb1_backend()
        if b is not None:
            return b
    except ImportError:
        pass
    import usb.backend.libusb1 as lb
    for path in (None, "/opt/homebrew/lib/libusb-1.0.dylib", "/usr/local/lib/libusb-1.0.dylib"):
        b = lb.get_backend() if path is None else (lb.get_backend(find_library=lambda _: path)
                                                   if os.path.exists(path) else None)
        if b is not None:
            return b
    raise Fail("libusb not found: pip install libusb-package")


def open_device(usb, timeout):
    """Wait for 4C4A:8057 and take it from macOS's mass storage driver."""
    print("waiting for the SMK-37 in UBOOT ('WL82 UBOOT1.00') ...")
    be = backend(usb)
    t0 = time.time()
    while time.time() - t0 < timeout:
        dev = usb.core.find(idVendor=VID, idProduct=PID, backend=be)
        if dev is not None:
            print(f"found it after {time.time() - t0:.1f} s")
            try:
                try:
                    if dev.is_kernel_driver_active(0):
                        dev.detach_kernel_driver(0)
                except (NotImplementedError, usb.core.USBError) as e:
                    if getattr(e, "errno", None) not in (None, 2):
                        print(f"  (could not detach macOS's driver: {e})")
                bot = Bot(dev, usb)
                try:
                    Uboot(bot).inquiry()
                except Exception:
                    bot.reset_recovery()
                return bot
            except Exception as e:
                print(f"  could not open it ({e}); waiting")
                time.sleep(1.0)
                continue
        time.sleep(0.02)
    raise Fail("the SMK-37 did not appear in UBOOT")


def give_back(path):
    """the files to the user who ran sudo"""
    uid, gid = os.environ.get("SUDO_UID"), os.environ.get("SUDO_GID")
    if uid and gid:
        os.chown(path, int(uid), int(gid))


def main():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", default=os.path.join(here, "backup"), help="where the dumps go (default: backup/)")
    ap.add_argument("--package", default=os.path.join(here, "build", "tonitor.fwsc"),
                    help="TONITOR's package, for the region table (default build/tonitor.fwsc)")
    ap.add_argument("--check", metavar="DUMP", help="only check a dump already made (no device)")
    ap.add_argument("--loader", help="a local wl82loader.bin instead of downloading it")
    ap.add_argument("--wait", type=float, default=120, help="seconds to wait for UBOOT (default 120)")
    ap.add_argument("--tries", type=int, default=3, help="attempts if it drops out mid-way (default 3)")
    args = ap.parse_args()
    pkg = args.package if os.path.exists(args.package) else None
    try:
        if args.check:
            bad = check(open(args.check, "rb").read(), pkg)
            print("check: " + ("; ".join(bad) if bad else "the flash is as TONITOR's map assumes"))
            return 1 if bad else 0
        loader = get_loader(args.loader)
        if os.geteuid() != 0:
            raise Fail("run it with sudo: macOS has to give the device up")
        try:
            import usb.core
            import usb.util
        except ImportError:
            raise Fail("pyusb is missing: .venv/bin/pip install pyusb libusb-package")
        os.makedirs(args.out, exist_ok=True)
        give_back(args.out)
        for attempt in range(1, args.tries + 1):
            bot = open_device(usb, args.wait)
            try:
                dump, paths = backup(Uboot(bot), loader, args.out, say=lambda *a, **k: print(*a, **k, flush=True))
                for p in paths:
                    give_back(p)
                break
            except (Fail, usb.core.USBError) as e:
                print(f"\n  attempt {attempt}: {e}")
                if isinstance(e, Fail) and ("nothing read" in str(e) or "not a WL82" in str(e)):
                    return 1
            finally:
                try:
                    usb.util.dispose_resources(bot.dev)
                except Exception:
                    pass
        else:
            raise Fail("gave up; send this whole output")
        bad = check(dump, pkg)
        print("check: " + ("; ".join(bad) if bad else "the flash is as TONITOR's map assumes"))
        print("Keep the dumps in two places (they hold this unit's own calibration). Power-cycle the keyboard.")
        return 1 if bad else 0
    except Fail as e:
        print(f"smk37_backup: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
