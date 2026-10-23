// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments (the update protocol, tools/fm1_install.py)
// TONITOR: the SMK-37 update over Web MIDI, in the browser -- a port of tools/fm1_install.py.
//
// Step 1: the running firmware reads parts of the package and starts the update loader. Step 2: the loader
// reads the whole package and writes it. In both the device asks (SysEx read requests on the logical image)
// and we answer. Then the keyboard restarts and the installed identity is checked. A keyboard found already in
// update mode (an install cut off) only needs step 2. M-VAVE's own SMK-37 packages (the way back to the stock
// firmware) are read as tools/fm1_install.py reads them: their 36 interleave blocks, and their board checked
// from the package itself, so an FM-1 package is never written to an SMK-37.
//
// No DOM here: the page (install.js) gives it a MIDIAccess and a progress callback; tests give it a fake.

export const HS_QUERY = [0xF0, 0x00, 0x32, 0x45, 0x00, 0x00, 0x00, 0x40, 0x7F, 0xF7];
export const UPGRADE = [0xF0, 0x22, 0x24, 0x35, 0x7F, 0xF7];
export const FINISH_CHECK = 0xE0000000, FINISH_WRITE = 0xF0000000;
const MAXDATA = 512, BLOCKS = 20, BLK = 0x30, KEEP = 0x2F, MAX_BLOCKS = 64;
const LOADER_MARK = "FELUCCA-LOADER-1";
// never probe other gear: the SMK-37 (stock: "SMK-37 Pro-Private") and the FM-1 under TONITOR (or ACOUSMA, its
// working name), SLOOP / Felucca, CLIPPER,
// the update loaders, the stock firmware
export const PORT_RE = /smk|fm-1|tonitor|acousma|felucca|sloop|clipper|ota|composite|sinco|usb-midi/i;

// milliseconds; the tests shorten them
export const DELAY = {
  open: 300, start: 2000, reply: 10, loader: 3000, reboot: 3000, retry: 1000, poll: 1000, hs: 1000,
  idleCheck: 8000, idleWrite: 180000, waitLoader: 30000, waitReboot: 40000,
};

export class InstallError extends Error {
  constructor(code, msg) {
    super(msg);
    this.code = code;
  }
}

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const now = () => (typeof performance !== "undefined" ? performance.now() : Date.now());

// ------------------------------------------------------------ wire format ---

export function pack7(data) {
  const out = [];
  let acc = 0, nb = 0;
  for (const b of data) {
    acc |= b << nb;
    nb += 8;
    while (nb >= 7) {
      out.push(acc & 0x7F);
      acc >>>= 7;
      nb -= 7;
    }
  }
  if (nb) out.push(acc & 0x7F);
  return out;
}

export function unpack7(s) {
  const out = [];
  let acc = 0, nb = 0;
  for (const b of s) {
    acc |= (b & 0x7F) << nb;
    nb += 7;
    while (nb >= 8) {
      out.push(acc & 0xFF);
      acc >>>= 8;
      nb -= 8;
    }
  }
  return out;
}

const sum = (a) => a.reduce((x, y) => x + y, 0);

export function response(addr, data, fl = 0) {
  const n = data.length;
  const body = [0x00, 0x59, 0x30, (n + 8) & 0xFF, ((n + 8) >> 8) & 0xFF, 0, fl,
    addr & 0xFF, (addr >>> 8) & 0xFF, (addr >>> 16) & 0xFF, (addr >>> 24) & 0xFF, n & 0xFF, (n >> 8) & 0xFF, 0];
  for (const b of data) body.push(b);
  body.push(~sum(body.slice(6)) & 0xFF);
  return [0xF0, ...pack7(body), 0xF7];
}

const le = (u, a, b) => u.slice(a, b).reduceRight((x, y) => x * 256 + y, 0);

/** -> {fl, addr, len} or null */
export function parseRequest(pkt) {
  if (pkt.length < 2 || pkt[0] !== 0xF0 || pkt[pkt.length - 1] !== 0xF7) return null;
  const u = unpack7(pkt.slice(1, -1));
  if (u.length !== 15 || u[0] !== 0x00 || u[1] !== 0x59 || u[2] !== 0x30) return null;
  if ((~sum(u.slice(6, 14)) & 0xFF) !== u[14]) return null;
  return { fl: u[6], addr: le(u, 7, 11), len: le(u, 11, 14) };
}

/** -> {text, model, version, loader} or null */
export function parseIdentity(pkt) {
  if (pkt.length < 2 || pkt[0] !== 0xF0 || pkt[pkt.length - 1] !== 0xF7) return null;
  const d = unpack7(pkt.slice(1, -1));
  if (d.length !== 34 || d[0] !== 0x00 || d[1] !== 0x59 || d[2] !== 0x11) return null;
  let txt = String.fromCharCode(...d.slice(6, 33));
  txt = txt.replace(/\0+$/, "");
  const m = /^([^_]+)_(\d+)$/.exec(txt);
  return m ? { text: txt, model: m[1], version: +m[2], loader: m[1].toLowerCase().startsWith("ota-") } : null;
}

// --------------------------------------------------------------- package ---

const marker = (raw, k) => ((k + 1) * BLK <= raw.length ? raw[k * BLK + KEEP] : null);
const idChar = (m, i) => { const c = (m - i - 1) & 0xFF; return c >= 0x20 && c < 0x7F; };

/** the identity blocks: 20, or more when the identity is longer (then unused 0x7D markers follow) */
function identityBlocks(raw) {
  let n = BLOCKS, m;
  while (n < MAX_BLOCKS && (m = marker(raw, n)) !== null && m !== 0x7D && idChar(m, n)) n++;
  return n > BLOCKS && marker(raw, n) === 0x7D ? n : BLOCKS;
}

/** interleave blocks: the identity blocks, then more while the marker is the unused 0x7D (SMK-37 Pro v016: 36) */
function blockCount(raw) {
  let n = identityBlocks(raw);
  while (n < MAX_BLOCKS && (n + 1) * BLK <= raw.length && raw[n * BLK + KEEP] === 0x7D) n++;
  return n;
}

/** the image the device reads during an update: the package without the marker bytes */
export function logicalImage(raw, blocks = BLOCKS) {
  const out = new Uint8Array(raw.length - blocks);
  let o = 0;
  for (let i = 0; i < blocks; i++) {
    out.set(raw.subarray(i * BLK, i * BLK + KEEP), o);
    o += KEEP;
  }
  out.set(raw.subarray(blocks * BLK), o);
  return out;
}

/** -> [image, blocks]: the detected count when the files' CRCs agree, else 20 */
function deinterleave(raw) {
  const n = blockCount(raw), image = logicalImage(raw, n);
  if (n !== BLOCKS && packageDamage(image)) {
    const alt = logicalImage(raw, BLOCKS);
    if (!packageDamage(alt)) return [alt, BLOCKS];
  }
  return [image, n];
}

/** the package identity ("SMK-37 Pro_731", "SMK-37 Pro_016"): one marker byte after each identity block */
export function productOf(raw) {
  if (raw.length < BLOCKS * BLK) throw new InstallError("badpkg", "not a firmware package (too short)");
  const ib = identityBlocks(raw);
  const n = ib === BLOCKS ? BLOCKS : Math.min(ib, deinterleave(raw)[1]);
  let s = "";
  for (let i = 0; i < n; i++) {
    const m = raw[i * BLK + KEEP];
    if (m !== 0x7D) s += String.fromCharCode((m - i - 1) & 0xFF);
  }
  return s;
}

export const modelOf = (text) => text.split("_")[0].replace(/^ota-/, "");
// the SMK-37 Pro, SMK-37 Elite, MKE-P37, Starrykey 37 Play: one keyboard under several names
const SMK_FAMILY = /^(SMK-37[ +].*|MKE-P37|STARRYKEY-37.*)$/i;
const SMK_MODEL = /^(ota-)?(SMK-37([ +].*)?|MKE-P37|STARRYKEY-37.*)$/i;
/** the models of identities a and b are the same hardware (never an FM-1 and an SMK) */
export function sameKeyboard(a, b) {
  const ma = modelOf(a), mb = modelOf(b);
  return ma === mb || (SMK_FAMILY.test(ma) && SMK_FAMILY.test(mb));
}

/** CRC-16/XMODEM, as the package and the device use */
export function crc16(data, crc = 0) {
  for (const b of data) {
    crc ^= b << 8;
    for (let k = 0; k < 8; k++) crc = crc & 0x8000 ? ((crc << 1) ^ 0x1021) & 0xFFFF : (crc << 1) & 0xFFFF;
  }
  return crc;
}

/** JieLi ENC stream cipher (its own inverse) */
function dec(b) {
  const out = Uint8Array.from(b);
  let key = 0xFFFF;
  for (let i = 0; i < out.length; i++) {
    out[i] ^= key & 0xFF;
    key = ((key << 1) ^ (key & 0x8000 ? 0x1021 : 0)) & 0xFFFF;
  }
  return out;
}

/** what is wrong with the update image (header, file list, each file's CRC), or null */
export function packageDamage(image) {
  if (image.length < 0x40) return "too short";
  const hdr = dec(image.subarray(0, 0x40));
  if (crc16(hdr.subarray(2, 0x40)) !== (hdr[0] | (hdr[1] << 8))) return "header CRC";
  const count = hdr[8] | (hdr[9] << 8);
  if (count < 1 || count > 8 || image.length < 0x40 + 0x50 * count) return "file list";
  const lst = image.subarray(0x40, 0x40 + 0x50 * count);
  if (crc16(lst) !== (hdr[2] | (hdr[3] << 8))) return "file list CRC";
  for (let i = 0; i < count; i++) {
    const e = dec(lst.subarray(i * 0x50, (i + 1) * 0x50));
    const dcrc = e[4] | (e[5] << 8);
    const off = le(Array.from(e), 8, 12), size = le(Array.from(e), 12, 16);
    let name = "";
    for (let k = 0x40; k < 0x50 && e[k]; k++) name += String.fromCharCode(e[k]);
    if (off + size > image.length) return `${name}: cut short`;
    // flash.bin (0) and ota.bin (100) carry the CRC of their bytes as served; the stock packer's other entries
    // store a CRC of something else: checked for their bounds only
    const typ = e[0] | (e[1] << 8);
    if ((typ === 0 || typ === 100) && crc16(image.subarray(off, off + size)) !== dcrc) return `${name}: CRC`;
  }
  return null;
}

// ---------------------------------------------------- the package's board ---
// Read from the package itself, never from an identity: the FM-1's and the SMK-37's official packages share the
// head and the update loader. Installed on an SMK-37, the FM-1's stock package erases the factory calibration and
// overwrites the stock resources. The board: flash.bin's length, its region table (SFC-enciphered, key 0x980F)
// and where a USR image (file type 50) goes.
const LAYOUT = { smk: [0x9D000, 0xC0000, [0xF4000, 0xA000]], fm1: [0x93000, 0xE9000, [0xEA000, 0x12000]] };

function sfc(data, base, key = 0x980F) {
  const out = Uint8Array.from(data);
  for (let i = 0; i < out.length; i += 32) {
    let k = (key ^ ((base + i) >>> 2)) & 0xFFFF;
    for (let j = i; j < Math.min(i + 32, out.length); j++) {
      out[j] ^= k & 0xFF;
      k = ((k << 1) ^ (k & 0x8000 ? 0x1021 : 0)) & 0xFFFF;
    }
  }
  return out;
}

const rd = (b, o, n) => le(Array.from(b.subarray(o, o + n)), 0, n);

/** [{typ, off, size, to, tosize}] from the file list */
function packageFiles(image) {
  if (image.length < 0x40) return [];
  const count = rd(dec(image.subarray(0, 0x40)), 8, 2), out = [];
  for (let i = 0; i < Math.min(count, 11); i++) {
    if (0x90 + i * 0x50 > image.length) break;
    const e = dec(image.subarray(0x40 + i * 0x50, 0x90 + i * 0x50));
    out.push({ typ: rd(e, 0, 2), off: rd(e, 8, 4), size: rd(e, 12, 4), to: rd(e, 0x1C, 4), tosize: rd(e, 0x20, 4) });
  }
  return out;
}

/** flash.bin's region table: {name: [address, size]} */
function regionTable(flash) {
  if (flash.length < 0x4120) return {};
  const t = sfc(flash.subarray(0x4060, 0x4120), 0x60), out = {};
  for (let r = 0; r < 0xC0; r += 32) {
    let name = "";
    for (let c = r + 16; c < r + 32 && t[c] > 0x20 && t[c] < 0x7F; c++) name += String.fromCharCode(t[c]);
    if (name) out[name] = [rd(t, r + 4, 4), rd(t, r + 8, 4)];
  }
  return out;
}

/** "smk", "fm1" (any sign of the FM-1's layout) or null */
export function packageBoard(image) {
  const files = packageFiles(image), fl = files.find((f) => f.typ === 0);
  if (!fl || fl.off + fl.size > image.length) return null;
  const rt = regionTable(image.subarray(fl.off, fl.off + fl.size));
  const usr = files.filter((f) => f.typ === 50);
  const [sLen, sBtif, sUsr] = LAYOUT.smk, [fLen, fBtif, fUsr] = LAYOUT.fm1;
  const btif = rt.BTIF ? rt.BTIF[0] : 0, rUsr = rt.USR || [0, 0];
  if (fl.size === sLen && btif === sBtif && rUsr[0] === sUsr[0] && rUsr[1] === sUsr[1] &&
      usr.every((u) => u.to === sUsr[0] && u.tosize === sUsr[1]))
    return "smk";
  if (fl.size === fLen || btif === fBtif || rUsr[0] === fUsr[0] || usr.some((u) => u.to === fUsr[0])) return "fm1";
  return null;
}

function hasLoaderMark(raw) {
  const mark = new TextEncoder().encode(LOADER_MARK);
  outer: for (let i = 0; i + mark.length <= raw.length; i++) {
    for (let k = 0; k < mark.length; k++) if (raw[i + k] !== mark[k]) continue outer;
    return true;
  }
  return false;
}

/** raw .fwsc bytes -> {product, image, stock}; throws InstallError("badpkg"). A package without our update
 * loader (M-VAVE's) only with allowStock, and only when it is laid out for the SMK-37 */
export function loadPackage(raw, allowStock = false) {
  const product = productOf(raw);
  if (!/^[^_]+_\d+$/.test(product)) throw new InstallError("badpkg", `not a firmware package (identity ${product})`);
  const stock = !hasLoaderMark(raw);
  if (stock && !allowStock) throw new InstallError("badpkg", "no Felucca update loader in this package");
  const [image] = deinterleave(raw);
  const bad = packageDamage(image);
  if (bad) throw new InstallError("badpkg", `damaged package (${bad}); nothing was written. Download it again.`);
  const board = packageBoard(image);
  if (board === "fm1")
    throw new InstallError("fm1pkg", "This is a firmware for the FM-1, not for the SMK-37. On an SMK-37 it would " +
      "erase the keyboard's calibration. Nothing was sent.");
  if (stock && board !== "smk")
    throw new InstallError("board", "This package is not laid out for the SMK-37. Nothing was sent.");
  if (!SMK_MODEL.test(modelOf(product)))
    throw new InstallError("model", `This package is for ${product}, not for the SMK-37. Nothing was sent.`);
  return { product, image, stock };
}

// ------------------------------------------------------------------ MIDI ---

/** one MIDI in/out pair with a SysEx queue */
class Link {
  constructor(input, output) {
    this.input = input;
    this.output = output;
    this.name = input.name;
    this.q = [];
    this.waiter = null;
    this.onmsg = (ev) => {
      const d = ev.data;
      if (d[0] !== 0xF0) return;
      this.q.push(Array.from(d));
      if (this.waiter) {
        const w = this.waiter;
        this.waiter = null;
        w();
      }
    };
    input.addEventListener("midimessage", this.onmsg);
  }

  get lost() {
    return this.input.state === "disconnected" || this.output.state === "disconnected";
  }

  send(pkt) {
    try {
      this.output.send(pkt);
      return true;
    } catch (e) {
      return false;
    }
  }

  async read(ms) {
    if (this.q.length) return this.q.shift();
    await new Promise((res) => {
      const t = setTimeout(() => {
        this.waiter = null;
        res();
      }, Math.max(ms, 0));
      this.waiter = () => {
        clearTimeout(t);
        res();
      };
    });
    return this.q.length ? this.q.shift() : null;
  }

  drain() {
    this.q.length = 0;
  }

  close() {
    this.input.removeEventListener("midimessage", this.onmsg);
  }
}

function pairOutput(name, outs) {
  const exact = outs.find((o) => o.name === name);
  if (exact) return exact;
  const base = (n) => n.replace(/\s+\d+$/, "");   // Windows numbers each port
  const same = outs.filter((o) => base(o.name) === base(name));
  if (same.length === 1) return same[0];
  return outs.length === 1 ? outs[0] : null;
}

async function handshake(link, tries = 3) {
  link.drain();
  for (let t = 0; t < tries; t++) {
    if (!link.send(HS_QUERY)) return null;
    const end = now() + DELAY.hs;
    let left;
    while ((left = end - now()) > 0) {
      const p = await link.read(left);
      if (!p) break;
      const id = parseIdentity(p);
      if (id) return id;
    }
  }
  return null;
}

// --------------------------------------------------------------- updater ---

export class Updater {
  /** access: a MIDIAccess (navigator.requestMIDIAccess({sysex: true})) */
  constructor(access) {
    this.access = access;
  }

  inputs() {
    return [...this.access.inputs.values()].filter((p) => p.state !== "disconnected");
  }

  outputs() {
    return [...this.access.outputs.values()].filter((p) => p.state !== "disconnected");
  }

  /** the first keyboard-looking port that answers the handshake (and want(identity)) -> {link, id} or null */
  async find(want = null) {
    const outs = this.outputs();
    for (const inp of this.inputs()) {
      if (!PORT_RE.test(inp.name || "")) continue;
      const out = pairOutput(inp.name, outs);
      if (!out) continue;
      let link;
      try {
        if (inp.open) await inp.open();
        if (out.open) await out.open();
        link = new Link(inp, out);
      } catch (e) {
        continue;
      }
      await sleep(DELAY.open);   // CoreMIDI may drop a message sent just after opening
      const id = await handshake(link, 2);
      if (id && (!want || want(id))) return { link, id };
      link.close();
    }
    return null;
  }

  async waitFor(want, ms) {
    const end = now() + ms;
    while (now() < end) {
      const dev = await this.find(want);
      if (dev) return dev;
      await sleep(DELAY.retry);
    }
    return null;
  }

  /** answer read requests until the device asks for `finish` -> {served, finished, lost} */
  async serve(link, image, finish, idle, progress) {
    let served = 0, last = now();
    for (;;) {
      const pkt = await link.read(DELAY.poll);
      if (!pkt) {
        if (link.lost) return { served, finished: false, lost: true };
        if (now() - last > idle) return { served, finished: false, lost: false };
        continue;
      }
      const r = parseRequest(pkt);
      if (!r) continue;
      last = now();
      if (r.addr === FINISH_CHECK || r.addr === FINISH_WRITE) {
        link.send(response(r.addr, [...new TextEncoder().encode("success"), 0]));
        if (r.addr === finish) return { served, finished: true, lost: false };
        continue;
      }
      if (r.len > MAXDATA || r.addr + r.len > image.length)
        throw new InstallError("badreq", `the device asked for 0x${r.addr.toString(16)}+${r.len}, outside the package`);
      if (DELAY.reply) await sleep(DELAY.reply);
      if (!link.send(response(r.addr, image.subarray(r.addr, r.addr + r.len), r.fl)))
        return { served, finished: false, lost: true };
      served++;
      progress(served, r.addr + r.len);
    }
  }

  async check(dev, image, step) {
    step("start", dev.id.text);
    dev.link.send(UPGRADE);
    await sleep(DELAY.start);
    const r = await this.serve(dev.link, image, FINISH_CHECK, DELAY.idleCheck, (k) => step("check", k));
    dev.link.close();
    if (!r.finished)
      throw new InstallError(r.lost ? "lost" : "stopped",
        `The keyboard ${r.lost ? "was disconnected" : "stopped answering"} after ${r.served} requests. Nothing was written.`);
    step("loader");
    await sleep(DELAY.loader);
    const ota = await this.waitFor((i) => i.loader, DELAY.waitLoader);
    if (!ota)
      throw new InstallError("noloader", "The update loader did not appear. Replug the USB cable and run the install " +
        "again: the keyboard stays in update mode until it is done.");
    return ota;
  }

  async write(ota, image, step) {
    step("write", 0);
    ota.link.send(UPGRADE);
    await sleep(DELAY.start);
    const r = await this.serve(ota.link, image, FINISH_WRITE, DELAY.idleWrite,
      (k, end) => step("write", Math.min(99, Math.floor((end * 100) / image.length))));
    ota.link.close();
    if (!r.finished)
      throw new InstallError(r.lost ? "lost" : "stopped",
        `The loader ${r.lost ? "was disconnected" : "stopped answering"} after ${r.served} requests. Replug the USB ` +
        "cable and run the install again to finish.");
    step("write", 100);
  }

  async verify(product, step) {
    step("reboot");
    await sleep(DELAY.reboot);
    const back = await this.waitFor((i) => !i.loader, DELAY.waitReboot);
    if (!back) throw new InstallError("noreturn", "The keyboard did not come back: switch it off and on again.");
    back.link.close();
    if (back.id.text !== product)
      throw new InstallError("mismatch", `Written, but the keyboard reports ${back.id.text} (expected ${product}).`);
    step("done", back.id.text);
    return back.id.text;
  }

  /** running firmware -> loader -> new firmware; dev from find() */
  async install(dev, image, product, step = () => {}) {
    if (!sameKeyboard(dev.id.text, product) || !SMK_MODEL.test(modelOf(dev.id.text))) {
      dev.link.close();
      throw new InstallError("model", `The device is ${dev.id.text}; this firmware is for ${product}. Nothing was sent.`);
    }
    if (dev.id.loader && !/^ota-(SMK-37 Pro|FM-1)_9\d\d$/i.test(dev.id.text)) {   // (our loader: SLOOP's)
      dev.link.close();   // another firmware's loader: its image layout may differ
      throw new InstallError("foreign", `The keyboard is in the update mode of another firmware (${dev.id.text}). ` +
        "Finish that update with its own updater (M-UPGRADE), then install again.");
    }
    const ota = dev.id.loader ? dev : await this.check(dev, image, step);
    await this.write(ota, image, step);
    return this.verify(product, step);
  }
}
