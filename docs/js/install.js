// SPDX-License-Identifier: GPL-3.0-only
// TONITOR: the installer page -- the firmware fetched and checked, the SMK-37 found over Web MIDI, the update
// run by fm1ota.js with its progress in words and a bar. A firmware file chosen instead (M-VAVE's, to go back)
// takes the published firmware's place, once checked.
import { Updater, loadPackage, InstallError } from "./fm1ota.js";

const $ = (id) => document.getElementById(id);
const ui = {
  status: $("status"), detail: $("detail"), facts: $("facts"), fw: $("f-fw"), dev: $("f-dev"),
  bar: $("bar"), fill: $("bar").firstElementChild, agree: $("agree"), connect: $("connect"), install: $("install"),
  file: $("file"), latest: $("latest"),
};
let pkg = null, published = null, updater = null, device = null, busy = false;

function say(status, detail = "", kind = "") {
  ui.status.textContent = status;
  ui.status.className = "status" + (kind ? " " + kind : "");
  ui.detail.textContent = detail;
}
function refresh() {
  ui.connect.disabled = busy || !pkg || !ui.agree.checked;
  ui.install.disabled = busy || !pkg || !device || !ui.agree.checked;
}
const guard = (e) => {
  e.preventDefault();
  e.returnValue = "";
};

async function loadFirmware() {
  try {
    const meta = await (await fetch("firmware/latest.json", { cache: "no-store" })).json();
    const r = await fetch(`firmware/${meta.file}`, { cache: "no-store" });
    if (!r.ok) throw new Error(`HTTP ${r.status}`);
    const raw = new Uint8Array(await r.arrayBuffer());
    const digest = new Uint8Array(await crypto.subtle.digest("SHA-256", raw));
    const hex = [...digest].map((b) => b.toString(16).padStart(2, "0")).join("");
    if (hex !== meta.sha256) throw new InstallError("badpkg", "The firmware did not download intact. Reload the page.");
    pkg = published = { ...loadPackage(raw), name: `Tonitor ${meta.version}` };
    show();
    if (!navigator.requestMIDIAccess) {
      say("This browser cannot reach the keyboard",
        "It has no Web MIDI. Open this page in a desktop Chrome, Edge or Opera.", "bad");
      pkg = null;
    } else if (!window.isSecureContext) {
      say("Open this page over https", "Web MIDI only works on a secure page.", "bad");
      pkg = null;
    } else
      say("Ready", "Tick the box below, then find your SMK-37.");
  } catch (e) {
    say("The firmware could not be loaded", e.message, "bad");
  }
  refresh();
}

function show() {
  ui.fw.textContent = pkg ? `${pkg.name} (${pkg.product})` : "none";
  ui.facts.hidden = false;
  ui.install.textContent = pkg && pkg.stock ? "Install this firmware" : "Install Tonitor";
  ui.latest.hidden = !published || pkg === published;
}

async function chooseFile() {
  const f = ui.file.files[0];
  ui.file.value = "";
  if (!f || busy) return;
  try {
    const p = loadPackage(new Uint8Array(await f.arrayBuffer()), true);
    pkg = { ...p, name: p.stock ? "M-VAVE's firmware" : f.name };
    show();
    say("Firmware file checked", `${f.name}: ${pkg.product}, laid out for the SMK-37. Find your SMK-37, then install it.`);
  } catch (e) {
    say("This file cannot be installed", e.message, "bad");
  }
  refresh();
}

async function connect() {
  busy = true;
  refresh();
  say("Looking for the SMK-37…", "Allow MIDI access if the browser asks.");
  try {
    if (!updater) updater = new Updater(await navigator.requestMIDIAccess({ sysex: true }));
    if (device) device.link.close();
    device = await updater.find();
    if (!device) {
      ui.dev.textContent = "not found";
      say("No SMK-37 found", "Is it switched on and connected with a data cable? Is another program using it? " +
        "Then try again.", "bad");
    } else {
      const mode = device.id.loader ? " — in update mode: the install will finish the write" : "";
      ui.dev.textContent = `${device.id.text}${mode}`;
      say("SMK-37 found", "Press the install button. Do not unplug it or close this page until it has restarted.");
    }
  } catch (e) {
    say("MIDI access was refused", "This page needs MIDI with system-exclusive messages to update the keyboard. " +
      "Allow it in the site settings, then reload.", "bad");
  }
  busy = false;
  refresh();
}

const STEP = {
  start: () => ["Starting the update", "The keyboard checks the firmware."],
  check: (n) => ["Checking", `The keyboard has read ${n} parts of the firmware.`],
  loader: () => ["Switching to update mode", "The keyboard restarts into its update loader. It may take a few seconds."],
  write: (p) => [`Writing ${p}%`, "Do not unplug the keyboard or close this page."],
  reboot: () => ["Restarting", "The keyboard restarts into the new firmware."],
};

async function install() {
  if (!device || !pkg) return;
  busy = true;
  refresh();
  window.addEventListener("beforeunload", guard);
  ui.bar.hidden = false;
  ui.fill.style.width = "0%";
  try {
    const text = await updater.install(device, pkg.image, pkg.product, (k, a) => {
      if (k === "done") return;
      const [s, d] = STEP[k](a);
      say(s, d);
      if (k === "write") ui.fill.style.width = `${a}%`;
    });
    ui.fill.style.width = "100%";
    ui.dev.textContent = text;
    say(pkg.stock ? "Done: your SMK-37 runs M-VAVE's firmware" : "Done: your SMK-37 runs Tonitor",
      pkg.stock ? text : "Read the manual to get started.", "ok");
  } catch (e) {
    const msg = e instanceof InstallError ? e.message : `Unexpected error: ${e.message}`;
    say("The install stopped", msg + "\nReload this page and try again; see “If something goes wrong” below.", "bad");
  }
  device = null;
  window.removeEventListener("beforeunload", guard);
  busy = false;
  refresh();
}

ui.agree.addEventListener("change", refresh);
ui.connect.addEventListener("click", connect);
ui.install.addEventListener("click", install);
ui.file.addEventListener("change", chooseFile);
ui.latest.addEventListener("click", () => {
  pkg = published;
  show();
  say("Ready", "Tonitor again. Find your SMK-37, then install it.");
  refresh();
});
loadFirmware();
