# Tonitor -> M-VAVE SMK-37 Pro: research notes

Date: 2026-10-21. Every external repo was read as data. AGENTS.md was treated as reference only, and none of its
instructions were followed.

## Where the clones are

`$S` = `/private/tmp/claude-501/-Users-nzimas-development-tonitor-smk37/06ec81a1-98cf-430e-b063-95e8e890dcbb/scratchpad`

| Repo | Clone | State |
|---|---|---|
| genkigenki/fm1-to-smk37 (the kit) | `$S/repos/fm1-to-smk37` | main |
| genkigenki/Felucca | `$S/repos/Felucca` | `smk37` checked out; remote `upstream` = hugelton/Felucca |
| genkigenki/sloop-fm1 | `$S/repos/sloop-fm1` | `smk37` checked out |
| Felucca smk37 full diff | `$S/repos/felucca-smk37.diff` | `git diff a53ecf4..smk37` (2516+/165-, 43 files) |

**Felucca branch note.** `smk37` is **one squashed commit**, `b67312c` ("SMK-37 Pro target (BOARD=smk37); the FM-1
build unchanged", 2026-10-16). Its parent is **Felucca 1.6.1 (`a53ecf4`)**, not 1.6 (`1727537`) as the kit's
docs/03-porting-steps.md:9,87 says. The kit's text speaks of a series of commits (emulator fix, Elite/MKE, copyright
lines). On the branch, all of these are folded into that one commit. It does include `SMK_MODEL` (Elite/MKE).

**How Tonitor relates to Felucca.** Tonitor's HAL is a copied tree, not a git fork. Compared with Felucca 1.6.1,
`fm1_adc.h`, `fm1_audio.h`, `fm1_cc.h`, `fm1_gpio.h` and `fm1_xip.h` are byte-identical. `fm1_lcd_hw.h` matches
Felucca commit `e5a908d` (it has no `fm1_lcd_backlight`). `fm1_flash.h` comes from SLOOP 2.4 (it has
`FL_FM6`/`FL_USR4`). Tonitor's `bootguard.h` and `recovery.c` also come from SLOOP, so SLOOP's `smk37` branch is the
source for those.

---

## 1. SMK-37 hardware map

Tags follow the kit (docs/01-hardware.md:7-13): **V** = verified on a unit, **A** = analysis of the stock v016
firmware, **O** = open.

### 1.1 SoC, boot, flash part
- AC791N (WL82). The SPL and boot config (0x0000-0x3FFF) are byte-identical to the FM-1's. Chip key 0x980F. The update loader `ota.bin` is identical. **A** (01-hardware.md:24-30)
- The app executes at XIP **0x02000120**, and app.bin sits at flash 0x4120. This is the same as on the FM-1. **V** (01:28)
- The flash JEDEC id is **0xEB6014** (the FM-1's is 0x856014). **V** (01:25,32-37). Other units may carry a different part, so read the id during the backup.
- The driver uses only generic NOR commands (9F, 05/35, 06, 20, 02, 0B), so adding the id is enough. Felucca: `firmware/hal/fm1_flash_map.h:14-20` (`FL_JEDEC_OK`).

### 1.2 Flash map and protected ranges
Stock v016 region table (**A**, 01:41-62):

| Range | Stock use | Rule |
|---|---|---|
| 0x00000-0x03FFF | header, SPL, boot config | **NEVER** |
| 0x04000-0x9CFFF | app area (PRCT). The app head is at 0x4000, app.bin at 0x4120 | update loader only |
| 0x9D000-0xBFFFF | VM (stock settings) | writable (custom store and OTA) |
| 0xC0000-0xC0FFF | BTIF (Bluetooth pairing) | **NEVER** (unique to the unit) |
| 0xC1000-0xC1FFF | USRTRIM: factory calibration, `trimdat` | **NEVER** (unique to the unit) |
| 0xC2000-0xEAFFF | USRFLASH (stock user mapping). End marker `usrdata\0` at 0xEAFF8 | writable, with the stock-marker rule (below) |
| 0xEB000-0xF3FFF | not listed in the region table | the reference ports leave it unused |
| 0xF4000-0xFDFFF | USR: stock resources | **do not write** (needed for a return to stock) |
| 0xFE000-0xFEFFF | not listed | unused |
| 0xFF000-0xFFFFF | key_mac | **NEVER** |

- The region table holds **absolute** offsets. Some unpackers wrongly add 0x4000. (01:56-61)
- Package region flags: VM `12 81`, PRCT/BTIF `92 82`, USRTRIM/USRFLASH/USR `92 81`. (01:60-62; Felucca `tools/fm1pkg_make.py:44-50`)

**How the allow-list works** (Felucca `firmware/hal/fm1_flash_map.h:22-92`):
- Explicit writable windows:
  - `FL_DATA` 0x9D000-0xBAFFF
  - `FL_OTA` 0xBB000-0xBFFFF
  - `FL_SMP` 0xC2000-0xE9FFF
  - `FL_STOCKTAG` (exactly the 0xEA000 sector)
  - For the loader only: `FL_APP` 0x4000-0x9CFFF and `FL_REC` 0x9D000-0xBFFFF
- `FL_WIN_SAFE` plus `_Static_assert`s check at compile time that every window stays clear of the protected ranges (lines 67-80).
- `FL_PROTECTED`/`FL_GUARD` (lines 90-92) is a **second, independent net** inside the RAM driver. `fl_erase4k_ram` and `fl_prog_ram` call `FL_GUARD` instead of `FL_RANGE_OK` (diff of `fm1_flash.h`). It refuses any protected range even when a caller defines its own `FL_RANGE_OK`, as the loader does.
- The loader uses `FL_LDR_RANGE_OK` (line 82). `ldr_core.c:34-48` asserts its windows stay below 0xC0000.
- The plain XIP window starts at **0x9D000** on the SMK (0x93000 on the FM-1). `FL_PLAIN_LO`, used by `fl_plain_window_init`.

**The calibration record at 0xC1000 (USRTRIM)** (**A** for the layout; key velocity with the factory curves works **V**; 01:64-78):

| Offset | Contents |
|---|---|
| +0x008 | 16 pads x 5 u16 |
| +0x0A8 | 37 keys x 5 u16 |
| +0x21A | 5 u16 for the pedal |
| +0x224 | the marker `trimdat\0` |

- Read it and check the marker and that every curve rises strictly. If either check fails, use the defaults in RAM. **Never write the sector.**
- Defaults: pads {50,550,720,850,950}, keys {5,640,800,920,980}, pedal {20,850,920,980,1010}.
- Loader code: Felucca `smk_input.h:227-264` (`smk_trim_defaults`, `smk_trim_load`); called at `main.c:267-270`.
- Boot self-test: 8 bytes at 0xC1224 must read `trimdat\0`.

**The stock marker in USRFLASH** (Felucca `fm1_flash_map.h:58-65`, `storage.c` `st_stock_tagged`/`st_stock_intact`/`st_stock_release`):
- While `usrdata\0` sits at 0xEAFF8, the stock firmware trusts USRFLASH and never rebuilds it.
- So before the **first** erase or write anywhere in 0xC2000-0xE9FFF, erase the 0xEA000 sector once. A later return to stock then rebuilds its factory mapping.

### 1.3 Display
| Item | SMK-37 | Status |
|---|---|---|
| Controller | ST7789V, IPS | **V** |
| Resolution | 240x240, no window offset, MADCTL 0x00, COLMOD 0x05 (RGB565, high byte first), INVON | **V** |
| Bus | SPI1 **group A** (`IOMAP_CON1` 0x51020 bit4 = 0): CLK **PB6**, MOSI **PB7** | **V** |
| CS | **PC8**, active low | **V** |
| D/C | **PB5** (low = command) | **V** |
| Reset pin | none (power-on reset) | **A** |
| Backlight | no control pin; always on. **PA2 is a matrix row** | **V** |
| SPI1 | 0x11D00 CON=0x4021, BAUD 4 (~12 MHz) | **V** |

(01:93-131)

- Orientation: MADCTL 0x00 with no offset is the stock setting (**V**). The FM-1's init sets a row offset (01:98).
- Init tables:
  - The stock tuned table is at 01:108-127.
  - Felucca keeps its minimal SWRESET/SLPOUT/COLMOD/MADCTL/INVON/NORON sequence for the SMK too (`Felucca/firmware/src/lcd.c:98-112`).
  - **Tonitor's `LCD_SEQ` is byte-identical to Felucca's**, so it needs no change. The stock table is optional.
- Pins: `Felucca/firmware/hal/fm1_lcd_hw.h:44-61,68-91`.

### 1.4 Pad RGB LEDs (all **V**, 01:133-158)

**Wiring and signal**
- 16 WS2812-class LEDs on one data line, **PC10**.
- PC10 is SPI1 MOSI in **group B** (`IOMAP_CON1` bit4 = 1). PC9, the group B clock, must stay an **analog input** (it is ADC ch8).
- Encoding: one SPI byte per LED bit, `0xF0` = 1 and `0xC0` = 0, MSB first, colour order **GRB**.
- Bit clock: SPI1 BAUD 8 (~6.67 MHz).

**Frame**
- The stock frame is a 36-byte reset plus 16 x 24 bytes.
- Felucca uses a **250-byte reset** instead. Newer WS2812 parts otherwise shift the chain by one. (08-pitfalls.md:79; `lcd.c:164-205`, `PADLED_LATCH 250`)

**Chain order** (hardware pad -> chain slot): `{0,15,14,13,12,11,10,9,1,2,3,4,5,6,7,8}` (`lcd.c:179`)

**Sharing SPI1 with the LCD** (`fm1_lcd_hw.h:142-157`, `fm1_padled_send`/`fm1_padled_done`; `lcd.c` `padled_flush`)
1. Wait until the LCD transfer has finished and CS (PC8) is high.
2. Set bit4 and BAUD 8, then DMA the frame.
3. Wait for the DMA, clear bit4 and restore the LCD baud.

Never switch the pin group mid-DMA. Keep PC10 a low output between frames.

### 1.5 Key matrix: keys, buttons, encoders
**Shift registers and return lines (V)**
- Columns: **4 x 74HC595** (32-bit word) shifted by **SPI2 group C**. CLK is PA3, MOSI is PA4 (`IOMAP_CON1` bits 17..16 = `10`).
- SPI2 setup: 0x11E00 CON=0x4020, BAUD 5, then enable bit 0. The word goes out as 4 bytes by DMA.
- Latch on **PA1** (pulse high). /OE on PA0, held low (**A**).
- Return lines (rows 0..7): **PA2, PA5, PA6, PA7, PA9, PH6, PH8, PH9**. Inputs with pull-ups; low = contact closed.
- (01:160-193; `smk_input.h:10-18,39-47,267-290`)

**The 32-bit word** (01:175-184)
| Bits | Use |
|---|---|
| 31..24 | column 0..7 select, active low, one-hot |
| 23..16 | column 8..15 select, active low, one-hot |
| 15..0 | button LED lines (active low) |
| 5..3 | the analog mux address |

**Matrix layout and scan**
- 16 columns x 8 rows. Rows 0-4 are the keybed, rows 5-6 the buttons, row 7 the encoders.
- **Scan order matters** (`smk_input.h:548-580`). In each 10 kHz tick:
  1. Read the rows of the column latched one tick earlier.
  2. Latch the word shifted during the last tick.
  3. Start the ADC for this tick.
  4. Start the DMA of the next column's word.
- A frame is 16 ticks = **1.6 ms (625 frames/s)**. The FM-1 runs 909/s.

**Keys: 37 velocity keys, two contacts each (V)**
- k=0 is C3 and k=36 is C6. Contact table: 01:200-211 and `smk_input.h:69-76`.
- Velocity algorithm (stock): `T = 10 + time from contact 1 to contact 2, in 0.1 ms`; `x = 1023 - T`; look x up in the key's 5-point curve; MIDI velocity = `max(1, v>>3)`. Note-off when contact 1 opens. (01:213-229; `smk_input.h:213-225,326-358`)
- With a 1.6 ms frame, fast strokes quantise coarsely.

**Buttons: 18 (V, all names checked on a unit; 01:231-245; `smk_input.h:77-81`)**

| id | code | name | id | code | name |
|---|---|---|---|---|---|
| 0 | 26 | KNOB BANK | 9 | 45 | CHORD |
| 1 | 76 | FADER BANK | 10 | 15 | GLOBE |
| 2 | 96 | < | 11 | 86 | BT |
| 3 | 56 | > | 12 | 95 | PATCH |
| 4 | 66 | OCTAVE- | 13 | 55 | PARA |
| 5 | 36 | OCTAVE+ | 14 | 85 | FX |
| 6 | 75 | ARP | 15 | 35 | SEQ |
| 7 | 65 | NOTE REPEAT | 16 | 16 | SEQ PLAY |
| 8 | 25 | SCALE | 17 | 46 | SEQ REC |

Debounce: press after 2 frames, release after 4 frames.

**Encoders K1..K8 on row 7 (V, order K1..K8 verified on a unit; 01:247-271)**
- A/B codes: {97,a7} {37,47} {17,27} {b7,c7} {57,67} {77,87} {d7,e7} {f7,07}.
- Felucca's comment at `smk_input.h:82` still says the order is "to confirm". It is stale.
- Decoder (stock): a new state must hold 2 frames, with a 6-bit history. **Divide by 4** for one step per detent (`SMK_ENC_DIV 4`). The stock divisor of 2 gives two steps per click. (`smk_input.h:381-409`)

**Button LEDs: 19 single-colour LEDs on two commons**
- The LED lines are the low half of the 595 word, active low. LED table: `smk_input.h:88-89`. LED 18 is the red half of SEQ PLAY. (01:273-294)
- The two commons are the second USB PHY's pads used as GPIO: register **0x16A0C**, bit17 = group 0 and bit18 = group 1, low = group on. Keep `|0x1000F` on every write.
- Multiplex: alternate the group on every frame.
- **V** for the LEDs the reference port drives, **A** for the rest of the table.

### 1.6 ADC and muxes (V, 01:296-327)

**Channels**
| Channel | Pin | Mux | Carries |
|---|---|---|---|
| ch1 | PA8 | A | pads 0-7 |
| ch8 | PC9 | B | pads 8-15 |
| ch2 | PA10 | C | faders, wheels, pedal, battery |

- All three are analog pins (no DIE, no pulls).
- The 3 x 8:1 muxes share one address: **bits 5..3 of the 595 word**. It takes effect one tick later.

**Timing: use the SMK's, not the FM-1's**
- SMK: CON = `0x400D | ch<<8`, then set bit4, then bit6. Done flag is bit7, after ~40 µs.
- The FM-1's `0xF04E` takes ~200 µs and never finishes inside a 100 µs tick.
- Clear bit14 of 0x11900 before each conversion. (`smk_input.h:534-544`)

**Mux C addresses**
| Address | Use |
|---|---|
| 0-3 | faders, index map {1,2,3,0} |
| 4 | pitch wheel |
| 5 | mod wheel |
| 6 | pedal |
| 7 | battery |

- Pad mux map (mux A; mux B is the same plus 8): `{1,2,3,0,4,7,5,6}` (`smk_input.h:91-94`).
- Cycle: 24 slots (8 addresses x 3 channels) = 2.4 ms.

### 1.7 Pads (16; velocity and pressure)
- **Top row = hardware pads 0..7 (mux A), bottom row = 8..15 (mux B)**, both left to right. **V**, padtest 2026-10-10. Older comments that say the opposite are wrong. (01:333-338)
- Use one hardware index for both the ADC and the LEDs, and convert to reading order in one place.
- Baseline: starts at 100, steps down 1 when the reading is below it, steps up 2 while below curve point 0.
- Hit: above baseline + 24. The peak of the next 3 samples, through the pad's curve, gives the velocity.
- Release: below baseline + 12 for 2 samples.
- Pressure (stock formula): raw ≤ 512 gives `raw>>4`; above that, `min(127, (raw-512)/4 + 32)`.
- (01:346-351; `smk_input.h:434-502`)

### 1.8 Faders (4)
- Full scale is 1023 (**V**).
- Filter: median of 3, then a **signed** IIR, then 0..127 with a ±1 dead band that still reaches both ends. (01:353-359; `smk_input.h:511-524`)
- Soft takeover in the UI: `Felucca/firmware/src/main.c:77-113` (`smk_faders`).

### 1.9 Wheels
- Pitch: rest ~535, travel ~445-640 (**V**).
- Mod: ~474-629, **up = lower raw** (**V**).
- Self-calibration: centre taken at power-on, drift followed slowly while resting, range widened to what has been seen, dead band ~12. (01:361-371; `Felucca/firmware/src/main.c:115-157` `smk_wheels`)
- The audio side is `seq.c:770+,813+` (`smk_wheels_block`, as MIDI bend and CC1).
- SLOOP adds a taught calibration (`sloop-fm1/firmware/src/smk_wheel.h`).

### 1.10 Pedal, battery, power
- **Pedal**: mux C address 6. Reads ~985 with nothing plugged in. The stock firmware inverts it and maps it through the extra curve. No plug-detect pin found. **A**
- **Battery**: mux C address 7. Stock thresholds raw 530/560/602 (4 levels). The divider ratio is open. **A**. Tonitor's are 531/561/591 (`ui.c:1433`), close enough to keep.
- **Power switch**: a hard slide switch, no soft power latch. **A**
- **Charger status**: **PB1**, input, low = charging. Holding PB1 low for 8 s resets the chip. **Never drive it and never enable its pull-down.** **A** (01:380-390)

### 1.11 Audio codec / I2S
- Same DAC/codec path, output stage and sample-rate setup as the FM-1. No change to the audio driver; the engines run unchanged. **V** (01:392-395)
- Felucca's smk37 branch does not touch `fm1_audio.h`, and Tonitor's `fm1_audio.h` is byte-identical to Felucca 1.6.1's.
- Codec control lines PC0/1/2/6 and ALNK0 do not collide with any SMK pin. **No I2S differences.**

### 1.12 MIDI
| Item | SMK-37 | Status |
|---|---|---|
| TRS jack | **MIDI OUT only. There is no TRS MIDI IN.** | **A** |
| UART | UART1 (0x12100), TX only, on **PC7** (the FM-1's LCD CS) | **A** |
| Baud | 31250 (BAUD 383 from PLL48M) | **A** |
| USB MIDI | class compliant | **V** |

(01:397-415)

- PH8, the FM-1's MIDI IN pin, is a **matrix row** on the SMK. Felucca forces `FELUCCA_UART 0` and adds an `#error` (`Felucca/firmware/src/felucca.c:28-37`).
- TX setup: `IOMAP_CON3` (0x51028) bit7 clear and bits[23:20] = 1. PC7 with PU=1, PD=1, output low, DIE=1. UART1 CON0 = 0x3400, then the baud, then enable.
- Sending a byte: set CON0 bit13 (clear TPND), write BUF 0x1210C, then wait for bit15.
- The jack is listed as type A (community spec sheet, not measured).
- **Felucca has no TX driver** ("TODO", `felucca.c:29-30`). SLOOP has one: `sloop-fm1/firmware/hal/smk_uart_tx.h` (57 lines) and `firmware/src/smk_midi_out.h` (ring, pacing, no-hang). The hook in `usb.c:164-165` is guarded `!FELUCCA_LOADER`; `main.c:319` initialises it. Status **A**: not confirmed on a unit.

### 1.13 USB
- Stock VID:PID **4353:B64B**, strings "SMK-37 Pro Midi", "SMK-37 Pro Audio" (**A**). The FM-1 is 4C4A:C755.
- Stock update loader: 4D4A:4155. Mask-ROM bootloader: `WL82 UBOOT1.00`, 4C4A:8057, USB mass storage (**V**).
- Felucca-family descriptors, including Tonitor's 1209:0003 composite MIDI + UAC1, work unchanged (**V** for Felucca's).
- Host tools must match port names containing `SMK`. (01:417-428)
- The SMK's USB-C port gives **no VBUS**, which matters for USB-host experiments (08:82).

### 1.14 Package and identity strings (01:430-440; 02:45-62)
| Item | FM-1 | SMK-37 Pro |
|---|---|---|
| Stock app | `FM-1_015` | `SMK-37 Pro_016` (14 chars) |
| Custom app (Felucca/SLOOP) | `FM-1_9XY` | `SMK-37 Pro_9XY` |
| **Tonitor** (proposed) | `FM-1_700` | `SMK-37 Pro_7XY` |
| Recovery screen | `FM-1_000` | `SMK-37 Pro_000` |
| Update record name written by the app | `ota-FM-1_015` (12 B) | **`ota-SMK-37 Pro_016`** (18 B, zero-padded to record +0x27) |
| Custom loader identity | `ota-FM-1_900` | `ota-SMK-37 Pro_900` |
| Stock package container | 20 interleave blocks | **36 blocks** (v016: 14 identity chars + 22 x 0x7D) |

- Elite and MKE builds: `SMK-37 Elite_…` and `MKE-P37_…` (`-DSMK_MODEL`). Felucca: `fm1_cc.h` and `build.py:65`.
- The Starrykey 37 Play uses the Pro build.
- Package region table: PRCT `0x9D000`, VM `0x9D000/0x23000`, BTIF `0xC0000`, USRTRIM `0xC1000`, USRFLASH `0xC2000/0x29000`, USR `0xF4000/0xA000`. App slot (XIP length) **0x97400**. (02:88-93; Felucca `fm1pkg_make.py:38-53`)
- Installers detect the block count (`while i<20 or marker==0x7D`, then check the CRCs of flash.bin and ota.bin only). Felucca: `web/fm1pkg.js:211-256`, `tools/fm1_install.py:24-38`.

### 1.15 Recovery and rescue (UBOOT equivalents)

**What exists on the SMK**
- **UBOOT soft key** `F0 22 24 35 7D F7`. A custom app or custom loader understands it; **the stock app does not**. (**V** on the loader; 02:41)
- **OCTAVE- + OCTAVE+ held 5 s** (button ids 4 and 5, while stopped) enters UBOOT. This replaces the FM-1's OCT- + OCT+. (02:135, 166; 03:213)
- **Rescue mode**: the SLOOP bootguard opens the USB recovery screen after 2 failed boots, with identity `SMK-37 Pro_000` (`sloop-fm1/firmware/src/felucca.c:193`). A third failed boot goes to UBOOT. **OCTAVE- alone at power-on** = manual rescue (`sloop-fm1/firmware/src/bootguard.h:27-34`, using `FM1_OCT_BOTH` and `FM1_BTN_OCT_DOWN = 4`). (02:165)
- **Both OCTAVE buttons at power-on** = panel calibration in Felucca and SLOOP (`Felucca/firmware/src/main.c:296`). Tonitor has no panel calibration.
- **Console**: `uboot yes` (Felucca and SLOOP CDC console).

**Last resorts**
- If the SPL or app header is broken, the mask ROM enters UBOOT by itself.
- Otherwise a JieLi USB_KEY dongle (an RP2040). These were built for the FM-1 and are **untried on the SMK** (**O**). (02:167)

**Writing flash in UBOOT** (02:169-186)
- Never write 0x0-0x3FFF, 0xC0000-0xC1FFF or 0xFF000-0xFFFFF except from your own dump.
- Work in whole 4 KiB sectors: erase, write, read back and compare.
- To restore the stock app, write 0x4000-0x9CFFF from a Method A dump.

### 1.16 Full flash backup procedure (02:95-139)
Tools: jl-uboot-tool (Python 3; raw SCSI, so root on Linux), a custom SMK package, and a host tool that can run only the check step of an update and then send one SysEx.

**Method A** (preferred, before anything is installed)
1. Connect over USB and close every other program using the MIDI port.
2. Run **only the check step** with the custom package: identity query, enter upgrade (`F0 22 24 35 7F F7`), serve the reads. The stock app accepts it, stages the custom loader and resets. The device now answers `ota-SMK-37 Pro_9xx`. **The app area has not been written.**
3. Send `F0 22 24 35 7D F7`. The keyboard re-enumerates as mass storage `WL82 UBOOT1.00` (**V**).
4. Find the device: `/dev/sgN` on Linux, `\\.\PhysicalDriveN` on Windows.
5. Read the whole flash twice:
   `python3 jluboottool.py --device <dev> --chip wl82 "read 0 0x100000 smk37-dump-1.bin"` (then `-2.bin`). Note the flash id it prints (expect `0xeb6014`).
6. Check both dumps:
   - each is exactly 1,048,576 bytes,
   - both have the same SHA-256,
   - `trimdat` is at 0xC1224.

   If any check fails, read again.
7. Keep the dump in at least two places. 0xC0000-0xC1FFF and 0xFF000-0xFFFFF are unique to the unit.
8. Power-cycle. The keyboard comes back in the custom loader's update mode, because the record is still there. Either finish the custom install or install the stock package.

**Method B** (after a custom firmware is installed): OCT- + OCT+ for 5 s, or the console's `uboot yes`, then steps 4-8. This dump lacks the stock app area but keeps the unit-unique ranges.

**Tonitor-specific note.** Tonitor's `tools/fm1_install.py` and `docs/js/fm1ota.js` must first learn the SMK names (port filter `SMK`, model `SMK-37 Pro`, loader `ota-SMK-37 Pro_9xx`) before they can run step 2 against a stock SMK. Running only the check step needs a small tool or flag, for example SLOOP's install pieces.

---

## 2. Tonitor HAL and board files: what must change

Felucca paths below are relative to `$S/repos/Felucca` (branch smk37) and SLOOP paths to `$S/repos/sloop-fm1`.
Tonitor paths are relative to `/Users/nzimas/development/tonitor-smk37`.

| Tonitor file (lines) | Today (FM-1) | SMK change | Port from |
|---|---|---|---|
| `firmware/hal/fm1_cc.h` | only `FM1_INLINE` | Add `BOARD_SMK37` and `SMK_MODEL`, or hard-code SMK, since this tree is SMK-only per HANDOFF §10 | `firmware/hal/fm1_cc.h:12-21` |
| `firmware/hal/fm1_flash.h:38-58` | FM-1 windows. Allow-list: `FL_DATA` 0x97000-0xE0000, **`FL_GLOB` 0xFC000-0xFF000**, `FL_FM6` 0xE5000-0xE7000, **`FL_USR4` 0xE7000-0xFB000**, `FL_OTA` 0xE0000-0xE5000 | Replace with an `fm1_flash_map.h` include. Use `FL_GUARD` in `fl_erase4k_ram` (:174) and `fl_prog_ram` (:191). `FL_PLAIN_LO` 0x9D000 at :225-229. Comment on `fl_jedec_ram` (:151). **`FL_GLOB` and `FL_USR4` lie inside the protected USR range and must go.** | `firmware/hal/fm1_flash.h` (diff), `firmware/hal/fm1_flash_map.h:1-92` (new; rewrite its windows for Tonitor's sessions, section 3) |
| `firmware/hal/fm1_input.h` (374 lines) | 2 x 595 bit-banged, 11 x 6 matrix, 27 keys + 14 buttons, 7 encoders with learned rest, LED glow (`fm1_led_dim`, `fm1_led_bg`, `fm1_led_bg_ns`, `fm1__led_lines`), `FM1_KEYMAP` | Replace with `smk_input.h`, which keeps the API (`fm1_in`, `fm1_input_init`, `fm1_input_tick`, `fm1_input_scan`, `fm1_enc_take`, `fm1_input_edges`, `fm1_input_note_edges`, `fm1_led_key`). Tonitor-only symbols need stubs or rework: `fm1_led_dim[]` (used in `ui.c:1343-1353` via `FM1_KEYMAP`), `fm1_led_bg*`, `fm1__led_lines`. | `firmware/hal/smk_input.h` (670 lines, new), `firmware/hal/smk_compat.h` (no-op LED glow API, new). SLOOP's `firmware/hal/smk_input.h` (722 lines) adds velocity setting hooks. |
| `firmware/hal/fm1_adc.h` | Polls ch4 (PB6, MASTER pot) and ch3 (PB1, battery). **`fm1_adc_init` sets PB6 and PB1 to analog**: on the SMK, **PB6 is the LCD clock** and PB1 the charger status | Drop it on the SMK. `smk_input.h:653-670` provides `fm1_adc_init` (empty) and `fm1_adc_read` (battery from mux C; MASTER as a virtual pot on K1, 24 counts per detent). Tonitor's `main.c:379,408-421` then work unchanged. | `firmware/hal/smk_input.h:653-670`; `firmware/src/felucca.c:73-83` (include switch) |
| `firmware/hal/fm1_lcd_hw.h:27-44` | PC7 CS, PC8 DC, PC9 CLK, PC10 DO, group B, **PA2 backlight driven low** | Pins PB6/PB7/PC8/PB5, group A, no backlight (PA2 is a matrix row), PC10 low output. Add `fm1_padled_send`/`fm1_padled_done` and `fm1_lcd_cur_baud`. | `firmware/hal/fm1_lcd_hw.h:44-157` |
| `firmware/src/lcd.c` | ST7789 driver; `LCD_SEQ` is the same as Felucca's | Keep `LCD_SEQ`. Optionally use the stock table (01:108-127). Add pad LED code: `PADLED_*`, `padled_set`, `padled_flush` (250-byte latch; put `padled_buf`, 634 B, in POOL because RAM is tight). Header comment at :4 mentions PA2. | `firmware/src/lcd.c:98-104` (comment), `:164-205` |
| `firmware/hal/fm1_uart.h:40-44` and `src/midi_uart.c` | TRS MIDI IN, RX on **PH8** | **Must not run on the SMK** (PH8 is a matrix row). Remove `uart_midi_init` (`main.c:393`) and `uart_midi_poll` (`main.c:143`). Optional TRS MIDI OUT on PC7. | Guard: `Felucca/firmware/src/felucca.c:28-37`. TX: `sloop-fm1/firmware/hal/smk_uart_tx.h`, `sloop-fm1/firmware/src/smk_midi_out.h`, hook at `sloop-fm1/firmware/src/usb.c:164-165`, init at `sloop-fm1/firmware/src/main.c:319` |
| `firmware/src/tonitor.c:10,36-40` | `TONITOR_ID "FM-1_700"`; includes `fm1_input.h` and `fm1_adc.h` | `SMK-37 Pro_700` (plus model). Include `smk_input.h` and `smk_compat.h` instead. | `firmware/src/felucca.c:40-45,73-83` |
| `firmware/src/main.c:217` | `OTA_IDENTITY` `FM-1_000` / `TONITOR_ID` | `SMK_MODEL "_000"` | `sloop-fm1/firmware/src/felucca.c:193` |
| `firmware/src/main.c:366`, `src/recovery.c:60`, `loader/loader.c:74` | `== 0x856014u` | `FL_JEDEC_OK(id)` | `firmware/hal/fm1_flash_map.h:14-20`; `firmware/src/project.c:1162` |
| `firmware/src/main.c:290-310` (`ses_flash_*`, `fl_erase4k_quiet`) | Guarded by `SES_LO..SES_HI` only | New windows; **also** call `FL_GUARD`. The trim record is read with `st_read(0xC1000, buf, 0x22C)` (SPI read, no plain window needed), then `smk_trim_load(buf)`. | `firmware/src/main.c:267-270`, `firmware/src/storage_hw.c:28-49` |
| `firmware/src/main.c:424-430` | UBOOT on OCT- + OCT+ held 5 s via `PANEL_BTN` | Works once `PANEL_BTN` maps to ids 4 and 5 | — |
| `firmware/src/main.c` main loop | — | Add `padled_flush()` once per UI frame, between LCD transfers. Add fader and wheel polling. | `firmware/src/main.c:398-416` |
| `firmware/src/bootguard.h:27-29` | `(buttons & 3u) == 1u` (FM-1 OCT- = bit 0) | `(buttons & FM1_OCT_BOTH) == 1u << FM1_BTN_OCT_DOWN` (OCTAVE- = id 4) | `sloop-fm1/firmware/src/bootguard.h:27-34` |
| `firmware/src/recovery.c:48` | `fm1_input_scan()` polled | Works with `smk_input.h:583-592` (16 ticks, waits per tick) | — |
| `firmware/src/ui.c:47-50` | `PANEL_BTN = {2..13, 0, 1}`, `PANEL_ENC = {0,1,6,2,3,4,5}` | Tonitor's `B_*` enum has the same order as Felucca's. SMK default `PANEL_BTN = {14,8,9,7,13,10,12,11,6,15,16,17,4,5}` (FX = FX, SEL = SCALE, ENV = CHORD, LFO = NOTE REPEAT, EDIT = PARA, GLO = GLOBE, HOME = PATCH, SAVE = BT, ARP, SEQ, PLAY = SEQ PLAY, REC = SEQ REC, OCT-/OCT+). Encoders: K1 = MASTER (virtual pot, encoder 0); SELECT/ALGORITHM/PRESETS = K2-K4 (ids 1-3); KNOB 1-4 = K5-K8 (ids 4-7), i.e. `PANEL_ENC = {1,2,3,4,5,6,7}`. The mapping is the owner's decision. | `firmware/src/panel.c:36-49` |
| `firmware/src/ui.c:1343-1353` (`led_dim_key`), `ui_leds` | Column bitmaps through `FM1_KEYMAP`; key LEDs as ids 14+k | Rewrite for the SMK: 19 button LEDs (on/off only) and 16 RGB pads. There are no key LEDs and no glow, so "sounding" needs another cue (pad colour). | `firmware/src/ui_input.c:414-448` (`smk_leds`), `:1535-1710` |
| `firmware/src/ui.c` keys | 27 keys, SHIFT = rightmost black key, no velocity | Keys 5..31 map to note bits 0..26 automatically. Outer keys come through `smk_extkey_take()`. Velocity is in `fm1_in.key_vel[k]`. | `firmware/src/seq.c:708+` (`key_vel`, `ext_keys_block`) |
| `firmware/src/ota.c:24-26,374` | `OTA_AREA` 0xE0000, record 0xE4F00, `"ota-FM-1_015"` (12 B) | `OTA_AREA` **0xBB000**, len 0x5000, record **0xBFF00**, `OTA_FL_LEN` 0x9D000, `OTA_PARM_ID "ota-" SMK_MODEL "_016"` copied for its length, zero-padded. **Also add SLOOP's board check**: Tonitor accepts the official loader (`ota.c:293`), so without the check the FM-1 stock package would erase BTIF/USRTRIM and overwrite USR. | `firmware/src/ota.c:27-47,383-384`; `sloop-fm1/firmware/src/ota.c:286-321,350` (`ota_board_check`, -15/-16/-17) |
| `firmware/loader/loader.c:15,20` | `"ota-FM-1_900"`; `FL_RANGE_OK` 0x4000-0x93000 or **0x93000-0xFC000** | Use `"ota-" SMK_MODEL "_900"` and `FL_LDR_RANGE_OK` (app 0x4000-0x9CFFF, records 0x9D000-0xBFFFF). Build the loader with the SMK define. | `firmware/loader/loader.c:16-34` |
| `firmware/loader/ldr_core.c:32-34,92-103` | `LDR_APP_HI` 0x93000, record window 0x93000-0xFC000 (crosses BTIF, USRTRIM and USR) | `LDR_APP_HI` 0x9D000, `LDR_REC` 0x9D000-0xC0000, plus the `_Static_assert` | `firmware/loader/ldr_core.c:34-48` |
| `firmware/app.ld:15` | XIP LENGTH 0x8DFBC | 0x97400 (+38 KiB) | `tools/build.py:238-244` |
| `tools/fm1pkg_make.py:24-25,133-136` | `APP_SLOT` 0x8DFBC, `FLASH_SIZE` 0x93000, FM-1 regions | `REGIONS["smk37"]`, `BOARDS["smk37"] = (0x9D000, 0x97400, ...)` | `tools/fm1pkg_make.py:35-63,171-173` |
| `tools/build.py:40,253,261` | `PRODUCT "FM-1_700"`, release `FM-1_7XY` | `SMK-37 Pro_7XY`; `-DBOARD_SMK37=1` for app **and** loader; `MODEL`; package name | `tools/build.py:60-66,168,207,394-430` |
| `tools/fm1_install.py:4-38,124`; `docs/js/fm1ota.js:17,105,174`; `docs/js/fm1backup.js` | FM-1 port names; 20 blocks | Accept `SMK` port names, the `SMK-37 Pro` model and `ota-SMK-37 Pro_9xx` loaders; 36-block detection; a model check on resume. Do not install FM-1 layouts on an SMK. | `tools/fm1_install.py`, `web/fm1pkg.js:211-256`, `web/fm1ota.js`; SLOOP's board-from-contents check |
| `firmware/src/usb.c:199` + strings | 1209:0003 "FM-1 Tonitor" | Optionally change the product string (owner's choice); a new PID makes hosts re-read the name | (Felucca keeps its own) |
| `firmware/src/ui.c:1429-1433` battery | 531/561/591 | Fine (stock 530/560/602). `fm1_adc_read(FM1_ADC_BATT)` returns -1 until mux C has read; `main.c` already tolerates `b <= 0`. | — |
| Console (if any) | — | `smk`, `padtest`, `leds` debug commands are useful at bring-up. Tonitor has no CDC console (`FELUCCA_CDC 0`), so consider a SysEx or telemetry equivalent. | `firmware/src/console.c:363-427` |
| Unchanged | `fm1_audio.h`, `fm1_sys.h`, `fm1_irq.h`, `fm1_guard.h`, `fm1_isr.S`, `fm1_vec.S`, `fm1_time.h`, `fm1_timer.h`, `fm1_usb.h`, `fm1_xip.h`, `fm1_multicore.h`, `fm1_core1_*`, `fm1_power.h`, `crt0.S` | Same SoC. `fm1_power.h` (Melodee's FM-1 stock supply values) is **not verified on the SMK** (O); the boot config is identical (A). | — |

---

## 3. Flash collisions: Tonitor's map against the SMK

| Tonitor range | What | SMK collision |
|---|---|---|
| app slot 0x120 + 0x8DFBC (flash 0x4120-0x920DB, plus cfg_tool) | app | None. It fits inside the SMK app area (0x4000-0x9CFFF). The package must still be **flash.bin = 0x9D000** with the SMK region table, or the SLOOP board check fails and the official loader writes the FM-1 layout. The slot may grow to 0x97400. |
| **0x97000-0x9CFFF** (sessions, first 6 sectors) | sessions | **Inside the SMK app area (PRCT).** The update loader rewrites it on every install. With a 0x97400 slot the app itself ends at ~0x9B520 and overlaps. |
| 0x9D000-0xBFFFF (sessions) | sessions | VM. Allowed in principle, but **0xBB000-0xBFFFF is the SMK OTA staging and update record (0xBFF00)**, and the whole VM is the loader's record-drop window. |
| **0xC0000-0xC1FFF** (sessions) | sessions | **PROTECTED: BTIF + USRTRIM, the factory key and pad calibration. A session write here destroys the unit's calibration.** |
| 0xC2000-0xDFFFF (sessions) | sessions | USRFLASH. Allowed (`FL_SMP`), but the stock-marker sector 0xEA000 must be released before the first write. |
| 0xE0000-0xE4FFF (OTA staging, record 0xE4F00) | OTA | USRFLASH. Not protected, but **outside the loader's record window** (0x9D000-0xBFFFF), so the loader could not drop the record. Move it to 0xBB000 / 0xBFF00 as Felucca and SLOOP do. |
| `FL_FM6` 0xE5000-0xE6FFF (allow-list only) | — | USRFLASH. Unused by Tonitor; drop it. |
| `FL_USR4` 0xE7000-0xFAFFF (allow-list only) | — | Crosses the 0xEA000 stock marker, the unlisted area and **USR 0xF4000-0xFAFFF (protected)**. Drop it. |
| `FL_GLOB` 0xFC000-0xFEFFF (allow-list only) | — | **0xFC000-0xFDFFF is USR (protected).** Drop it. |
| loader record window 0x93000-0xFC000 | loader | **Crosses BTIF, USRTRIM and USR.** `ldr_records_drop` (`ldr_core.c:92-103`) erases any sector whose last 256 B hold a CRC-valid record. Must become 0x9D000-0xBFFFF. |

### Where Felucca smk37 puts its data
(`Felucca/firmware/src/storage.c:64-183`, `fm1_flash_map.h:22-38`)

| Range | Felucca smk37 use |
|---|---|
| 0x9D000-0xAEFFF | 9 A/B pairs: projects 0x9D000-0xA4FFF, user presets 0xA5000-0xA8FFF, autosave 0xA9000/0xAA000, FM6 patches 0xAB000/0xAC000, settings 0xAD000/0xAE000 |
| 0xAF000 | **owner tag** `SMK37DATAFELUCCA` (16 B). Data that is not ours, or untagged, is cleared at first boot (`st_owner_check`). |
| 0xB0000 | FM6 voice bank |
| 0xB1000-0xBAFFF | spare (the kit's place for a fork's extra objects; avoid 0xB0000) |
| 0xBB000-0xBFFFF | OTA staging, record at 0xBFF00 |
| 0xC2000-0xE9FFF | 2 user sample slots of 80 KiB. Felucca's 3rd slot has no flash on the SMK (`eng_sample.c:47-58`). |
| 0xEA000 | erased once, before the first USRFLASH write (stock marker) |

### Options for Tonitor's sessions store (73 x 4 KiB today; `SES_MAX 73`; `sect` is a `uint8_t` index from `SES_LO`)
1. **USRFLASH 0xC2000-0xE9FFF: 40 sectors, contiguous.** Outside the loader's record-drop window, so a session sector can never be mistaken for an update record. Tonitor has no user samples, so the window is free. Needs Felucca's stock-marker release (`st_stock_tagged`, `st_stock_intact`, `st_stock_release`) and an owner tag.
2. **VM 0x9D000-0xBAFFF: 30 sectors.** Inside the loader's record window. Felucca accepts this; the risk is a session sector whose last 256 B happen to form a valid record (CRC16 plus the 0x5441 tag).
3. **Both, through a sector-to-address map** (`ses_addr(s)`): 30 + 40 = **70 sectors**. That is close to today's 73, so the code keeps the "one record a sector at most" logic.
   - The owner tag goes in a sector of its own, for example 0xBA000 or 0xAF000-style.
   - Every allowed window must be asserted with `FL_WIN_SAFE`.
   - `backup.c` (`SES_LO + sect*SES_SECT` at :85,96,109,118) must use the same map.

Either way:
- Drop `FL_GLOB`, `FL_FM6` and `FL_USR4`.
- Keep `FL_GUARD`.
- Tell the owner that the first boot clears the stock firmware's settings, or whatever any other firmware left in those ranges.

0xEB000-0xF3FFF is unlisted and not in the kit's protected list, but the reference ports never write it (01:51, 88). Avoid it unless the owner accepts an unverified range.

---

## 4. Pitfalls (kit docs/08-pitfalls.md plus Tonitor specifics)

1. **Protected flash.** Never erase or program 0x0-0x3FFF, 0xC0000-0xC1FFF, 0xF4000-0xFDFFF or 0xFF000+. Keep the allow-list, the asserts and the independent `FL_GUARD`. Never widen them. (AGENTS.md:12-15; 08:9-11)
2. **The official-loader hole.** The FM-1's stock package has the same head and the same official loader as the SMK's. Without SLOOP's `ota_board_check`, Tonitor (which accepts the official loader) would let it erase BTIF/USRTRIM and overwrite USR. Felucca smk37 **lacks** this check; SLOOP has it (`sloop-fm1/firmware/src/ota.c:286-321`).
3. **Update record name.** It must be `ota-SMK-37 Pro_016` (18 B). With `ota-FM-1_015`, M-Vave's tool may not recognise the official loader during a return to stock. (02:59-62)
4. **Pins.**
   - PB1: never drive it and no pull-down. Don't copy `fm1_adc_init`.
   - PB6: the LCD clock. `fm1_adc_init` would turn it into an analog input.
   - PA2: matrix row 0. Tonitor's `fm1_lcd_hw_init` drives it low as the "backlight".
   - PH8 (and PA9, PH6, PH9): matrix rows. No UART RX, never outputs.
   - PC9: keep it analog (ADC ch8), never an SPI1 clock.
   - PC7: MIDI OUT now, no longer LCD CS. (AGENTS.md:19-20; 01:172-173,389)
5. **ADC timing.** Use `0x400D`, not the FM-1's `0xF04E`. Otherwise pads and faders read nothing (`adc_miss` rises). (01:308-311)
6. **Scan order.** It sets the matrix codes; a different order shifts them by one column. (01:189-193)
7. **Encoders: 4 counts per detent.** The FM-1's learned-rest decoder in Tonitor does not apply. Flip the sign if a knob runs backwards. (01:269-271)
8. **Frame-rate constants.** SMK 625 frames/s against the FM-1's 909. Check Tonitor constants counted in frames (for example `FM1_REST_FRAMES`). (08:69; AGENTS.md:237-239)
9. **Button loops.** Loops over `14u` buttons miss FX, SEQ, PLAY and REC (ids 14-17). An LED array of `FM1_NCOL` overflows (SMK LED ids go to 18 buttons and 16 pads). Write SMK LED code; don't just recompile. (08:61-67)
10. **Pad rows.** Top row = hardware pads 0-7. Convert in one place (`SMK_PAD_OF_WHITE`/`SMK_WHITE_OF_PAD`). Check with `padtest`. (08:80)
11. **Pad notes.** A pad must end the note it started, even if the layout changed meanwhile (`smk_pad_note[]`). (03:211-212)
12. **WS2812 latch.** Use ~250 bytes of zeros, not the stock 36. Send only between LCD transfers, with CS high. Never switch the pin group mid-DMA. (08:79; 01:150-158)
13. **Fader filter.** It must be signed. An unsigned IIR wraps when the fader moves down. (01:358-359)
14. **Wheels.** Travel differs per unit; self-calibrate, and the mod wheel is inverted. (01:361-371)
15. **No TRS MIDI IN on the SMK.** Tonitor 0.51's TRS input and clock-in are lost. Tell the owner. MIDI OUT on PC7 is unconfirmed on a unit. A blocking `putbyte` must never sit in the audio or UI path. Guard the TX hook from the loader build. (08:84-87)
16. **RAM budget.** Tonitor has about 1.6 KB of `.data`+`.bss` free. The SMK layer costs ~1.2 KB of RAM in Felucca. Put `padled_buf` (634 B) and large SMK buffers in POOL. (03:228-229; HANDOFF §2)
17. **Owner tag.** Give Tonitor its own data family. The first boot clears untagged or other firmware's data in the store windows. Use a family of ≤ 7 characters and compare 16 bytes. (08:10,14-15,18)
18. **Package blocks.** 20-block identity for Tonitor's own packages. Installers must read stock v016 packages as 36 blocks and check only the flash.bin and ota.bin CRCs. (02:64-86)
19. **Installer model checks.** Port filters must accept `SMK`. `resume()` must check the loader model against the package. Parse identities up to the first NUL. Match port names as case-insensitive substrings (Windows `MIDIIN2 (...)`). (08:24-35)
20. **Per-model packages.** Pro, Elite and MKE (`SMK_MODEL`). The Starrykey uses the Pro build. (08:26-29)
21. **The trim record.** Read it, validate it, never write it. Tonitor reads flash over SPI (`st_read`, `fl_read_ram`); that works for 0xC1000 without the XIP plain window. Felucca uses `fm1_xip_ptr` after `fl_plain_window_init` from 0x9D000.
22. **Loader record drop.** It scans the record window and erases any sector with a valid-looking record. Keep the window inside the VM, and preferably keep sessions out of it (option 1 above).
23. **The UBOOT soft key does not work on the stock app.** Method A backup therefore needs the custom loader staged first. The SMK USB_KEY dongle route is untested.
24. **No VBUS on the SMK's USB-C.** This matters for any USB-host idea. (08:82)
25. **Docs that contradict.** Felucca's `smk_input.h:82` ("K1..K8 order to confirm") and some console texts about pad rows are stale. Trust 01-hardware.md's verified rows.
