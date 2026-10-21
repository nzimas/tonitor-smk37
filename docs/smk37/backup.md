# SMK-37: the full flash backup, on a Mac

Before TONITOR is first installed on an SMK-37, the whole 1 MiB flash is read and kept. Parts of it are unique
to each unit and cannot be downloaded again: `0xC0000-0xC1FFF` (Bluetooth pairing, and the factory key, pad
and wheel calibration) and `0xFF000-0xFFFFF` (the chip's key and MAC). The procedure follows the kit's
"Method A" (genkigenki/fm1-to-smk37, `docs/02-update-and-recovery.md`, section 3.1), with a Mac in place of
Linux or Windows.

## What happens

1. `tools/fm1_install.py --uboot` runs only the first step of an install. The stock firmware reads TONITOR's
   package, stages its update loader and restarts into it. The loader writes nothing until it is served the
   write, which `--uboot` never does. It sends the loader the UBOOT key (`F0 22 24 35 7D F7`) instead: the chip
   restarts into its mask-ROM bootloader and shows up on USB as `WL82 UBOOT1.00`.
2. `tools/smk37_backup.py` takes that device from macOS, loads JieLi's flash loader into the chip's RAM, reads
   the flash twice, compares the two reads, and checks the dump against TONITOR's flash map. It has no erase
   or write command.
3. The keyboard is switched off and on. It starts in TONITOR's update loader (its update record is still
   there), ready for the install.

The stock firmware ignores the UBOOT key, so step 1 is needed. (From TONITOR onwards, OCTAVE- and OCTAVE+
held for 5 seconds also enter UBOOT.)

## Once

```
.venv/bin/pip install pyusb libusb-package
./build.sh
```

## The backup

1. Close every program that uses the keyboard's MIDI ports.
2. Start the loader and enter UBOOT (it asks before it starts):

   ```
   .venv/bin/python tools/fm1_install.py build/tonitor.fwsc --uboot
   ```

3. Read the flash (macOS asks for your password):

   ```
   sudo .venv/bin/python tools/smk37_backup.py
   ```

   If macOS says the disk is not readable, choose **Ignore**. The script waits up to 2 minutes for the
   device. The two dumps go to `backup/` (never committed: see `.gitignore`).

4. Read the check at the end. "the flash is as TONITOR's map assumes" means:
   - the two reads are the same,
   - the calibration marker `trimdat` is at `0xC1224`,
   - the region table at `0x4060` is the same as in TONITOR's package (the stock v016 layout).

   If anything else is printed, stop: nothing is installed until the layout is understood.

5. Copy `backup/` to two other places.
6. Switch the keyboard off and on. It is now in TONITOR's update loader: install TONITOR
   (`./install.sh`), or put the stock firmware back with M-VAVE's SMK-37 Pro package.

A dump can be checked again at any time, without the keyboard:

```
.venv/bin/python tools/smk37_backup.py --check backup/smk37-dump-....bin
```

## Where the tools come from

- `tools/smk37_backup.py`: Charles Vestal's `fm1_rescue.py` (fm1-x0x; also in hugelton/felucca-bsp), which
  reads and restores an FM-1's flash from a Mac through the same bootloader. Everything that writes was
  removed; the SMK-37's flash chips (`0xEB6014`, and `0x856014` on some units) are accepted.
- The flash loader `wl82loader.bin`: kagaimiq/jl-uboot-tool, commit `c32b28e`, SHA-256
  `d41da612…dca5376`, checked before use.
