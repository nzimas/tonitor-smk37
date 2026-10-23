<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/assets/logo-dark.svg">
    <img src="docs/assets/logo-light.svg" alt="Tonitor" width="520">
  </picture>
</p>

<p align="center"><b>An alternative firmware for the M-VAVE SMK-37 Pro, for electroacoustic experimentation, live ambient music and sound exploration.</b></p>

Tonitor for the SMK-37 Pro is a separate firmware that shares its core with
[Tonitor for the M-VAVE FM-1](https://github.com/nzimas/tonitor) (it started from Tonitor 0.53, commit `c39d18f`).
It turns the SMK-37 Pro into a self-contained six-track instrument: seven synthesis engines, loopers,
sequencers and three modes in which the other tracks play along with the one you play.

> [!NOTE]
> **Work in progress.** Builds are numbered (BUILD N); the manual and the website for the SMK-37 are still to
> come. The SMK-37's hardware notes and the flash backup procedure are in [docs/smk37](docs/smk37/).
> What each engine costs, and how much you can stack: [docs/smk37/engine-costs.md](docs/smk37/engine-costs.md).

> [!WARNING]
> Installing Tonitor replaces the firmware M-VAVE ships with the SMK-37 Pro. It is unofficial, is not made,
> endorsed or supported by M-VAVE, and installing it **may void any warranty that applies to your device**.
> **Back up the keyboard's flash first** ([docs/smk37/backup.md](docs/smk37/backup.md)). You install it at your
> own risk; read the [disclaimer](#disclaimer) first.

## What it is

- **Six tracks**, each with its own engine and sound, level, pan, filter, granulator, crush, distortion and
  sends to a master delay and reverb.
- **Seven engines**: FM (six operators, DX7 style), ANIMA, CALIGULA, RUSSOLO, DRUID, BAYAREA and HIVE. Every
  sound is generated from a seed, in families, and can lean towards an ambient or an experimental path.
- **Every key plays**: the 37 keys follow the chosen scale and root contiguously, black keys included.
- **Loopers** (up to eight loops a track, with direction, speed, stutter and climb), **BALLSY** (a physics
  sequencer), **BRICKLAYER** (a step sequencer built a note at a time), chords and latched notes.
- **REACT, SHADOW and ARP**: the tracks you choose play along with the track you are playing.
  - **REACT** improvises from a memory of everything you play, recombining it in your style and rhythm.
  - **SHADOW** follows your playing closely, behind you, at its own pace, now and then backwards or moved
    along the scale.
  - **ARP** breaks the chords you play into arpeggios at related speeds, following your chord changes.
- **Sessions, presets, MIDI** over USB and the TRS jack, USB audio.
- **A screen made for low vision**: few words, very large type (Atkinson Hyperlegible), high contrast.

## The panel, in short

| Control | What it does |
|---|---|
| Top row of pads, 1–6 | the six tracks (the chosen one lit) |
| Top row of pads, 7–8 | the AMBIENT and EXPERIMENTAL paths for new sounds |
| Bottom row of pads | the page of the button that opened it (chords, loop options, tracks for REACT/SHADOW/ARP, …) |
| Top row of buttons | CHORD, LOOP, MACRO, BALLSY, BRICKLAYER (left to right); a short press turns on or off, a long press shows the options on the bottom pads |
| Bottom row of buttons | HOME (a new sound), SAVE (sessions; SHIFT: global setup), scale, reverb and delay, PLAY, REC |
| KNOB BANK | SHIFT |
| FADER BANK | the envelope page; held, the REACT / SHADOW / ARP page; with SHIFT, that mode on or off |
| < and > | through sounds, sessions and settings; on the REACT page, the mode |
| Knobs K1–K8 | level, pan, filter, grain, crush, distortion, delay send, reverb send (or a page's own values) |
| Fader 4 | master level |
| Wheels | pitch bend and modulation |

## Building and installing

On macOS (or Linux), with Docker for the toolchain:

```sh
./setup.sh                 # once: the Python environment and the toolchain image
./build.sh                 # builds build/tonitor.fwsc
./install.sh               # builds, then installs on the SMK-37 connected over USB
./install.sh --info        # what the connected SMK-37 runs
./install.sh FILE.fwsc     # installs another package (to go back, for example)
```

The installer works from the stock firmware, from SLOOP, Felucca or Tonitor itself, and from the rescue mode.
Back up the flash first with `tools/smk37_backup.py` (see [docs/smk37/backup.md](docs/smk37/backup.md)).

The host tests build the firmware's sound and panel code on the computer (`tests/host/host.c`);
`tests/check_target_ir.sh` checks that the firmware needs nothing the chip lacks (no floating point, no 64-bit
division).

## How it is built

`firmware/src/tonitor.c` is the single compilation unit: the engines (`fm.c` around msfa's `fm6_core.c`,
`fm_dx7.c`, `anima.c`, `cal.c`, `russolo.c`, `druid.c`, `bayarea.c`, `hive.c`), `layers.c` (the six tracks, the
loopers, the mix, dealt to the chip's two cores), `ui.c` (panel and screen), `react.c` (REACT, SHADOW, ARP),
`balls.c`, `bricks.c`, `sessions.c`, `setup.c` and `midi.c`. `firmware/hal/` is the SMK-37 board layer (keybed,
buttons, encoders, pads, faders, wheels, pad LEDs, screen). `research/dx7/` holds the analysis of a collection of
DX7 voices from which five FM families were learned.

# Licence

- **The firmware** is free software under the **GNU General Public License, version 3 only**
  (`GPL-3.0-only`, [`LICENSE`](LICENSE)): it is a work based on GPL-3.0 code (see [`LICENSING.md`](LICENSING.md)).
- **The documentation and the logo** written for Tonitor are under the **MIT licence**
  ([`LICENSES/MIT.txt`](LICENSES/MIT.txt)).
- Third-party parts keep their own licences: msfa (Apache-2.0), the LIRA-8 architecture (BSD 3-clause),
  Noisferatu's techniques (GPL-3.0), Rings (MIT), the fonts (SIL OFL 1.1), the JieLi SDK files placed in the
  package (Apache-2.0).

[`LICENSING.md`](LICENSING.md) lists every file and its licence.

# Credits

Tonitor is by Nuno Zimas. It stands on the work of others:

- [**Felucca**](https://github.com/hugelton/Felucca) by Leo Kuroshita / Hügelton Instruments: the hardware
  track, USB stack, update loader, rescue mode and installer that made these firmwares possible, and the
  SMK-37 input layer (`genkigenki/Felucca`, branch `smk37`).
- **SLOOP** (`genkigenki/sloop-fm1`, branch `smk37`): the SMK-37 installer and the TRS MIDI output.
- **fm1_rescue.py** by Charles Vestal ([fm1-x0x](https://github.com/charlesvestal/fm1-x0x)), on which the
  backup tool is based.
- **msfa** by Google Inc. and Pascal Gauthier (Apache-2.0), from Dexed, in Felucca's integer port: the FM.
- **ANIMA** and **BAYAREA** are ported from the anima and bayarea opcodes of Vascular; **HIVE** from the
  SWARMER machine of DigiRust.
- **CALIGULA** follows the architecture of [**LIRA-8**](https://github.com/MikeMorenoDSP/LIRA-8) by Miguel
  Moreno (BSD), an emulation of SOMA Laboratory's LYRA-8.
- **RUSSOLO** re-makes, in fixed point, the techniques of
  [**Noisferatu**](https://github.com/rob-scape/noisferatu) by Robert Heel (GPL-3.0).
- **DRUID** is a fixed-point port of the resonators of
  [**Mutable Instruments Rings**](https://github.com/pichenettes/eurorack) by Émilie Gillet (MIT).
- REACT follows ideas from François Pachet's Continuator, IRCAM's OMax and Somax, and George Lewis's Voyager.
- **Atkinson Hyperlegible** by the Braille Institute (SIL OFL 1.1) and **Terminus** (SIL OFL 1.1).

M-VAVE and SMK-37 are trademarks of their owner, used here only to say which device Tonitor runs on.

# Disclaimer

Tonitor is an **unofficial** firmware. It is not made, endorsed, approved or supported by M-VAVE, and it is
not affiliated with M-VAVE.

**Installing it replaces the device's original firmware and may void any warranty that applies to your
SMK-37 Pro**, if any applies at all; check the terms of your warranty before installing. Installing any
firmware carries a risk: an interrupted or failed update can leave the device unable to start until it is
recovered, and saving sessions overwrites data that other firmwares may keep in the same flash area.

The software is provided **"as is", without warranty of any kind**, express or implied, including but not
limited to the warranties of merchantability, fitness for a particular purpose and non-infringement. In no
event shall the authors or contributors be liable for any claim, damages or other liability — including
damage to your device, loss of data, or loss of the use of the device — arising from, out of or in
connection with the software, its installation or its use. You install and use it entirely at your own
risk. See also sections 15 and 16 of the [GPL](LICENSE).
