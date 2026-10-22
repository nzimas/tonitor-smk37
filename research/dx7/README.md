# New FM families learned from the DX7 collection (proposal, for listening)

Nothing here is installed or wired into the firmware. Six candidate FM families, each learned from a group of
voices in the DX7 collection, are written as recipes in `fm_dx7.c`. Generation 6 wiring is ready in `gen6.patch`
but **not applied**: `firmware/` is unchanged on this branch. Listen first, pick the families you want, then see
"To build it" below.

**Listen:** `wav/` holds 8 sounds per family (48 in all), drawn exactly as the firmware would draw them: the
generator on generation 6, the level measure, and a layer's gain and limiter. Each is a note held 6 s and released
for 4 s. File names read `FAMILY-n-path-note.wav`; for every family, 1–2 are on the open path, 3–4 ambient, 5–6
experimental, 7 open, 8 ambient. `wav/list.txt` lists each one's peak level and patch (algorithm, each operator's
ratio and level, pitch envelope). `wav/sources/` has three of the collection's own voices per family, played
through the same core at middle C, so you can hear what each family learned from.

## The collection

- `dx7-presets.bin` (vascular-android): 16,384 voices; 9,606 are different (compared on the 118 bytes before the
  name).
- No names. This copy has none, and neither has its source, `DX7.afx` from the DX7-SuperCollider project (the
  local copies in interwoven/, wildrider-*/ and eap-mc2/ were checked: digits only, one voice a line). So the
  groups below are described by measurement only.
- 11% of all operators are in fixed-frequency mode, and 2,573 of them sit at 1–9 Hz. That is far more than a
  factory library, and it fits a collection scraped from every DX7 bank online, edits and random patches
  included. Those slow fixed operators turned out to be the most useful finding (SHAPER, below).

## How the families were found

1. `unpack.py`: bulk dump to the engine's 155-byte voice layout, duplicates removed.
2. `dx7scan.c` (built on the host harness, like `tests/golden`): every voice rendered through the firmware's own
   FM core (`fm_measure`, then middle C held 4 s, released 2 s). Measured: loudness over time (attack, level at
   0.5 s and 3.8 s, release), spectral centroid at 80 ms, 0.6 s and 3 s (brightness and how it falls),
   inharmonicity (energy off the harmonic grid), and how much the level jitters while held.
3. `analyse.py`: the parameters (algorithm, carriers, ratios, fixed operators, modulation levels, envelopes,
   pitch envelope, LFO) next to the measurements; k-means into 36 groups (`clusters.txt`).
   - About 7,200 voices fall in conventional groups: sustained and harmonic (organ, brass and string types),
     harmonic and decaying (piano, electric piano, plucked and bass types). These were set aside.
   - 2,360 are unconventional by at least one measure: inharmonic (1,536), pitch envelope moving (605), slow
     attack over 1 s (598), level moving (472), deep pitch LFO (268).
4. `subset.py` re-clusters those 2,360 on sound and structure together (18 groups, `subset.txt`). `idioms.py`
   counts the programming idioms behind them.
5. Groups that the current FM families already cover were set aside (see the end). Six idioms that none of them
   make were kept, and `learn.py` turns each into tables (the block between the markers in `fm_dx7.c`):
   - **24 skeletons**: algorithm, feedback, and each operator's frequency (ratio, or fixed in Hz) and level
     (off, under or full). The most common in the group come first, never two from the same 32-voice bank (the
     collection holds many edits of one voice).
   - **12 carrier and 12 modulator envelope shapes**: k-means centroids of the group's envelopes. Each is an
     average shape, not any one voice's.
   - the group's ranges for rate scaling and detune; its feedback and LFO habits (wave, speed, how often and how
     deep it moves pitch and level).
   - for DIVE and CREEP, 24 pitch-envelope shapes from the group.

A recipe draws one skeleton. Each operator gets an envelope shape, bent ±6 on rates and ±4 on levels, plus a
level within its off/under/full band. Ratios off the harmonics move by up to 1.5% half the time, and fixed
frequencies by ±5 fine steps. The LFO is redrawn from the group's habits. The recipe then sets its own trait (below). After that,
every generation-1+ step applies as for any family: the keyboard scaling and high-ratio care (`fm_shape1`), the
mutations, generation 3's breadth, the path leaning (`fm_ambient` / `fm_experimental`), the never-silent check and
`fm_measure`. A recipe never reproduces a collection voice: envelopes, levels, scaling, detune and LFO are all
redrawn.

## The six candidates

Path weights below are what `gen6.patch` gives them (out of each path's total; the existing weights are kept).

`compare.txt` measures 6 renders of every family the same way: brightness, inharmonicity, attack, level left at
3 s, level movement, pitch travel and brightness fall, each scaled to its spread. On those averages each new
family lies 1.0–1.5 from its nearest existing family. Two existing families are typically 2.7 apart, and the
closest existing pair is 0.57. This measure is coarse: it cannot hear SHAPER's rolling timbre or LADDER's chord,
and its pitch travel is the loudest partial's, which jumps. Treat it as a sanity check only. The WAVs decide.

### SHAPER — slow wave-shaping (learned from 1,481 voices)

**What it does:** one or two carriers are fixed at 1–9 Hz (sometimes up to 19 Hz) and modulated by operators that
follow the key. A carrier that barely turns acts as a sine wave-shaper on its modulators. As its slow phase turns,
the tone rolls between its odd and its even harmonics a few times a second, at the same pace across the
keyboard. It sounds harmonic but restless: a pulsing, vowel-like rolling of the timbre with no vibrato.
Envelopes go from organ-held to slowly decaying.

**Why it is new:** no current family is built on a slow fixed carrier. The mutations can fix an operator, but only
at 10 Hz or above. STATIC fixes modulators at audio rates (ring modulation), and ITERATIF pulses the level with the
LFO. Here the motion is in the timbre, comes from the
carrier's own frequency, and does not depend on the LFO.

**Weights:** open 3, ambient 3, experimental 4.

### LADDER — partials on a ladder (130 voices)

**What it does:** 4–6 carriers on a ladder of ratios (the additive algorithms, 21–32). The ladder is one of three kinds:
near-unison (1.00 / 1.07 / 1.14 / 1.21 …, a beating microtonal cluster), an octave stack (1 / 2 / 4 / 8 / 16 /
32), or an uneven set (8 / 6.66 / 6 / 5.75 / 5.45, a chord off the scale). The carriers' envelopes are
staggered, as in 47% of the group (enter one by one) and 63% (die one by one): the higher partials arrive later
and die sooner. In 40% of draws the recipe builds a ladder of its own of one of the three kinds instead of
taking one from the table.

**Why it is new:** SPECTRE detunes carriers a few cents around the same ratio; METAL scatters carriers on
inharmonic bell ratios. LADDER's steps are regular and wide (up to a whole tone), and its staggered envelopes make
a chord or cluster that changes shape as it sounds.

**Weights:** open 3, ambient 4, experimental 2.

### DIVE — the pitch envelope's full reach (60 voices)

**What it does:** the pitch envelope glides more than an octave and a half, up to the DX7's four octaves either
way, at an audible pace (rate 20–96), and lands within an octave of the note. You get falls from far above,
dives from below, a pitch thrown up and caught, and risers. The operators are the group's: often fixed or
high-ratio modulators, so the dives sound like lasers, bombs and sirens. A third of draws are turned upside down.

**Why it is new:** WARP keeps its pitch envelope within 33–67 ("bent, never thrown out of hearing"). GLISSE's
journey (now inside WARP) is slow, and IMPACT's fall is short. DIVE uses the whole range. On the ambient path,
`fm_ambient` narrows it to a third, as for every family.

**Weights:** open 3, ambient 0, experimental 5.

### REVERSE — swells until it is let go (123 voices)

**What it does:** every carrier rises for as long as the key is held: slow first stages (rates 12–34, sometimes in
two steps), held at the top, then cut on release (rate 50–80). In 60% of draws the modulators open with it, so
the sound brightens as it grows. In the audition the attack reaches its peak after 2–5 s. Played and released,
it sounds like tape run backwards.

**Why it is new:** BLOOM opens the timbre over seconds and closes it halfway; the pads (DRONE, TIDE) swell and
hold. None keeps rising to the release and then stops. On the ambient path the tails are lengthened, as for every
family, so there it is a long swell with a long tail.

**Weights:** open 3, ambient 4, experimental 2.

### CREEP — the pitch leaving slowly (58 voices)

**What it does:** the group's pitch-envelope shapes, with their slow stages (rate 12 or less in the collection)
redrawn at rates 0–3: 0.6–1.7 semitones a second. Each moves towards a level within about an octave of the
note. A held note drifts a few semitones over several seconds, or up to an octave over 10–20 s, and sometimes
comes back on release. The carriers reach their level within a second and hold it, so only the pitch moves. Many
of the group's skeletons also use slow fixed modulators (1–40 Hz), which add an uneven wobble.

**Why it is new:** GLISSE's and WARP's pitch envelopes move at 3–65 semitones a second. CREEP is 2 to 100 times
slower, about the speed of a tape machine running down. It suits ambient drones that should not stay in tune
with themselves.

**Weights:** open 3, ambient 4, experimental 2.

### CLANG — Chowning's bells as DX7 programmers made them (194 voices)

**What it does:** carriers modulated by operators on ratios well off their harmonics (at least 0.1 off the
half-harmonic grid: 1.36, 1.3, 3.42, 1.67, 6.28 …), the index dying with the note. Struck, decaying, clangorous.
The skeletons include tubular-bell and gong-like pairs and some with slow fixed modulators that warble the
partials.

**Why it is new:** METAL builds bells by adding carriers on inharmonic ratios (additive, few sidebands). MALLET is a
harmonic bar with a quick knock. CLANG puts the inharmonicity in the modulation, so a dense spray of sidebands
collapses as the index dies. It is the nearest to an existing family (METAL); listen with that in mind.

**Weights:** open 3, ambient 2, experimental 4. On the ambient path it keeps its strike half the time, like
METAL. Its DETUNE macro becomes INHARMONIC, as for METAL; DIVE's does too.

## What was set aside, and why

- **Conventional instruments** (about 7,200 voices): brass, strings, organs, pianos, electric pianos, basses,
  plucks. Their skeletons are in the clusters if you ever want them.
- **Sample-and-hold computer burble, chopped tremolos, noise** (subset groups 10, 12, 5, 4, 8): BRUIT, SHARDS,
  GLITCH and ITERATIF already make these.
- **Very short noisy hits** (groups 7, 11, 16): IMPACT and GLITCH.
- **Deep pitch-LFO sirens** (group 6): WARP's siren.
- **Chords of fixed-frequency carriers** (all six fixed at 1–3 kHz, for example): the same pitch on every key.
  These are alarms and beeps, not playable.

## Cost and compatibility

- **Flash:** about 10 KB for all six (estimated: `fm.c` compiled at `-Os` for 32-bit ARM, with and without:
  +5.3 KB code, +5.2 KB tables). The JieLi toolchain only runs in Docker, so the real number needs a build. The
  tables are 24 skeletons × 16 B + 24 envelope shapes × 8 B per family, plus 2 × 24 pitch shapes × 8 B. Halving
  the skeletons and shapes would save about 2.5 KB. Roughly per family: 0.6–0.8 KB of tables and 0.3–0.9 KB of
  code, plus about 1.2 KB of shared drawing code. The app slot is 581,564 B.
- **RAM:** none (tables are `const`, no new state). **CPU:** none; the sound runs on the same core, and the host
  check's worst FM block cost is unchanged (2.3 µs).
- **Earlier sounds:** generations 0–5 draw bit-identically with generation 6 wired in, checked with the golden
  test. Generation 5 had no fingerprint, so this branch adds `tests/golden/gen5.txt` (made from the unchanged
  build-25 code) and runs it in `tests/run_tests.sh`.
- **Host tests with generation 6:** `host check` (40 seeds of every family, none silent, none clipping, none
  running away): 0 failures. The 48 audition sounds: no clipped samples.
- **This branch's own tests** (`tests/run_tests.sh`, firmware unchanged): "tests: ok", golden 0–5 included.
  One step had to be skipped: `tests/check_target_ir.sh` already fails on the base commit (19bca04), because of
  a 64-bit division in `sh_play` (SHADOW, `react.c`, build 24). It is unrelated to this work and needs its own
  fix.

## To build it

After choosing:

1. Copy `research/dx7/fm_dx7.c` to `firmware/src/` and apply the patch: `patch -p1 < research/dx7/gen6.patch`.
   It touches `dsp.h` (GEN_NOW 6, GEN_N 7), `fm.c` (the families after `FF_N5`, their weights `FF_W6`, names,
   `fm_generate`, the INHARMONIC macro for DIVE and CLANG, CLANG's strike on the ambient path, the include),
   `mod.c` (how often each comes with a modulation), `golden.c` and `run_tests.sh`.
2. Write generation 6's fingerprint: `build/golden 6 > tests/golden/gen6.txt`.
3. To drop a family: remove its line from `fm_dx7.c`, its `case` in `fm_generate`, and its name, weight column
   and `MODP_FM` entry (enum, `FF_NAME`, `FF_W6`, `mod.c`). Or keep it in the code with weight 0 on every path.
4. `research/dx7/audition.sh` does steps 1–2 in a copy (`build/gen6`), checks generations 0–5, and renders the
   audition again.

## Files

| | |
|---|---|
| `fm_dx7.c` | the six recipes and their learned tables (to go into `firmware/src/`) |
| `gen6.patch` | generation 6 wired into `fm.c`, `dsp.h`, `mod.c` and the tests |
| `audition.sh`, `audition.c` | builds the copy with generation 6, checks generations 0–5, renders `wav/` |
| `unpack.py`, `dx7scan.c`, `analyse.py`, `subset.py`, `idioms.py`, `learn.py`, `dx7lib.py`, `compare.py` | the analysis (order: as in "How the families were found"; they write to `build/dx7`) |
| `clusters.txt`, `subset.txt`, `idioms.txt`, `learned.txt`, `compare.txt` | their results |
| `wav/` | the audition (not committed: about 60 MB; `wav/list.txt` is) |
