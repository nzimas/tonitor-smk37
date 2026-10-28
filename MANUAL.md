# Manual

## Playing it

Tonitor turns the SMK-37 Pro into six tracks. Each track has its own engine and sound, its own level, pan,
filter, granulator, crush and distortion, and its own sends to a master delay and a master reverb. You choose a
track on the top row of pads and play it on the keys. The other tracks go on sounding: held notes, latched
notes, loops and sequencers keep playing while you choose and play another track.

The tracks start on FM, ANIMA, CALIGULA, HIVE, DRUID and BAYAREA (tracks 1 to 6), with a new sound each, on the
OPEN path, in a scale and root drawn at random. Each track has a colour: 1 yellow, 2 cyan, 3 pink, 4 green, 5 orange, 6 lilac.

### The panel

This manual calls each control by its Tonitor name. Several buttons carry another word printed on the
SMK-37: the button printed SCALE is MACRO in Tonitor, and the button printed FX opens the SCALE page. The table
gives both.

| Tonitor name | Printed on the SMK-37 | Where |
| --- | --- | --- |
| track pads 1–6 | pads | top row of pads, 1st to 6th |
| AMBIENT, EXPERIMENTAL | pads | top row of pads, 7th and 8th |
| pads 9–16 | pads | bottom row of pads, left to right |
| **CHORD** | ARP | top row of buttons, 1st |
| **LOOP** | NOTE REPEAT | top row of buttons, 2nd |
| **MACRO** | SCALE | top row of buttons, 3rd |
| **BALLSY** | CHORD | top row of buttons, 4th |
| **BRICKLAYER** | GLOBE | top row of buttons, 5th |
| (no function) | BT | top row of buttons, 6th |
| **HOME** | PATCH | bottom row of buttons, 1st |
| **SAVE** | PARA | bottom row of buttons, 2nd |
| **SCALE** | FX | bottom row of buttons, 3rd |
| **SEQ** | SEQ | bottom row of buttons, 4th |
| **PLAY** | SEQ PLAY | bottom row of buttons, 5th |
| **REC** | SEQ REC | bottom row of buttons, 6th |
| **SHIFT** | KNOB BANK | |
| **FADER BANK** | FADER BANK | |
| **<** and **>** | < and > | under the screen |
| **OCTAVE−**, **OCTAVE+** | OCTAVE−, OCTAVE+ | |
| **K1–K8** | the eight knobs | |
| fader 4 | the rightmost fader | |

The two rows of six buttons glow dimly while idle, so they can be found, and light fully while what they control
is on. SHIFT is lit while held. Faders 1 to 3 and the pedal input have no function.

### The controls

| Control | What it does |
| --- | --- |
| **track pads 1–6** | choose track **1 to 6**. The chosen track's pad is lit in its colour; a track that sounds is half lit; the others are dim. Choosing a track closes any page and goes back to the track view. **Held** (0.8 s): **copy** the track, or **paste** the copy onto it (see Copying a track). |
| **AMBIENT** (pad 7), **EXPERIMENTAL** (pad 8) | the chosen track's **generative path** (white: lit while chosen, dim while not). Pressing the lit one goes back to **OPEN**. The screen says AMBIENT, EXPERIMENTAL or OPEN. See Generative paths. |
| **<** and **>** | the chosen track's **engine**: **FM**, **ANIMA**, **CALIGULA**, **RUSSOLO**, **DRUID**, **BAYAREA**, **HIVE** or **OBRADOR**, a step a press. Each track keeps a sound of each engine, so stepping back finds the sound it had. On the pages that use them, < and > do the page's job instead (sessions, presets, GLOBAL SETUP, the MOD page, the REACT page). |
| **SHIFT + < / >** | in the track view, a **new sound of the family before or after** on the track's engine. At the first or last family, a new sound of that family again. |
| **HOME** | a **new sound** for the chosen track, on its engine and path. The screen says NEW. HOME is lit while held. |
| **SHIFT + HOME** | a **new sound of the same family** (kind, species) as the chosen track's sound. The screen says SAME FAMILY. |
| **OCTAVE− / OCTAVE+** | the chosen track's **octave**, as far as every one of the 37 keys stays within MIDI's range. The screen says OCT +1, OCT −2, and shows the octave at the top right (nothing at 0). OCTAVE− is lit below the middle, OCTAVE+ above. |
| **SHIFT + OCTAVE− / OCTAVE+** | **every loop, latch and sequencer** of every track down or up **one step of the scale** (TRANSP +2). See The loopers. |
| **SHIFT + a track pad held + OCTAVE− / OCTAVE+** | **that track's** loops, latches and sequencer only (TRANSP T3 +2). |
| **the keys** | play the chosen track **in the scale chosen**. Every key, black ones too, plays the next step of the scale. See The keys. |
| **SHIFT** (KNOB BANK) | hold SHIFT and press a key: the note **latches**. Hold keys and press SHIFT: they latch. **SHIFT tapped twice** lets every latch go. See Latched keys. |
| **K1** | the chosen track's **level**: 1.5 dB a click, from OFF up to +4.5 dB. The screen says it as the knob's share of its turn (VOL 74 %); a track starts at 87 % (0 dB). |
| **K2** | the chosen track's **pan**, left to right in 41 steps (PAN L 50, PAN C). Tracks start spread out: 1 in the middle, 2 half right, 3 and 4 a quarter left and right, 5 and 6 near the middle. |
| **K3** | the chosen track's **filter** (see The filter and the crush). |
| **K4** | the chosen track's **granulator** (see The granulator). |
| **K5** | the chosen track's **crush** (see The filter and the crush). |
| **K6** | the chosen track's **distortion**, 0–100 (0, off, at the start). See The distortion. |
| **K7** | the chosen track's **delay send**, 0–100 (0 at the start). See The delay. |
| **K8** | the chosen track's **reverb send**, 0–100 (40 at the start). See The reverb. |
| **CHORD** | pressed: the chosen track's **chord mode** on or off (CHORD lit while on). **Held** (0.6 s): the **chord forms** on the bottom row of pads, and chord mode on. Held again: the pads freed. See Chords. |
| **LOOP** | the chosen track's **loop options** on the bottom row of pads (LOOP lit while they are up; LOOP PADS). Pressed again: the pads freed (PADS FREE). See Loop options. |
| **MACRO** | the **macros** on or off (MACRO lit while on; EDIT in red over the sound on the screen). On, K1–K8 turn the eight macros of the chosen track's sound. See Macros. |
| **BALLSY** | pressed: the chosen track's physics sequencer **BALLSY** on (its page up) or off. **Held** (0.6 s) while on: its page up or down. Lit while on, blinking while it plays. See BALLSY. |
| **BRICKLAYER** | pressed: the chosen track's step sequencer **BRICKLAYER** on (its page up) or off. **Held** (0.6 s): its options on the bottom row of pads (turning it on if it was off); held again, the pads freed. Lit while on, blinking while it plays. See BRICKLAYER. |
| **FADER BANK** | pressed: the **ENVELOPE** page up or down (FADER BANK lit while up). See Envelope. **Held** (0.6 s): the **REACT / SHADOW / ARP page** and its pads on the bottom row, up or down. |
| **SHIFT + FADER BANK** | **REACT, SHADOW or ARP** (the mode shown on its page) **on or off**. FADER BANK blinks slowly while it is on. See REACT, SHADOW and ARP. |
| **REC** | the chosen track's **looper**: arm a new loop, end it, arm the next. See The loopers. |
| **REC held** (1 s) | **empty** the chosen track's looper: the whole screen flashes LOOP n / EMPTY. |
| **SHIFT + REC** | the chosen track's loops **VARY**, on or off (VAR at the top right while on). See VARY. |
| **PLAY** | pressed and let go: stop the chosen track's loops, or play them again, each from its start. PLAY is lit while they play. On BALLSY's or BRICKLAYER's page, PLAY starts and stops that sequencer; while the chosen track's sequencer plays, PLAY stops it from any page. |
| **PLAY held** (0.6 s) | the chosen track's **loops on the bottom row of pads**, to take each out or bring it back (OVERDUBS); held again, the pads freed (PADS FREE). While PLAY is held, the screen shows the loops and the keys take them in and out too. |
| **SHIFT + PLAY** | from any track: if any loop or sequencer plays, every loop and sequencer of every track stops (ALL STOPPED; the loops are kept). If none plays, every loop and every sequencer that is on starts, together, from their starts (ALL PLAYING). |
| **SCALE** | the **SCALE page** (SCALE lit): K1 the root, K2 the scale. SCALE again, or a track pad, goes back. See Scales. |
| **SCALE held** (0.8 s) | a **new scale and root**, drawn at random; the SCALE page shows them for 1.5 s. See Scales. |
| **SHIFT + SCALE** | the **PRESETS** page of the chosen track's engine. See Presets. |
| **SAVE** | the **SNAPSHOTS** page (SAVE lit): the 16 pads are the session's snapshots. See Snapshots. |
| **SAVE held** (0.8 s) | **save** the session (on the SNAPSHOTS page: store the snapshot framed). See Sessions. |
| **SHIFT + SAVE** | the **SESSIONS** page (SAVE lit). See Sessions. |
| **SHIFT + SAVE held** (0.8 s) | **GLOBAL SETUP** (SAVE lit). See Global setup and MIDI. |
| **SEQ** | pressed again and again: the **DELAY** page, the **REVERB** page, the **MOD** page, then back to playing (SEQ lit while one is up). On the DELAY and REVERB pages, K3–K6 set their four values. |
| **SHIFT + SEQ** | the chosen track's **modulation thrown anew** (MOD RANDOM). See Modulation. |
| **pitch wheel** | bends the chosen track ±2 semitones. |
| **mod wheel** | on the chosen track: a vibrato (5.5 Hz, up to 0.4 semitone) and the first macro pushed up. |
| **fader 1** | **MORPH** by hand: with the morph time at MANUAL, from the snapshot you are on to the one whose pad you tapped. See The faders. |
| **fader 2** | **SPACE**: every track's reverb and delay sends, the reverb's decay and the delay's repeats, raised together. |
| **fader 3** | the **master filter**: low-pass below the middle, off at the middle, high-pass above. |
| **fader 4** | the **master level**. |
| **OCTAVE− at power-on** | the **RESCUE** mode. **OCTAVE− and OCTAVE+ held 5 s**: **UBOOT**. See Starting, rescue and UBOOT. |
| MIDI in (USB) | notes play the tracks by channel; CCs set level, pan, macros, filter and sends. See Global setup and MIDI. |

### The keys

All 37 keys play. With a scale chosen, every key, black or white, plays the next step of the scale, up from
the root on the lowest key. The keyboard is one continuous run of the scale: a seven-note scale spans a little
over five octaves. With **CHROMATIC**, the keyboard plays as printed, C3 to C6.

Each track's octave 0 places the 37 keys for the scale's length. A twelve-note scale (CHROMATIC) starts at C3. A
seven- or eight-note scale starts an octave lower. A five- or six-note scale starts two octaves lower. OCTAVE−
and OCTAVE+ move the chosen track from there, as far as every key stays in MIDI's range. A change of scale or
root brings each track's octave back into range if it has to.

The keys answer velocity. GLOBAL SETUP's VELOCITY section sets the curve and the range (see Global setup and
MIDI). The softest note a key plays is velocity 16, so a light touch is never lost on a sound that answers
velocity strongly. A strike slower than 150 ms is taken as one of 60 ms.

Voices come from one pool of twelve shared by all the tracks (CALIGULA and RUSSOLO are one organism a track,
outside the pool). A note takes a free voice, so a track has as many as its notes need, and a new note never
cuts the release of the one before while voices are free. A key let go ends its note on the track it was
played on, so a held note keeps sounding while you choose and play another track. See Voices and load.

### Generative paths

Each track has a path that leans what **HOME** draws, on every engine. A path changes nothing already playing:
it shapes the next sounds drawn on that track. Pads 7 and 8 show the chosen track's path.

| Path | Pad | Leans towards |
| --- | --- | --- |
| **AMBIENT** | 7 | sounds for ambient and dark ambient: ethereal, melodic, often melancholic. FM favours drones, beating spectra (SPECTRE), voices (CHOIR), bowed, hollow and tidal tones, mallets, LADDER, REVERSE and CREEP, never BRUIT, IMPACT, FRICTION, SHARDS, STATIC or GLITCH; then softer attacks, long tails, darker modulators, slow LFOs, no sirens, often an octave down. ANIMA draws mostly HAZE, GOLDEN, VOYAGE, TIDE, CHOIR, BOWL, FOG and DRIFT, with a darker tone, more space, a slower hand and long releases. CALIGULA draws SPORE, VEIL and LITANY most; tuned on roots, fifths, octaves and unisons; slow swells and a slower hand. RUSSOLO draws CHORD most, then DUST, CRACKLE, BLIPS, WALK and WANDER, clean and slow. DRUID favours BOWED, HARP, DRONE, BELL and BOWL with long decays. BAYAREA favours BONGO, CHIME, SWELL and DRONE. HIVE favours PAD, DRONE, WASH, SPECTRA and TIDES, slower and darker |
| **EXPERIMENTAL** | 8 | sounds for electroacoustic and acousmatic music: edgier, gnarlier. FM favours WARP and GLITCH most, then bruits, frictions, shards, static, SHAPER and CLANG, brighter, bent more often, never SPECTRE, CHOIR, BOWED or TIDE. ANIMA draws mostly GRIT, SWARM, SHARDS, MACHINE, BITS and HOWL, and makes them wild. CALIGULA draws WIRE, THORN, HIVE and RUST most; clusters and tritones; sharper and more driven. RUSSOLO draws NOR, XOR, CLANG, BEND and QUAD most, crushed and abrupt. DRUID favours RATTLE and PLATE with shorter decays. BAYAREA favours PLUCK and FOLDED with more drive and folding. HIVE favours STAB, CLUSTER, METAL, GROUND and AIR, with more noise and drive |
| **OPEN** | neither lit | every family, kind and species alike. Every track starts here |

The engines section gives each engine's families and what the paths do to them. Sessions keep each track's
path, and the path each of its sounds was drawn on.

### Macros

Press **MACRO** (lit while on) and the eight knobs stop being the track's mix and effects: they become **eight
macros** that reshape the chosen track's sound while it plays, under a held note. The band under the engine
says **EDIT** in red as long as the macros are on. Press MACRO again to give the knobs back.

Each macro turns from the middle, which is the sound as HOME drew it; either way transforms it. Each answers
early in its turn (a quarter of the way gives half the change). While a knob turns, the whole screen is its dial,
with the macro's name for this sound, the track and how far it is turned (+40 %, −25 %, AS DRAWN).

The macros are chosen for each engine, and for some by the kind of sound:

| | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| **FM** | BRIGHTNESS | DETUNE (INHARMONIC on metallic and noisy families) | SATURATION | MOTION |
| **ANIMA** | TONE | GRAINS, NOISE, FM, FORMANT or DRIVE (by kind) | RESONATOR | MOTION |
| **CALIGULA** | WAVEFORM | MOD DEPTH | FEEDBACK (DELAY for SPORE, VEIL, LITANY, MOTH) | TOUCHES (GATE RATE for HIVE, MOTH, WIRE) |
| **RUSSOLO** | the kind's first parameter | the kind's second parameter | CRUSH | RANDOMNESS |
| **DRUID** | STRUCTURE | BRIGHTNESS | DAMPING | POSITION |
| **BAYAREA** | SHAPE | FOLDS | FM | DECAY |
| **HIVE** | DETUNE | MOVEMENT | MIX (NOISE on GROUND and AIR) | CUTOFF |
| **OBRADOR** | CUTOFF | MORPH | RESONANCE | DETUNE |

| | K5 | K6 | K7 | K8 |
| --- | --- | --- | --- | --- |
| **FM** | ATTACK | RELEASE | LFO RATE | VELOCITY |
| **ANIMA** | ATTACK | RELEASE | GRAIN SIZE | PHASER |
| **CALIGULA** | ATTACK | RELEASE | SPREAD | DELAY TIME |
| **RUSSOLO** | ATTACK | RELEASE | CHORD | HOLD |
| **DRUID** | EXCITATION | BOW | DUST | MUTE |
| **BAYAREA** | ATTACK | FM RATIO | SPRING | DRIVE |
| **HIVE** | RESONANCE | FILTER ENV | RING | SHIMMER |
| **OBRADOR** | FILTER ENV | LFO | DRIVE | VINTAGE |

Each engine's section below says what its macros do. The **mod wheel** pushes the first macro up, on top of its
knob. Each track keeps the macros of each of its eight sounds. **HOME** draws a new sound with its macros in the
middle. Sessions and presets keep the macros with the sound. MIDI CC 16–19 and 20–23 set macros 1–4 and 5–8.

The macros act only in the track view. On a page (ENVELOPE, DELAY, REVERB, MOD, BALLSY, BRICKLAYER, REACT), the
knobs are the page's.

### Envelope

Press **FADER BANK** (lit while the page is up) to open the **ENVELOPE** page; press it again, or a track pad,
to close it. The page shows ENVELOPE and the track's number, the amplitude envelope of the chosen track's sound
drawn as a curve, and ATTACK, DECAY, SUSTAIN and RELEASE with their values.

As HOME drew the sound, the page reads the sound's own envelope and says **AS DRAWN** (in grey): for FM, the
envelope of the loudest carrier; for CALIGULA, the average of its pairs' attacks and releases. CALIGULA and
RUSSOLO have no decay or sustain stage (shown as 5 ms and 100 %).

| Knob | | Range |
| --- | --- | --- |
| **K3** | **ATTACK** | 1 ms – 10 s |
| **K4** | **DECAY** | 5 ms – 10 s |
| **K5** | **SUSTAIN** | 0–100 % |
| **K6** | **RELEASE** | 5 ms – 20 s |

The times are on logarithmic scales. K1 and K2 stay the track's level and pan. The value just turned is drawn
black on white for a moment.

The first turn starts from the values shown and replaces the sound's own amplitude envelope with the track's:
the attack is linear, the decay and release are exponential, the release ending 60 dB down. FM's carrier
envelopes and ANIMA's envelope are then held at full; CALIGULA and RUSSOLO are kept playing through the release
and then let go. Once the track's envelope is in use, turning the knobs also changes the notes that sound.

**HOME** (a new sound) or loading a preset gives the sound's own envelope back. Each track keeps an envelope for
each of its eight engines. Sessions store them and copying a track copies them; presets do not.

### The bottom row of pads

The bottom row of pads (9 to 16) plays no notes. It shows the page of the button that opened it, one page at a
time: opening another replaces it. The pages stay up while you choose other tracks; they act on the chosen
track.

| Opened by | Pads 9–16 | Colours |
| --- | --- | --- |
| **CHORD** held | the eight chord forms | blue; the chosen form bright |
| **LOOP** | REVERSE, PENDULUM, RANDOM, STUTTER, CLIMB DOWN, CLIMB UP, HALF, DOUBLE | reds; lit while on, dim while off |
| **BRICKLAYER** held | the same eight, for BRICKLAYER | violets; lit while on, dim while off |
| **PLAY** held | the chosen track's loops 1 to 8 | a soft red; half lit while in, dim while out, dark where there is no loop |
| **FADER BANK** held | pads 9–14: tracks 1 to 6 for REACT, SHADOW or ARP | the mode's colour; lit while in, dim while out |

The same gesture again (or, for LOOP, a second press) frees the pads (PADS FREE).

### Chords

With chord mode on for a track, each key plays three notes: its own note and two more of the same scale. Every
chord stays in the scale, and its quality (major, minor, diminished) is the scale's at that degree.

Press **CHORD** to turn the chosen track's chord mode on or off (CHORD lit while on). The screen says the form,
or CHORDS OFF. While chord mode is on, **CHO** shows at the top left of the screen, with the form's abbreviation
under it in grey.

Hold **CHORD** (0.6 s) to put the forms on the bottom row of pads; chord mode comes on with them. Press a pad to
choose a form (its name shows on the screen). Hold CHORD again to free the pads. Turning chord mode off, or
choosing a track whose chord mode is off, takes the forms off the pads.

| Pad | Form | Steps of the scale | Screen | |
| --- | --- | --- | --- | --- |
| 9 | **TRIAD** | 0, 2, 4 | TRI | root, third, fifth (the start) |
| 10 | **INV 1** | 2, 4, 7 | IN1 | first inversion: the third at the bottom, the root an octave up |
| 11 | **INV 2** | −3, 0, 2 | IN2 | second inversion: the fifth below the root |
| 12 | **SUS2** | 0, 1, 4 | SU2 | the third replaced by the second |
| 13 | **SUS4** | 0, 3, 4 | SU4 | the third replaced by the fourth |
| 14 | **OPEN** | 0, 4, 9 | OPN | root, fifth, and the third an octave up |
| 15 | **FOURTHS** | 0, 3, 6 | 4TH | stacked fourths |
| 16 | **FIFTHS** | 0, 4, 8 | 5TH | stacked fifths |

One chord sounds at a time on a track: a new key lets the one before go. Chords follow the loops' moves along
the scale like single notes, latch with SHIFT (all three notes), are recorded by the loopers as three notes, and
apply to BALLSY's and BRICKLAYER's notes as to keys. Sessions keep chord mode, its form and the chords latched;
copying a track copies its chord mode.

### Latched keys

A latched note sounds on after you let go of its key.

- **SHIFT held, then a key**: the note latches at once.
- **Keys held, then SHIFT**: the keys held latch.
- **A latched key pressed alone**: it is struck again and stays latched. A struck sound that has faded, or a
  note that was cut, sounds again.
- **SHIFT + a latched key**: that latch is let go; its note ends as a released key's would.
- **SHIFT tapped twice** (within 0.35 s): every latched note on every track is let go (LATCH OFF).

Latches belong to the track they were made on. On another track the same keys play as usual, so a drone can be
left on one track while you play another. A latched note keeps the velocity it was struck at, also when it is
moved along the scale. Latched notes are never taken by a new note and never cut by the load protection (see
Voices and load). Sessions keep the latches.

### The loopers

Every track has its own looper of up to **eight loops**, and they are all **free**: no tempo, no grid, no
quantising, not even between a track's own loops. Each overdub is a loop of its own, as long as you made it,
going round in its own length over the others, so they drift in and out of phase. Each plays back what was
played, to 0.73 ms, with each note's velocity (in 16 steps).

- Press **REC** to **arm** a new loop: REC blinks and the band under the engine flashes **ARMED** and the loop's
  number in red. Press REC again before playing to disarm it.
- The **first note** you play starts the recording: REC lights and the band shows **REC** and the number, white
  on red.
- Press **REC** again to **end** it: the loop is exactly as long as from that first note to this press. It plays
  at once, round and round (LOOP n). If the track's loops were stopped, they all start again with it.
- **Press REC again while it plays to overdub**: the same again, a new loop over the others.
- The ninth is refused: the screen says **8 LOOPS**.
- **REC held** (1 s) empties the track's looper, all its loops (LOOP n / EMPTY). Emptying returns the loops to
  forwards, normal speed and VARY off, and takes away the track's own move along the scale.

A loop can last up to 12.7 minutes; it closes there by itself. A track's loops hold about 500 notes between them.
A note still held when REC closes the loop is **tied over its end**: it sounds on unbroken as the loop goes
round, so a drone stays one long note.

The keys go on playing the track over its loops, and the loops' notes and the keys' never cut each other's notes
off. The looper records only what is played on the keys (and MIDI in): BALLSY's, BRICKLAYER's and REACT's notes
are never recorded.

**PLAY** stops the chosen track's loops, or plays them again, each from its start. PLAY is lit while they play.

**Bringing loops in and out.** Hold **PLAY** (0.6 s; it does not start or stop anything): the chosen track's
loops appear on the bottom row of pads, loop 1 (the first one recorded) on pad 9, each overdub on the next.
Press a pad to take its loop out (its notes stop, the others go on) or to bring it back in: it comes in **at
once, from its start**. While PLAY is held, the screen shows the loops (in the track's colour: in; a grey frame:
out), and the keys from the lowest one up do the same as the pads: the lowest white key loop 1, the next white
key loop 2, and so on. The pads stay up after you let go of PLAY; hold PLAY again to free them. With the loops
stopped, this chooses which will play: take out all but one, press PLAY, and only that one starts; then bring
the others in, one by one. Every loop is in when recorded. Sessions keep which loops are out.

**Moving the loops along the scale.** Hold **SHIFT** and press **OCTAVE+** or **OCTAVE−**: every loop of every
track moves one step of the chosen scale up or down, so each loop keeps its shape within the scale. Latched
notes and the sequencers move with them. The notes sounding move at once, at the velocity they sounded at. The
screen shows how far they are moved (TRANSP +2, TRANSP −1, TRANSP 0), up to two octaves of the scale either way.
Choosing another scale or root while they are moved walks the new one. **The keys move with them**: on a track,
they play the scale moved as far as that track's loops are, so what you add fits what is there, and **an overdub
recorded while the loops are moved plays back as you played it**. MIDI notes are not moved. Sessions keep the
move.

**One track along the scale.** Hold **SHIFT**, hold a track's pad, and press **OCTAVE+** or **OCTAVE−**: only
that track's loops, latched notes and sequencer move, a step of the scale at a time (TRANSP T3 +2). It adds to
the move of all the loops. Holding the track's pad with SHIFT down never copies it. Emptying a track's loops
takes its own move away; copying a track takes it along; sessions keep it.

### Loop options

Press **LOOP** to put the chosen track's loop options on the bottom row of pads (LOOP lit; LOOP PADS); press it
again to free the pads. Each pad is lit while its option is on and dim while off. The options apply to all the
track's loops together, each within its own length.

| Pad | Option | |
| --- | --- | --- |
| 9 | **REVERSE** | the loops play backwards: each note starts where it ended and ends where it started |
| 10 | **PENDULUM** | forwards to the end of each loop, backwards to its start, and so on |
| 11 | **RANDOM** | each time the next note is due (and at the loop's end), the loop jumps to a note of it drawn at random and goes on from there. Each note keeps its length and the time to the note that followed it; notes struck together stay together; the notes sounding are let go at each jump |
| 12 | **STUTTER** | each loop wanders about its eighths: at the end of each eighth the next is drawn: the same again, the one before, the next (twice as likely) or the one after it. What sounds is let go at each jump. Not with RANDOM |
| 13 | **CLIMB DOWN** | each pass a step of the scale lower, round an octave of the scale and back to where it began |
| 14 | **CLIMB UP** | each pass a step of the scale higher, round an octave of the scale |
| 15 | **HALF** | the loops take twice as long (the pitch does not change) |
| 16 | **DOUBLE** | the loops take half as long |

REVERSE, PENDULUM and RANDOM exclude each other (none lit: forwards), as do HALF and DOUBLE (none lit: normal
speed); pressing the lit pad again goes back. CLIMB DOWN and CLIMB UP exclude each other. STUTTER and the
climbs are each on or off. All of them combine with the direction and the speed. The climb moves on each time
the track's first playing loop comes round; its sounding notes are let go then, and a tied note is struck again.

Changing the direction while a loop plays continues from where it is. An overdub recorded at half or double
speed, or while the loops play backwards, is stored so that it plays back as it was played. Sessions keep the
options; copying a track copies them. The screen names each change (REVERSE, FORWARD, HALF SPEED, NORMAL SPEED,
STUTTER, STUTTER OFF, CLIMB UP, CLIMB OFF).

### VARY

**SHIFT + REC** turns the chosen track's loops' **VARY** on or off (VARY, VARY OFF; **VAR** at the top right of
the screen while on). Each note the loops play gets a random level (from −10 dB to full) and a random length
(50 ms to 1.5 s) instead of the recorded one. Now and then, at odds of its own, a note is changed or joined:

- dropped this time round (5 %);
- late by 5–40 ms (25 %);
- an octave up or down (8 %);
- replaced or joined by a note a step or two of the scale away, or a fifth (8 %);
- preceded by a grace note a step away (4 %);
- struck 2–4 times quickly (4 %);
- joined by a third or fifth above (10 %);
- echoed 1–4 times 120–400 ms apart, each quieter, sometimes climbing or falling a step each time (15 %);
- the notes of a chord strummed 20–60 ms apart (12 %).

Every note stays in the scale. The loops' **pace** is no longer steady either: it rises and falls slowly, and
every few seconds on average it halts for a moment and resumes, bursts to 2–3 times for a moment, grinds to a
stop and snaps back, or climbs to 2–2.5 times and drops suddenly (not while REC records). VARY applies to all the
track's loops. Sessions keep it; emptying the loops turns it off.

### PLAY, the sequencers and the looper

A track has one sequencer at a time: while its BALLSY is on, BRICKLAYER says NOT ALLOWED and changes nothing, and
the other way round. The looper works beside either.

A sequencer turned on waits for **PLAY**. On the sequencer's page, PLAY starts and stops it (PLAY or STOP on the
screen), and PLAY blinks while it plays. While the chosen track's sequencer plays, PLAY blinks on every page and
stops it from there. When no sequencer of the chosen track plays and its page is down, PLAY is the looper's. So
you can put a sequencer's page down, record loops over it from the keys, and start and stop the loops with PLAY.
**SHIFT + PLAY** starts or stops every loop and sequencer of every track together.

The sequencers' notes play as a key's would: in the scale, with the loops' moves along the scale and chord mode.
The looper does not record them. REACT, SHADOW and ARP hear them when they play on the guide track.

### BALLSY

**BALLSY** turns on a physics sequencer of bouncing balls for the chosen track and shows its page; BALLSY is lit
while it is on and blinks while it plays. Press BALLSY again to turn it off. The balls wait in the middle until
**PLAY** (on their page) starts them; PLAY again holds them still where they are (their notes let go), and PLAY
once more moves them on from there. Each track has its own, and they run at the same time.

**Hold BALLSY** (0.6 s) while it is on to put its page down or up. With the page down the balls keep playing,
and the knobs, the screen and the other pages work as usual. A track pad or another page also puts it down.

One to eight balls move in a box on the screen. Each time a ball hits a wall, it plays a note on the track. Each
note's length (50 ms to 1.5 s) and level (from −10 dB to full) are drawn at random; the level applies to FM,
ANIMA, DRUID, BAYAREA and HIVE (CALIGULA and RUSSOLO play at their own level). Each bounce changes the ball's
angle at random by up to 12°, kept between 15° and 75° from the walls, so the paths change and every wall is
reached. Each wall holds the notes of the scale, one segment a step, and the segment hit lights up. The pitch
rises clockwise all the way round, one octave a wall:

| Wall | Octave (at the start) | Direction of rising pitch |
| --- | --- | --- |
| bottom | −1 (the octave below the keyboard's) | right to left |
| left | 0 (the keyboard's) | upwards |
| top | +1 | left to right |
| right | +2 | downwards |

While the page is up, K3–K6 control the balls (K1 and K2 stay level and pan):

| Knob | |
| --- | --- |
| **K3** | the number of balls, 1–8; new ones start from the middle in a random direction |
| **K4** | speed, 0–100 (from 0.08 to 2.4 box widths a second) |
| **K5** | the octaves of the four walls, moved together by one octave a click |
| **K6** | instability, 0–100 (0 at the start): each ball gets a wind of its own, two values that wander at random. One scales its speed, up to 8 times faster or slower at 100, so a ball can nearly stop and then rush on; the other turns it, up to half a turn a second. Occasional gusts change its direction by up to 60°. At 0 the pace is steady and the paths straight |

The screen shows the box, the walls' segments in the track's colour and the balls; the line above shows the
number of balls, or the value just turned. Sessions store each track's balls and whether they are on and playing;
copying a track copies them.

### BRICKLAYER

**BRICKLAYER** turns on a step sequencer built a note at a time, for the chosen track, and shows its page;
BRICKLAYER is lit while it is on and blinks while it plays. A short press while it is on turns it off (its notes
are kept). It waits for **PLAY** (on its page), which starts it from its first step and, pressed again, stops it
(its notes let go). Notes can be entered whether it plays or not. Each track has its own.

**Hold BRICKLAYER** (0.6 s) to put its options on the bottom row of pads (BRICKS PADS); if it was off, it comes
on with them, its page up. Hold it again to free the pads.

On its page, **REC** is the entry, not the looper:

| Gesture | |
| --- | --- |
| **REC** | entry on or off (REC stays lit while it is on) |
| a key, entry on | its note is added at the end of the sequence (and sounds) |
| **SHIFT** + a key, entry on | that note is taken out, wherever it is |
| **SHIFT + REC** | the last note added is taken out (LAST OUT) |
| **REC held** (1 s) | the sequence is emptied (BRICKS / EMPTY) |

The notes play in the order they were added, spread over the sequence's length as evenly as possible (a
Euclidean rhythm, the first note on the first step: 3 notes in 8 steps fall on steps 1, 4 and 7). Notes are kept
as steps of the scale, so they follow the moves along the scale, the scale and the root, as latched notes do.
Chord mode applies to them as to keys. Up to 32 notes.

| Knob | |
| --- | --- |
| **K3** | **LENGTH**: steps, from the number of notes up to 32 |
| **K4** | **RATE**: 0.5–16 steps a second |
| **K5** | **HARMONY**: how often a note becomes a diad (a third or a fifth above) or a triad |
| **K6** | **VARIATION**: each note's level (down to −12 dB) and length (from a quarter to twice the usual) spread at random; the pace wanders (from half to twice the rate), and now and then stops for a moment or rushes; as likely as the knob says, a note stutters (struck 2–4 times within its step), the last 2–4 steps are played again (beat repeat), a note echoes 1–3 times, quieter, or an empty step plays a quiet ghost note from the sequence. At 0: a steady level and length, a steady pace |

BRICKLAYER's options on pads 9–16 are laid out as the loops' and work the same way on its steps:

| Pad | Option | |
| --- | --- | --- |
| 9 | **REVERSE** | the steps backwards |
| 10 | **PENDULUM** | forwards and backwards in turn |
| 11 | **RANDOM** | a step drawn at random each time |
| 12 | **STUTTER** | it wanders: the same step again, the one before, the next or the one after it |
| 13 | **CLIMB DOWN** | a step of the scale lower after each run of its steps, round an octave of the scale |
| 14 | **CLIMB UP** | a step higher after each run of its steps |
| 15 | **HALF** | half speed |
| 16 | **DOUBLE** | double speed |

With BRICKLAYER's page up, LOOP's pads set BRICKLAYER's options too, all eight: its direction, STUTTER, the
climbs and its speed, as BRICKLAYER's own pads do.

The screen shows the steps as cells, eight to a row (up to four rows): a note's cell filled in the track's colour,
an empty one as an outline, the step being played in white. The line above shows the notes and the steps
(3 IN 8), REC and the number of notes while entering, or the value just turned. Each note lasts 70 % of its step
(VARIATION spreads it). Sessions store each track's sequence, settings and options; copying a track copies them.

### REACT, SHADOW and ARP

In these three modes the other tracks play along with the one you play. The chosen track is the **guide**. The
tracks you take in listen to everything the guide sounds (its keys, latches, chords, MIDI in, loops, BALLSY and
BRICKLAYER) and play their own sounds in answer, in the scale. Choosing another track makes it the guide.

- **SHIFT + FADER BANK** turns the mode on or off at once (REACT, REACT OFF; SHADOW, SHADOW OFF; ARP, ARP OFF).
  FADER BANK blinks slowly while it is on, and **PLAY** flashes with each note the guide plays (keys, chords,
  loops, BALLSY, BRICKLAYER, MIDI in), on every page but BALLSY's and BRICKLAYER's.
- **FADER BANK held** (0.6 s) puts its page up, with its pads on the bottom row. Held again while the page is up,
  both go down. A track pad also puts the page down; the mode goes on running, and its pads stay up.
- On the page, **<** and **>** choose the mode: **REACT**, **SHADOW** or **ARP**. Changing the mode lets go of the
  notes the tracks are playing.
- On the pads, **pads 9–14 are tracks 1–6**. Press a pad to take that track in or out (2 IN, 2 OUT): lit in the
  mode's colour while in, dim while out. At first no track is in. The guide's pad is dark; pressed, it lights red
  for a moment and the screen says GUIDE TRACK (a track cannot follow itself).
- **K1** and **K2** stay the guide's level and pan. **K3–K6** (ARP: **K3–K8**) set the mode's values, 0–100.

The page shows the mode's name (in its colour while on, grey while off), the name of the knob turned last, its
value large, and a bar for each knob. REACT is sea green, SHADOW pale blue, ARP lime.

The tracks taken in play their notes as a sequencer's: the looper does not record them, and they never play on
the guide. When the guide stops, they stop. The modes, the tracks taken in and their values are not kept in
sessions.

### REACT

REACT improvises from a memory of everything the guide plays: its last 128 events (notes or chords), with their
levels, lengths and the gaps before them, kept as steps of the scale. After François Pachet's Continuator, IRCAM's
OMax and Somax, and George Lewis's Voyager.

Each track taken in improvises from that memory. From the event it played last, it goes on as the guide once went
on, or jumps to another place in the memory where the guide played the same intervals (and rhythm), and goes on
from there. So its lines are the guide's own, recombined: new phrases in the guide's style, never a pattern of
its own. Its rhythm is the guide's, slowed or quickened by its own factor. Each choice is weighed by how it fits
the harmony now (what the guide holds, and the pitch classes it played lately), and its notes are moved a third
if that fits better. Every note is kept in the scale. When the guide's harmony moves, a long note that no longer
fits lets go.

At each of the guide's phrases, and every 6 to 9 seconds of unbroken playing, the tracks regroup. Each takes a
way to play, suited to its sound (a texture such as CALIGULA, RUSSOLO, drones and washes plays only slow, low,
single notes), and a register of its own:

| Way | |
| --- | --- |
| **FOLLOWS** | with the guide, as fast or half as fast, its harmony followed closely |
| **CONTRASTS** | against it: slow and long under a busy guide, moving under one holding long chords; the other side of the keyboard, softer |
| **ECHOES** | the guide's line a few events behind, a third, a sixth or an octave away; at a rest, the rest of it |
| **ANSWERS** | in the guide's rest: its phrase continued as the guide might have, until the guide plays again |
| **RESTS** | sits out this time |

| Knob | | |
| --- | --- | --- |
| **K3** | **DENSITY** | how much they play: how often a track sounds, how long it waits (from twice to half the guide's gaps), how many sit out |
| **K4** | **COMPLEXITY** | low: single notes in plain rhythms; high: harmonised notes (a third or a sixth under), notes moved further along the scale, steps split in two or three, notes pushed off the beat, threes against twos |
| **K5** | **CONTINUITY** | low: they jump about the guide's memory, recombining it; high: they keep to its phrases as played |
| **K6** | **SPREAD** | how far from the guide's register: low, around its hands; high, from deep bass to high air |

They start at DENSITY 50, COMPLEXITY 40, CONTINUITY 60, SPREAD 40. After a long silence of the guide, the tracks
let go and wait.

### SHADOW

The tracks taken in shadow what the guide sounds, each from its own distance behind, at its own pace. Now and
then a shadow goes backwards or to and fro, rests and comes in again, its rhythm loosened, moved along the scale.

| Knob | | |
| --- | --- | --- |
| **K3** | **DELAY** | how far behind: from right behind (a note or two) to 15 s, each shadow at its own distance |
| **K4** | **DRIFT** | how far each one's pace strays from the guide's: from a little loose to half, two thirds, three quarters, one and a third, one and a half or twice, in and out of step |
| **K5** | **VARIATION** | how often each changes its way: low, forwards and steady for long; high, backwards, to and fro, resting and coming in again, notes late, left out or struck twice |
| **K6** | **TRANSPOSE** | how far it is moved: low, as played (or an octave); high, a third, a fourth, a fifth or a sixth away |

They start at DELAY 30, DRIFT 30, VARIATION 40, TRANSPOSE 30.

### ARP

The tracks taken in break the guide's chord into arpeggios, each at its own pace, its own way and in its own
octaves, and follow the chord as it changes. The chord is what the guide holds (a finger lifted, a note less);
let go, the last chord goes on until the guide has been silent a while. A single note is arpeggiated over its
octaves. A new chord, once it has stood 90 ms, is taken by each track at its next step, from the start of its
pattern: never between two of its notes, the pulse unbroken. From silence, the first chord starts them at once.
Textures arpeggiate four times slower. The page shows K3–K8 in two rows of three.

| Knob | | |
| --- | --- | --- |
| **K3** | **RATE** | the pace: a step every 2 s (0) to every 62 ms (100), steady |
| **K4** | **RELATION** | how the tracks' paces relate: all one at 0; higher, twice, half, three in two, two in three, four in three, three times as fast, each track its own, interlocking |
| **K5** | **MOTION** | which way: all up at 0; higher, each its own (down, up and down, down and up, the order played, from the outside in, from the middle out, at random), the second against the first; high, a track turns another way now and then |
| **K6** | **MUTATE** | how much a pattern changes as it repeats: fixed at 0; higher, steps swapped, a rest, a step struck again, the pattern turned |
| **K7** | **RANGE** | how many octaves each spans, 1 to 4, and how far apart the tracks sit |
| **K8** | **CHANCE** | tight at 0; higher, steps left out and steps struck twice |

They start at RATE 50, RELATION 40, MOTION 40, MUTATE 20, RANGE 30, CHANCE 10.

### Copying a track

Hold a track's pad until the screen flashes **COPIED / TRACK n**: the track is on the clipboard, and its pad
blinks. Choose another track and hold its pad until **PASTED / TRACK n**: it now has the copied track's tone,
sequencers and loops. The clipboard is then empty: the next hold copies again. Holding the copied track's own pad
again copies it again (as it is now), pasting nothing.

What is copied: the **tone** (the engine, the sounds of all eight engines with their macros and envelopes, the
filter, the granulator, the crush, the distortion, the delay and reverb sends, the octave, the path, the
modulation and the chord mode), **BALLSY** and **BRICKLAYER** with their settings, and **all the loops** with
their options and VARY, every overdub, those taken out still out. The copy is taken when you hold the pad: what
the track does afterwards is not pasted. The track pasted onto keeps its **level** and **pan**; whatever it had
before is replaced, and its latched notes are let go. The clipboard is kept in the flash beside the sessions
(writing it may hush the audio for a moment); it is emptied when the keyboard is switched off.

### Gesture loops

Besides its note loops, each track has **eight gesture loops**: recordings of your hands on the knobs and faders,
played back round and round. They have nothing to do with the note loops: they run whatever else plays, held
notes, latched chords, loops, sequencers, each at **its own length**, so they drift against everything else. A
filter sweep made over the end of a phrase comes back, next time, over another part of it, and further along each
time. That is the point: the gestures are not tied to the notes, and they bring the unexpected, even to you.

Press the **sixth button of the top row** (GESTURES; lit while it is up): the chosen track's eight gesture loops
are on the bottom row of pads, 9 to 16, all glowing faintly sky blue even while empty, so you know the pads are
theirs. Press it again to free the pads.

- **Hold a pad** (0.8 s) to **arm** it: it blinks red and the band says **ARMED**. Whatever it held is emptied.
- **Move a knob or a fader**: the recording starts with that move (the pad turns red; the band says G1 REC).
  Everything you move after it is kept, as long as you go on.
- **Press the pad** to end it: the gesture loop is as long as from that first move to this press, and it plays at
  once, round and round (sky blue).
- **Press a recorded pad** to switch it **off** (dim) or back **on** (lit; it starts from its beginning).
- **Press an armed pad** before moving anything to let it go (NOT ARMED).

What a gesture loop records: the track's **filter** (K3), **granulator**, **crush**, **distortion**, **delay** and
**reverb sends**, **level** and **pan**, the **eight macros** (with MACRO on) of the sound it plays, and **faders 2
and 3** (SPACE and the master filter). It records the track you armed it on; switching tracks while it records, the
other track's knobs are not recorded, the faders still are. One gesture loop is armed or records at a time: arming
another ends the one recording.

What it plays moves those values as your hand would: you can move them between its events, and it takes them over
again at its next one. A fader's own position is left where it is until you move it again. All the gesture loops of
all the tracks share one store of about a thousand moves; when it is full the screen says GESTURES FULL (empty one
by arming it). Sessions keep them; snapshots and copying a track do not (yet).

### The wheels and the master

The **pitch wheel** bends the chosen track up to 2 semitones either way. Every engine bends. The **mod wheel**
adds a vibrato (5.5 Hz, up to 0.4 semitone) and pushes the first macro of the sound up, up to its full turn. Both
act on the chosen track only; choosing another track lets go of them on the one left. The wheels calibrate
themselves: the pitch wheel's rest is taken at power-on.

**Fader 4** (the rightmost) is the master level. It sets what reaches the headphones and the outputs, not the
USB audio (see USB audio).

### The faders

Faders 1 to 3 are live controls over the whole instrument. They act on top of what is stored and are never saved:
sessions, snapshots and presets keep what they kept before, and the faders act on them wherever they stand. While a
fader moves, and a moment after, the screen shows its dial.

**Fader 1, MORPH.** A morph by hand between snapshots. Set the morph time (K3 on the SNAPSHOTS page) to
**MANUAL**, its lowest setting, below OFF. Then a pad on the SNAPSHOTS page no longer recalls its snapshot at once:
tapping it makes it the **target** (its pad and box blink; the screen says FADER 1 UP or FADER 1 DOWN), and nothing
changes yet. Move fader 1 from wherever it stands to the end it is furthest from (the top; or, if it stands near the
top, the bottom): levels, pans, sends, filters, macros and the other values that glide follow the fader from the
snapshot you are on to the target; what has no in-between (scale, root, octaves, loops, latched notes, sequencers,
engines) switches at the half-way, in either direction; a track whose sound differs goes down to silence there and
comes back. You can go back and forth on the way. At the end the machine is the target exactly, and it becomes the
snapshot you are on; the fader then does nothing until you tap another pad. Tapping another pad on the way starts
again from whichever of the two the machine is nearer. With any other morph time, a pad recalls as before (at once,
or over that time) and fader 1 does nothing (its dial says OFF, K3 TO MANUAL).

**Fader 2, SPACE.** At the bottom, everything as set. Pushing it up raises every track's reverb and delay sends
together, lengthens the reverb's decay and adds repeats to the delay, towards a wash at the top. Pulling it back
brings the mix back to dry. The values you set on the DELAY and REVERB pages and each track's sends stay as they
are.

**Fader 3, the master filter.** One filter over the whole mix, after the delay and the reverb, before the master
level (and in the USB audio's STEREO too). The middle is off, with a small zone either side so the fader finds it
easily. Below, a low-pass comes down from 20 kHz to about 60 Hz; above, a high-pass goes up from 20 Hz to about
8 kHz; a little resonant towards the ends. It glides as the track filter does. Its dial reads LP 1.2K, HP 400 or
OFF.

The faders act from where they are at power-on: fader 2 up means SPACE is already on, fader 3 away from the middle
means the master filter is already on.

### The screen

The screen is made for low vision: few words, very large type, high contrast.

From top to bottom, the track view shows:

- the **track number**, very large, in the track's colour; at the top left **CHO** and the chord form while chord
  mode is on; at the top right the **battery** (three bars green, two green, one amber, one red when low; a white
  bolt on a computer's USB power, when the reading is the supply's) and the track's **octave** to its left (+1,
  −2; nothing at 0), with **VAR** under them while the loops VARY;
- the track's **engine**, in white;
- the **family of its sound**, in the track's colour. When something changes (NEW, OCT +1, LOOP 2), a
  black-on-white message replaces it for a moment. While the looper is armed this band flashes **ARMED** and the
  loop's number in red; while it records it shows **REC** and the number, white on red. While MACRO is on it
  says **EDIT** in red;
- the track's **level meter**: a bar filling from the left in the track's colour as it sounds (−48 to +6 dB), and
  a white mark at the level K1 sets (red at the left when the track is off);
- at the very bottom, the track's **pan**: a block in its colour on a line from left to right, with a tick at the
  middle.

K3 to K8 (filter, granulator, crush, distortion, delay send, reverb send) each show, while they turn and for a
second after, a full-screen **dial**: the knob's name and the track, an arc from 7 to 5 o'clock lit in the
track's colour, a white pointer, and in large type what it does (LP 1.2 KHZ, DARK 40, BITS 6.5, SEND 40). The
macros show the same dial. K1 and K2 say their value in the message band.

Saving, loading, emptying a looper, copying and pasting flash the whole screen for 1.5 s. The pages (SCALE,
ENVELOPE, DELAY, REVERB, MOD, SNAPSHOTS, SESSIONS, PRESETS, GLOBAL SETUP, BALLSY, BRICKLAYER, the loops while PLAY is held,
REACT) each take the whole screen and are described in their sections. On every page, the value just turned is
drawn black on white for a moment.

If the chip's second core did not start, the screen says **1 CORE** for a few seconds: the instrument plays, with
half the room for voices.

### Scales

One scale and root for the whole keyboard, saved with the session. At start, and in a new (empty) session, both are
drawn at random. Press **SCALE** to open the SCALE page: **K1** chooses the root (C to B, round the octave), **K2** the scale. The page shows SCALE, the
root very large, and the scale's name under it; what you just turned is drawn black on white for a moment. SCALE
again, or a track pad, goes back.

Hold **SCALE** (0.8 s) to draw a new scale and root at random, as at start, from any page. The SCALE page shows them
for 1.5 s, then the page that was up comes back. The loops and latched notes follow the new scale, as when you turn
K1 or K2.

The root sits on the lowest key and the scale climbs from there across all 37 keys, black keys included (see The
keys for where each scale starts).

| Scale | Steps |
| --- | --- |
| MAJOR | 0 2 4 5 7 9 11 |
| MINOR NATURAL | 0 2 3 5 7 8 10 |
| MINOR HARMONIC | 0 2 3 5 7 8 11 |
| MINOR MELODIC | 0 2 3 5 7 9 11 |
| DORIAN | 0 2 3 5 7 9 10 |
| MINOR PENTA | 0 3 5 7 10 |
| WHOLE TONE | 0 2 4 6 8 10 |
| ENIGMATIC | 0 1 4 6 8 10 11 |
| JAPANESE IN | 0 1 5 7 8: the *in* (miyako-bushi) scale of the koto and shakuhachi |
| PHRYGIAN | 0 1 3 5 7 8 10 |
| LYDIAN | 0 2 4 6 7 9 11 |
| MIXOLYDIAN | 0 2 4 5 7 9 10 |
| LOCRIAN | 0 1 3 5 6 8 10 |
| MAJOR PENTA | 0 2 4 7 9 |
| BLUES | 0 3 5 6 7 10 |
| HUNGARIAN MINOR | 0 2 3 6 7 8 11 |
| DOUBLE HARMONIC | 0 1 4 5 7 8 11 (also called Byzantine) |
| PHRYGIAN DOMINANT | 0 1 4 5 7 8 10: the fifth mode of the harmonic minor |
| JAPANESE HIRAJOSHI | 0 2 3 7 8 |
| JAPANESE IWATO | 0 1 5 6 10 |
| PELOG | 0 1 3 7 8: Balinese *selisir*, in equal temperament |
| OCTATONIC | 0 1 3 4 6 7 9 10: half step, whole step, alternating |
| PROMETHEUS | 0 2 4 6 9 10: Scriabin's |
| CHROMATIC | all twelve: the keyboard as printed |

The screen splits the longer names over two lines (MIXO- LYDIAN, HUNGAR- IAN MINOR, OCTA- TONIC, PROME- THEUS,
CHROMA- TIC) rather than squeezing the letters together. MIDI notes are not quantised to the scale.

### Presets

Each engine has **64 presets**, P1 to P64, for the sounds you want to keep and bring back on any track. Hold
**SHIFT** and press **SCALE**: the PRESETS page of the chosen track's engine opens (the engine's name over the
slots), four slots a page. **<** and **>** step through them; a slot holding a sound shows its number in red and
the sound's family numbered among that engine's presets of the family, in the order of the slots (BRASS 1,
BRASS 2, STRINGS 1); an empty one EMPTY in grey. SHIFT + SCALE again, or a track pad, goes back.

- **SCALE held** (0.8 s), or **SAVE held**, saves the track's sound into the slot chosen. Over a sound already
  there it asks **OVERWRITE?** first, on **NO** until **>** turns it to YES (**<** back to NO); SCALE (or SAVE)
  answers.
- **SCALE** pressed, or **SAVE** pressed, loads the slot chosen into the track (which crossfades to it, as to a
  new sound) and goes back to playing. An empty slot says EMPTY.

A preset keeps what draws the sound again exactly (its seed, its family, its path and the generation of the
generators that drew it), its level and its eight macros. A sound chosen by family (SHIFT + < / >, SHIFT + HOME)
comes back the same from a preset or a session. It does not keep the track's envelope: a preset loads with the sound's
own. A preset loaded draws the same modulation as HOME would with that sound. Saving erases flash, which
silences the audio for a moment.

### Snapshots

A snapshot is the state of the machine at one moment. Each session holds 16 of them, one on each pad.

Press **SAVE** (lit while the page is up): the **SNAPSHOTS** page opens, 16 boxes in two rows of eight, as the
pads. A filled box holds a snapshot; the one last stored or recalled is white. While the page is up, the 16 pads
are the snapshots, the top row 1 to 8 and the bottom row 9 to 16: they choose no track and no path. A pad holding
a snapshot glows; the one last stored or recalled is lit; an empty pad is dark.

| To | Do this |
| --- | --- |
| Recall a snapshot | press and release its pad |
| Store the current state | hold its pad 0.8 s, or frame it with **<** and **>** and hold **SAVE** |
| Empty a snapshot | hold **SHIFT** and hold its pad 0.8 s |
| Set the morph time (0 to 30 s), or MANUAL (fader 1) | **K3** |
| Leave the page | **SAVE** again |

The keys go on playing while the page is up, and K1 and K2 stay the track's level and pan.

**What a snapshot holds.** The whole state of the machine. For each track: the engine it plays; the sound on every
one of its eight engines, each with its eight macros and its envelope; level, pan, reverb and delay sends, filter,
granulator, crush, distortion, octave, the track's transposition, its path, chord mode and modulation; its latched
notes and chords, with their velocities; BALLSY's and BRICKLAYER's settings (BRICKLAYER's STUTTER and climbs
too), BRICKLAYER's notes, and whether each plays; and its loops: which loops there are, where each stood against
the others, which are taken out, whether they play, and their direction, speed, VARY, STUTTER and climbs. For the
machine: the reverb, the delay, the scale, the root, the loops' transposition and REACT, SHADOW and ARP (on or off,
the mode, the tracks taken in, the values on their pages). Not stored: the track chosen, MACRO and which page is
up.

For example: record a loop and store snapshot 1; record an overdub and store snapshot 2. Snapshot 1 brings back the
loop alone and snapshot 2 brings back both. Loops that are in both snapshots keep playing when you switch; a loop
that comes back is placed where it stood against the others when it was stored.

A loop that a snapshot holds stays in memory even after you empty the looper (hold REC). It shares the looper's
room with what you record: when the looper has no room left, REC says LOOPER FULL. Snapshots that share sounds,
latched notes or loops store them once. If there is no room for a new snapshot, the screen says FULL; empty
snapshots you no longer need (SHIFT + hold the pad).

**Morph.** With a morph time of 0 (OFF), a recalled snapshot is applied at once. With a longer time, the machine
moves to it over that time; the snapshot's box and pad blink until it is reached:

- Levels, pans, sends, filter, granulator, crush, distortion, the macros and envelope of a sound that stays, the
  modulation amounts, LFO rates and fades, the reverb's DECAY, DAMP and MIX, the delay's REPEATS, DAMP and MIX, and
  the sequencers' speed, rate, harmony and instability change gradually.
- Everything that affects pitch changes at once, halfway through: scale, root, octaves and transpositions. So do
  the delay's TIME and the reverb's SIZE, the loops, the latched notes, chord mode, the sequencers and their notes,
  the loops' and BRICKLAYER's STUTTER and climbs, the modulation routings and LFO shapes, and REACT, SHADOW and ARP.
- A track whose sound changes fades out from a quarter of the way, switches sound halfway and fades back in by three
  quarters.

Recalling another snapshot during a morph starts a new morph from the current state. The morph runs in the main
loop and adds no load to the audio.

Snapshots are saved in the flash with the session (hold **SAVE**, off the SNAPSHOTS page, to save). A new session
has none; sessions saved before BUILD 33 load with none.

### Sessions

The keyboard always starts with no session loaded. There are 64 slots, numbered 1 to 64.

A session is the whole machine: every track's engine and its eight sounds with their macros and envelopes, its
path, octave, level, pan, filter, granulator (and the character it was drawn with), crush, distortion, delay and
reverb sends, modulation, chord mode, latched notes, BALLSY, BRICKLAYER, and its loops (playing or stopped, each
overdub in or out, with their options and VARY); the scale and root, the moves along the scale, the delay, the
reverb, REACT, SHADOW and ARP, the 16 snapshots with the loops they hold, the morph time, and the track chosen.
A session stores in full the sound of the engine each track is on, and the others
as the seed that made them and their level, so they come back the same.

**Choosing and loading.** Press **SHIFT + SAVE**: the **SESSIONS** page opens, on the session loaded if there is one.
**<** and **>** step through the slots. The number is very large: **red** when the slot holds a session, **grey**
with EMPTY under it when it does not; the session loaded says LOADED. Press **SAVE**:

- on a slot holding a session, it **loads** (LOADING, then LOADED and the number);
- on an empty slot, it starts a **new session** there (NEW and the number): a clean slate, as when the keyboard
  is switched on. The loops and latches go, every track is on its starting engine with new sounds and its
  settings at their defaults, a scale and root drawn at random, and the delay, the reverb and the moves along the scale too. The
  session you were in stays saved as it was, and the next save goes to the new slot.

Either way the page closes. A track pad, or SHIFT + SAVE again, closes it without either.

**Saving.** Hold **SAVE** (0.8 s):

- with a session loaded or chosen, it saves over it at once (SAVING, then SAVED and the number);
- with none loaded or chosen (just after switching on), the first empty slot is offered (SAVE NEW): **<** and
  **>** choose **NO** or **YES**, **SAVE** answers (NOT SAVED on NO);
- on the SESSIONS page, SAVE held saves into the slot shown, whatever is loaded: the session loaded is saved over
  at once; an empty slot asks SAVE NEW; another slot holding a session asks **OVERWRITE?** in red, on **NO** until
  **>** turns it to YES; SAVE answers.

The sessions live in the keyboard's user flash area: 40 sectors of 4 KB, shared with the presets, GLOBAL SETUP
and the clipboard. A typical session takes one sector; a session with long loops takes more. When there is no
room the screen says FULL. The first save takes over the area where the stock firmware kept its own settings;
if you go back to the stock firmware, it rebuilds them.

A session also keeps the seeds its sounds were made from, with the path and the *generation* of the generators
that drew them. Every generation is kept exactly as it was, so sessions and presets saved under an older build
load with the same sounds after an update that brings new generators. Erasing flash silences the audio for a
moment: a save uses blank flash when there is some, and the keyboard blanks free flash by itself while nothing
sounds.

**Backing up.** The [backup page](https://nzimas.github.io/tonitor-smk37/backup.html) saves them to a file on your computer and puts them back, over USB MIDI: it lists, sends and receives the
sessions, the presets and GLOBAL SETUP, each record checked whole before it replaces the one with the same
number. As each one arrives, the screen shows RESTORED and its number.

### Global setup and MIDI

**SHIFT + SAVE held** (0.8 s) opens **GLOBAL SETUP**, the keyboard's own settings, kept apart from the sessions. It has three
sections, **MIDI**, **VELOCITY** and **AUDIO**. **<** and **>** move through the entries and **SAVE** goes into one. On a
setting, SAVE puts the focus on its value (a white field), < and > change it, and SAVE sets it; **SHIFT + SAVE**
leaves it as it was, or goes back a level, and at the top level closes GLOBAL SETUP. Each level below the top
starts with **BACK**. A track pad also closes it. What changed is written to the flash when you leave.

| Section | Part | Settings | At the start |
| --- | --- | --- | --- |
| **MIDI** | **IN CHAN** | TRACK 1 to 6: the channel each track listens to (OFF, 1–16); AUTO: the channel that plays the chosen track | 1 to 6; AUTO 16 |
| | **OUT CHAN** | TRACK 1 to 6: the channel each track sends on (OFF, 1–16) | 1 to 6 |
| | **RECEIVE** | NOTES, CC (ON / OFF) | ON, ON |
| | **SEND** | NOTES, CC (ON / OFF) | OFF, OFF |
| | **PORTS** | USB IN; TRS OUT (the 3.5 mm jack, as MIDI OUT) | ON, ON |
| **VELOCITY** | **KEYS** | CURVE: EVEN, SOFT (soft playing louder, for a light touch), HARD (for a firm touch), FIXED | EVEN |
| | | LOWEST, HIGHEST: the range the curve maps into (1–127) | 1, 127 |
| | | FIXED: the velocity of every note when the curve is FIXED (1–127) | 100 |
| **AUDIO** | **USB** | OUT: STEREO (the master) or TRACKS (tracks 1 to 6, each on its own channel). See USB audio | STEREO |

**MIDI in** comes over USB only (the SMK-37 has no MIDI IN jack). A note plays every track whose IN channel it is
on, and the chosen track on the AUTO channel, at its velocity (the VELOCITY settings apply to the keys only). The
CCs a track answers to, on the same channels, and sends when SEND CC is on:

| CC | | CC | |
| --- | --- | --- | --- |
| 7 | level | 74 | filter (64 off; below, low-pass; above, high-pass) |
| 10 | pan | 91 | reverb send |
| 16–19 | macros 1–4 of the track's sound | 92 | delay send |
| 20–23 | macros 5–8 of the track's sound | | |

**MIDI out** goes over USB and out of the **TRS jack** together: every message sent over USB also goes out of the
jack, with or without a computer, while TRS OUT is ON. With SEND NOTES on, each track sends the notes it plays
(from the keys, the loops, BALLSY, BRICKLAYER, REACT, SHADOW, ARP and MIDI in) on its OUT channel. With SEND CC on,
the values above go out when they change, whatever changed them. Both are OFF at the start: turn them on to drive
other instruments. A jack with nothing that answers is left quiet; the keyboard never waits on it. There is no MIDI
clock, transport or program change: Tonitor has no tempo or patterns.

### Modulation

Each track has a small **modulation matrix**: two LFOs and four slots that move the track's own values while it
plays. The third press of **SEQ** opens the **MOD page**, in the track's colour. **<** and **>** go through its six
entries (**LFO 1**, **LFO 2**, then **MOD A** to **MOD D**), and **K3–K6** set the entry's four rows.

| Entry | K3 | K4 | K5 | K6 |
| --- | --- | --- | --- | --- |
| **LFO 1**, **LFO 2** | **SHAPE**: SINE, TRI, SAW UP, SAW DN, SQUARE, STEPS (a new random value each cycle), DRIFT (a smooth random wander) | **RATE**: 0.01 Hz (a cycle in 100 s) to 20 Hz | **MODE**: FREE (never restarted), NOTE (from the start at each note), ONCE (one cycle from each note, then held: an envelope) | **FADE**: off, or in over up to 10 s from each note |
| **MOD A** – **MOD D** | **FROM**: OFF, LFO 1, LFO 2, VELOCITY (softer notes less), KEY (the note, around C4), RANDOM (a new value each note) | **TO**: the sound's eight macros (by their names), FILTER, GRAINS, CRUSH, DRIVE, REVERB, DELAY, LEVEL, PAN, LFO 1 RATE, LFO 2 RATE | **AMOUNT**: −100 to +100 of the destination's whole swing | (none: the row **NOW** shows what the slot does at the moment) |

The modulation moves the values the track already has (what the knobs set, or the macros) around where they are,
and the whole track follows. It moves a value once a block, so it costs next to nothing. Sessions keep each
track's modulation, and copying a track copies it.

**SHIFT + SEQ** throws the chosen track's modulation anew (MOD RANDOM): both LFOs (any shape, rate, mode and fade)
and two to four slots, from any source to any destination, by amounts that may go far. It takes risks on purpose;
press it again for another, or set what you like on the MOD page.

**HOME** draws a modulation with some sounds, by engine and family: mostly with the slow, sustained families of
FM, HIVE, BAYAREA and DRUID (a slow LFO on the tone, sometimes a fade in, sometimes a second, slower one in the
stereo field or the reverb); now and then a struck or plucked sound gets its brightness on the velocity; never
ANIMA or CALIGULA, whose sounds already move by themselves. A new sound with none clears the track's modulation;
a preset loaded draws the same modulation again.

### The filter and the crush

**K3** is the chosen track's **filter**. From the middle (off), left is a resonant **low-pass** coming down from
20 kHz to about 35 Hz; right is a resonant **high-pass** going up from 16 Hz to about 12 kHz. The further from the
middle, the more resonant. Its dial reads the mode and cutoff (LP 1.2 KHZ, HP 400 HZ, OFF). The cutoff glides
smoothly as you turn, like an analog filter's: each click of the knob moves it about a third of an octave, turned
fast two clicks' worth, and the filter slides there over about 25 ms rather than jumping.

**K5** is the chosen track's **crush**. The middle is off. **Left** takes the bit depth down from 16 to 1.5 bits
(the sound coarsens, then breaks up, quiet tails gating out); **right** takes the sample rate down from 44.1 kHz
to about 300 Hz (metallic aliasing, then a stepped buzz). Both follow curves that keep the first half of the way
playable; the rate glides, it does not jump. Its dial reads BITS 6.5 or RATE 2.6K. It comes after the granulator,
before the track's limiter.

Both are saved with the session.

### The granulator

**K4** is the chosen track's **granulator**. The middle is off (as it starts).

- **Left**, towards dark ambient: long, overlapping, slowly drifting grains (60–400 ms), pitched down now and
  then, reversed now and then, darkened, fed back on themselves, sometimes frozen (the buffer stops listening for
  a few seconds and the cloud turns over what it holds).
- **Right**, towards scattering: short, fast grains (40 ms down to 4 ms, up to about 650 a second), their timing,
  place, pitch and level jittered more and more, reversed, stuttered, full of holes, decimated at the far end.

The further from the middle, the more grains and the less of the track. Its dial reads DARK 40, SCATTER 25 or
OFF.

Each track keeps the last half second of its own sound (at 22.05 kHz, 8-bit μ-law: a little grit is part of it)
and plays grains out of it, after its filter. Each track's granulator has a **character of its own**, drawn at
random when the keyboard starts and every time a session is loaded: the intervals its grains are pitched to, how
often they reverse, stutter, drop out or freeze, the shape of its scattered grains, how far its feedback and
darkening go, how it drifts. The knob's position is saved with the session; the character is drawn anew. When the
track's notes end, its grains ring on for about two seconds.

### The distortion

**K6** is the chosen track's **distortion** (0–100; 0 is off). It comes after the track's filter, granulator,
crush and limiter, and before its level, pan and sends, so the delay and the reverb receive the distorted sound.
Its dial reads DISTORTION n and the amount.

- Two gain stages in series: the first symmetric (odd harmonics), the second asymmetric (even harmonics). The
  amount sets the drive into the first stage (×0.5 to ×32, up to +36 dB) and the second stage's gain and bias.
- A bias in proportion to the input's own level is added before the first stage, so the clipped wave stays
  asymmetric and keeps its even harmonics at any drive. At low amounts the second harmonic dominates; higher
  amounts add more harmonics.
- The highs are raised 6 dB before the stages and lowered 6 dB after, so the mids and highs distort more than the
  bass. The stages run at twice the sample rate to reduce aliasing. A low-pass after them goes from 16 kHz down to
  6.5 kHz as the amount goes up.
- The output level is matched to the dry signal's level (measured over about 0.2 s), so turning the amount up
  changes the drive and the harmonics, not the loudness.

Stored in sessions and copied with the track.

### The delay

A master delay, mono, placed before the reverb. Each track's delay send (**K7**, its dial reading DELAY n / SEND)
feeds it; its echoes go to both sides of the master, and half of their level also goes into the reverb, so the
echoes are reverberated. Its line holds 2 seconds at 22.05 kHz in 8 bits (μ-law, as the granulator's), so the
repeats are slightly dark and grainy.

The first press of **SEQ** opens the **DELAY page**: the title and its four values in rows, each with a bar.

| Knob | | | At the start |
| --- | --- | --- | --- |
| **K3** | **TIME** | 20 ms to 2 s, on a logarithmic scale (shown in seconds: 0.02 to 0.99, then 1.0 to 2.0). Turning it glides the delay over about 0.1 s, so the repeats bend in pitch rather than click | 65 (0.40) |
| **K4** | **REPEATS** | the feedback: how much of each repeat comes back: 0 (one repeat) to 0.95 | 40 |
| **K5** | **DAMP** | a low-pass in the feedback path: each repeat darker than the one before | 40 |
| **K6** | **MIX** | the level of the echoes | 50 |

The delay's values and every track's send are kept in sessions; copying a track copies its send. The delay costs
nothing while nothing goes into it and its tail has died away.

### The reverb

A plate after Jon Dattorro (1997): a predelay, input diffusion, and a figure-of-eight tank whose two allpasses
sweep slowly out of step, so the tail stays in motion and never rings. Each track sends to it after its level
(**K8**, its dial reading REVERB n / SEND); the reverb's return is added to the mix before the master. The plate
runs at 22.05 kHz, half the sample rate, with Dattorro's times and decay kept: its top is near 11 kHz.

The second press of **SEQ** opens the **REVERB page**.

| Knob | | | At the start |
| --- | --- | --- | --- |
| **K3** | **SIZE** | the room: every delay 0.5–1.25 times Dattorro's (turning it bends the tail, gently) | 70 |
| **K4** | **DECAY** | the time to fall 60 dB, 0.4 s to about 25 s, whatever the size | 65 |
| **K5** | **DAMP** | how fast the highs die in the tail | 40 |
| **K6** | **MIX** | the reverb's return level, ×0 to ×4 | 50 (×1) |

The reverb's values are kept in sessions. It costs nothing while nothing goes into it and its tail has died away.

### USB audio

Connected to a computer (or a phone or tablet that takes USB audio), the keyboard is also a **USB audio input**:
choose "SMK-37 Tonitor" as the input in the recording program. It carries the master output, stereo, 16-bit, at
**44.1 kHz** (the rate Tonitor renders at; there is no 48 kHz). It is class compliant: no driver is needed, and
MIDI goes on over the same cable. The stream is taken before **fader 4**, so the fader sets the headphones' level
and not the recording's.

**TRACKS.** In GLOBAL SETUP, AUDIO > USB > OUT chooses what goes out: **STEREO**, the master as above, or
**TRACKS**, six mono channels, track 1 on channel 1 to track 6 on channel 6, to record the tracks apart. Each track
is taken after its level, before its pan and before the master delay and reverb. When you change it, the keyboard
leaves the USB for a moment and comes back as the new device (with six channels, or two); the recording program may
need to choose it again. The setting is kept, and the keyboard starts as it was left.

### Voices and load

The six tracks are rendered a block of 32 samples (0.73 ms) at a time. Every sounding voice takes a share of that
block. When the total passes **82 %**, Tonitor protects the audio: it ends the quietest fading tails first, then
the quietest voices, each faded over a few milliseconds so nothing clicks. A latched note is never ended this way.
Below 82 % nothing is cut. What each engine costs, and how much you can stack, is measured in
[docs/smk37/engine-costs.md](docs/smk37/engine-costs.md).

With all twelve voices busy, a new note takes the release tail that has rung longest (on any track), else the
oldest note held on its own track, else the oldest held anywhere, never a latched one.

Every sound ends. Once its key is let go, a voice is faded out over 0.19 s when it has been 66 dB under its
loudest for 0.2 s, when it has not fallen 1.5 dB in 2 s, or 30 s after its release began. Held and latched notes
are not touched.

### Starting, rescue and UBOOT

At power-on the screen shows TONITOR and the build (BUILD 31, for example), then the track view.

**RESCUE.** Hold **OCTAVE−** alone while switching the keyboard on: it starts in the rescue mode. The screen shows
**RESCUE**, **CONNECT USB**, and **RUN INSTALL** (or UNKNOWN FLASH if it cannot read its flash). Connect it to the
computer and run the installer: the rescue mode accepts a new package as the normal mode does. Nothing sounds and
nothing is written in this mode until the installer writes. If Tonitor fails to start twice in a row, the
keyboard starts in RESCUE by itself.

**UBOOT.** Hold **OCTAVE−** and **OCTAVE+** together for 5 seconds while Tonitor runs: the audio stops, the screen
shows **UBOOT** in blue, and the chip restarts into its own built-in updater (it appears on USB as WL82
UBOOT1.00). It is a way back whatever happens, and it is how the flash backup tool reads the keyboard (see
[docs/smk37/backup.md](docs/smk37/backup.md)). Switch the keyboard off and on to leave it. Holding both buttons at power-on does not
start RESCUE; hold them 5 seconds once Tonitor has started.

## The engines

Each track plays one of eight engines, and keeps a sound of each. **HOME** draws a new sound on the track's
engine, leaning to its path; **SHIFT + HOME** draws one of the same family; **SHIFT + < / >** draws one of the
family before or after. Every sound is drawn from a seed, measured as it is made, and given the gain that brings
it to one loudness. What each engine costs on the SMK-37's chip is in
[docs/smk37/engine-costs.md](docs/smk37/engine-costs.md).

### FM — six operators, DX7 style

The classic six-operator FM (msfa, the core of Dexed: 32 algorithms, rate/level envelopes, ratio or fixed
frequencies, feedback, LFO, pitch envelope). FM plays from the shared pool of voices and is the lightest of the
voiced engines. HOME draws a patch from one of the families of sound objects below.

| Family | After |
| --- | --- |
| **METAL** | bells and gongs: many carriers on inharmonic ratios, the high partials dying first |
| **DRONE** | spectral drones: near-harmonic pairs detuned to beat, an index wandering over minutes |
| **BRUIT** | noises: feedback and high ratios scattered by a sample-and-hold LFO |
| **IMPACT** | struck bodies of several kinds: thuds, wooden knocks, metal hits, drum skins, debris; the modulators dying before the carriers; the pitch falling to the note now and then |
| **ITERATIF** | iterative objects: a sound pulsed by its own LFO, square or random |
| **SPECTRE** | a spectral arpeggio: rich tones a few cents apart whose beating lifts one harmonic after another |
| **ABYSS** | sub-bass rumble folded by feedback, breathing slowly |
| **FRICTION** | creaking: sub-audio fixed-frequency modulators and random pitch |
| **CHOIR** | vowels: carriers on the harmonics nearest two formants, a late vibrato |
| **BOWED** | a bowed string: a saw from feedback, the bow's pressure rising with the swell |
| **HOLLOW** | odd harmonics only: a dark pipe |
| **BLOOM** | a tone that opens over seconds and half closes again |
| **TIDE** | three stacks crossfading, one fading as the next swells, under a slow tide of level |
| **SHARDS** | a tone broken by random steps of pitch and chopped by a square |
| **STATIC** | ring-modulated bands from modulators nailed to audio frequencies |
| **WIND** | breath and gusts over a soft drone |
| **PLUCK** | plucked strings, bright or dark: the modulators die at once, the carriers die away, higher strings quicker |
| **MALLET** | struck bars (marimba, vibraphone, kalimba): a high modulator's knock, gone at once, the bar ringing on |
| **WARP** | anything: ratios anywhere, partials nailed to frequencies, the pitch bent about by its envelope; now and then a long glide (with its siren), a pitch bouncing in at every note (up past it, back under, onto it), or both |
| **GLITCH** | broken, mechanical sounds: the level chopped by a square or random LFO, a crunch of heavy feedback with fixed-frequency modulators, the pitch thrown between steps, a slow uneven wobble |
| **SHAPER** | a carrier nailed to 1–19 Hz under modulators that follow the key: the carrier hardly turns, so it works as a wave shaper, and the tone rolls over between its odd and even harmonics a few times a second, at the same pace up and down the keyboard |
| **LADDER** | four to six carriers on a ladder of ratios (near unison, an octave stack, uneven steps), their envelopes staggered: the partials enter one by one, or die one by one |
| **REVERSE** | a sound that swells for as long as the key is held and stops when it is let go: tape run backwards |
| **CREEP** | a held note leaving its pitch slowly, over seconds to tens of seconds |
| **CLANG** | Chowning's bells as the DX7's programmers made them: carriers turned by modulators on ratios off their harmonics, the index dying with the note |

**GLISSE** (glissandi: a pitch envelope travelling octaves) and **RUBBER** (low ratios, the pitch bouncing in at
every note) are no longer drawn: WARP took them over, with their weight. A sound saved as a GLISSE or RUBBER under
an earlier generation still loads as it was; SHIFT + < / > steps over them.

SHAPER, LADDER, REVERSE, CREEP and CLANG were learned from a collection of 9,606 DX7 voices: the idioms its
programmers used that the other families do not make. Each draws one of its family's skeletons (algorithm,
feedback, every operator's frequency and level, taken from the collection), an envelope shape for each operator,
and the rest within the family's ranges: new sounds of that kind, never one of the collection's voices again.

Each patch is then shaped with care: modulators on high ratios turned down (their sidebands alias and scream),
the brightness scaled down the keyboard so high notes are not shrill nor low ones muddy, rate scaling on the
carriers. A patch may also be made plucked whatever its family, have a modulator moved to another ratio, or an
operator detuned against the rest, and about half the patches are bent once more, gently. HOME draws several
patches, measures each, and keeps one whose brightness suits the track's path (darker on AMBIENT, brighter on
EXPERIMENTAL), avoiding the family the track had.

On **AMBIENT**, FM favours DRONE, SPECTRE, CHOIR, BOWED, HOLLOW, TIDE, MALLET, LADDER, REVERSE and CREEP, draws
WARP seldom, and never draws BRUIT, IMPACT, FRICTION, SHARDS, STATIC or GLITCH; then softer attacks, long tails,
darker modulators, slow LFOs, no sirens, often an octave down. On **EXPERIMENTAL** it favours WARP most, then
GLITCH, BRUIT, FRICTION, SHARDS, STATIC, SHAPER and CLANG, brighter and bent more often, and never draws SPECTRE,
CHOIR, BOWED or TIDE.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **BRIGHTNESS** | the modulation index about 27 dB either way (down: towards pure sines; up: the feedback too), the carriers tilted (the higher partials up) |
| K2 | **DETUNE** | the carriers pulled apart ±40 cents and the modulators ±25 cents off them: beating, chorus, the partials off their places. **INHARMONIC** on METAL, BRUIT, IMPACT, ITERATIF, FRICTION, SHARDS, STATIC, WIND, WARP, GLITCH, MALLET and CLANG: every partial off its ratio (clangour) |
| K3 | **SATURATION** | feedback and saturation turning up; a low-pass, smoother, turning down |
| K4 | **MOTION** | the LFO's depths; turning up, a tremolo, quicker and deeper |
| K5 | **ATTACK** | every operator's first stage, 1/8 to 8 times as long |
| K6 | **RELEASE** | every operator's last stage once the key is up, 1/8 to 8 times as long |
| K7 | **LFO RATE** | the LFO 1/4 to 4 times as fast |
| K8 | **VELOCITY** | above the middle, the note's velocity on the brightness: soft darker, hard brighter, up to about 27 dB between the ends at full; at or below the middle, as drawn |

### ANIMA — a lo-fi synthesizer played by a simulated hand

ANIMA is the lo-fi engine: each voice runs at half the sample rate (22.05 kHz, so nothing above 11 kHz), its
samples drawn out to the full rate by straight lines between them. Its pitches, rates and filters are scaled to
sound where they would at the full rate; what goes is the top octave of air, and half of the chip's time.

A fixed-point port of the *anima* opcode from Vascular, rebuilt for Tonitor. While a key is held, a simulated
finger rests on a virtual surface and drags one figure after another (orbits, figures of eight, scribbles, slow
glides, now and then a flick), pressing in slow swells, and now and then lifting to land elsewhere. A ghost finger
comes and goes on an irregular clock. Where the finger is, how hard it presses, how fast it moves and how long it
has been down move the timbre, never the pitch.

The voice: two wavetable oscillators (24 band-limited single cycles), an FM pair, a stack of copies, a grain cloud,
a filtered noise, a resonant filter, a wavetable tremolo; then the hand's chain: a downsampler, a waveshaper, a
phaser, a second filter and a comb resonator tuned to the note. Each new sound is a *design*: which of these are
in, on which waveforms, and for every parameter which gesture moves it, over what range, along what curve, plus a
mod matrix of three to seven routes and Anima's eight sliders, drawn, often far from their middle.

Twenty-two kinds: **HAZE**, **VOYAGE**, **GLASS**, **BELLS**, **GOLDEN** (Anima's own) and **BREATH**, **GRIT**,
**SWARM**, **TIDE** (waves of noise over a low drone), **CHOIR** (vowels, a formant the hand moves), **SHARDS**
(glass grains scattered and crushed), **BOWL** (a beating inharmonic hum in a ringing comb), **DUST** (sparse
crackle over a dark drone), **MACHINE** (a pulsing, folded mechanism), **PLUCK** (a burst through the comb tuned to
the note, dying away), **REED** (nasal, through a formant the hand moves, a breath in it), **FOG** (a stack under
fog, a slow phaser drifting through it), **ORGAN** (drawbars, a rotary tremolo, a little drive), **BITS** (pulses in
few bits: the rate crushed and dragged about, folded), **DRIFT** (two saws drifting against each other, a chorus
the hand moves), **MURMUR** (a dark cloud of noisy grains) and **HOWL** (noise through a resonance so sharp it
whistles, swept by the hand).

The filter follows the keyboard, so low notes are not muffled nor high ones shrill; the second oscillator's
interval suits the path (consonant on AMBIENT, anything on EXPERIMENTAL); and an AMBIENT sound nearly always has a
resonant space to ring in. On **AMBIENT**, ANIMA draws mostly HAZE, GOLDEN, VOYAGE, TIDE, CHOIR, BOWL, FOG and
DRIFT, never GRIT, SHARDS, MACHINE, BITS or HOWL, with a darker tone, more space, a slower hand, long releases and
no crusher. On **EXPERIMENTAL** it draws mostly GRIT, SWARM, SHARDS, MACHINE, BITS and HOWL, never HAZE, TIDE,
CHOIR, FOG or DRIFT, and makes them wild: the sliders near their tops, the resonance often screaming, a crushed
sample rate, folding drive, a band of noise and scattered grains, often a fast tremolo, the comb ringing hard.

In Vascular a design plays up to six fingers, each a voice; here a voice has one finger on the sound. ANIMA plays
from the shared pool of voices; PLUCK is the lightest kind, FOG and BREATH the heaviest.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **TONE** | every filter four octaves up or down (sharper when up), the FM brighter or darker |
| K2 | by kind | what the kind is about, from nothing to far more: **GRAINS**, **NOISE**, **FM** (the index), **FORMANT** (sharper when up) or **DRIVE** (folded hard; turned down, a pure tone) |
| K3 | **RESONATOR** | much wetter and ringing longer (brought in if the sound had none), or dry |
| K4 | **MOTION** | the hand four times quicker, a tremolo pulsing; or nearly still, no tremolo |
| K5 | **ATTACK** | 1/8 to 8 times as long |
| K6 | **RELEASE** | 1/8 to 8 times as long |
| K7 | **GRAIN SIZE** | the grains 1/4 to 4 times as long |
| K8 | **PHASER** | deeper and a little quicker (brought in if the sound had none), or shallower, gone at the bottom |

### CALIGULA — an organism

After the architecture of SOMA's LYRA-8 as Miguel Moreno's LIRA-8 emulates it (BSD licence), rebuilt for Tonitor
and bent towards dark, restless textures. Eight oscillators in four pairs on two sides; each a triangle that its
pair's sharpness pushes towards a square; each with a **sensor** and an envelope, slow (a long swell, a longer
fall) or fast; each pair frequency-modulated at audio rate by its partner, the other pair of its side, a pair of
the other side or the **total feedback** (the whole organism, saturated, fed back in). A **hyper LFO** of two LFOs
(alone, AND, OR, XOR) opens and closes the modulation and gates chosen pairs into irregular rhythms; a short
modulated delay (two lines, ~35 ms) choruses, flanges or warps it; a drive.

A CALIGULA track is **played like the others**: the keys play the scale's notes, and it is **silent until a key is
down**. A key wakes the organism at that note; a second key on top moves the whole organism to the new note at
once (a snap, not a glide), and letting go of it goes back to the key still held; letting go of the last key lifts
the hand and the organism rings out. It is **rooted**: tuned around the note played, in intervals of it (octaves,
fifths, unisons and semitone clusters that beat, tritones, the depths). The hyper LFO moves timbre, never pitch:
no sirens. It is one organism a track, outside the pool of voices: its cost is the same whatever the number of
notes.

Nobody touches the eight plates: **a simulated hand** does, while a key is held. Each species has its own hand:
how often it touches, how long a finger stays, how many fingers at most, its favourite plates, quick taps against
long holds. The first finger stays down until the key is let go, so the sound never drops out under a held key.
Around the hand the organism has a life of its own: the modulation depths and the sharpness wander, oscillators
fire in **bursts** at irregular times, and now and then the feedback and the drive **surge** for a moment. A
pair's envelope may be slow: a swell of seconds, a release of up to nine seconds after the key is let go.

The hand often has a **phrase**: two to four plates it keeps coming back to, varied now and then, often on a pulse
of its own (long, short, short, longer), so it plays like someone rather than like chance. **HOME** draws a new
organism from ten species, tuned on one of ten sets of intervals (pillars of octaves and fifths, semitone
clusters, the depths, the harmonics, unisons, tritones, fourths, a minor seventh, open fifths, a quarter-tone
cluster):

| Species | |
| --- | --- |
| **SPORE** | slow swells, held drones, breathing; low |
| **THORN** | sharp, deep FM, driven, bursting often |
| **HIVE** | the hyper LFO quick and gating: irregular rhythms, bursts |
| **CARRION** | the depths, heavy total feedback, the delay warped by the voices, surges |
| **VEIL** | mostly triangles, light modulation: the calmest |
| **RUST** | driven, cross-modulated, surging, sometimes rhythmic |
| **LITANY** | a slow choir: unisons and octaves beating, nothing sharp, a hand that stays |
| **UNDERTOW** | deep: heavy feedback and a warped delay pulling it under, slow surges |
| **WIRE** | high and sharp, clusters and quarter tones, a quick hand tapping, bursts |
| **MOTH** | soft triangles fluttering through the gates, on a pulse |

On **AMBIENT**, CALIGULA draws SPORE, VEIL and LITANY most, then UNDERTOW, CARRION and MOTH, sometimes RUST; tuned
on roots, fifths, octaves, depths and unisons; slow swells, softer edges, fewer bursts and surges, a slower hand.
On **EXPERIMENTAL** it draws WIRE, THORN, HIVE and RUST most, never VEIL or LITANY; semitone clusters and tritones;
sharper, more driven, more bursts and surges, a quicker hand.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **WAVEFORM** | triangles to squares |
| K2 | **MOD DEPTH** | how deeply the pairs modulate each other |
| K3 | **FEEDBACK** | the total feedback and the drive; **DELAY** (the delay's mix and feedback) for SPORE, VEIL, LITANY and MOTH |
| K4 | **TOUCHES** | how busy the hand is, its touches and bursts; **GATE RATE** (the hyper LFO's speed) for HIVE, MOTH and WIRE |
| K5 | **ATTACK** | the pairs' envelope attacks, 1/8 to 8 times as long |
| K6 | **RELEASE** | the pairs' envelope releases, 1/8 to 8 times as long |
| K7 | **SPREAD** | each oscillator's detuning off its semitone, from none to twice |
| K8 | **DELAY TIME** | both delay lines 1/4 to 4 times as long |

### RUSSOLO — noise, logic and broken buffers

RUSSOLO re-makes the techniques of Robert Heel's Noisferatu (GPL-3.0) in fixed point, for the keys. It runs at a
third of the sample rate (about 14.7 kHz) in 10-bit values, each held until the next (the grit of a small DAC),
then reduces its bits and holds its samples further as the sound says.

A RUSSOLO track is **played like the others** and is **silent until a key is down**: the key opens an envelope and
sets the pitch (the oscillators' frequency, the speed a buffer is read at, the rate noise is sampled at); a second
key on top moves it there at once; letting go of the last key lets it fade. Like CALIGULA it is one organism a
track, outside the pool of voices; more notes cost nothing more. It is the lightest engine. **HOME** draws one of
seventeen kinds:

| Kind | |
| --- | --- |
| **HOLD** | sample-and-hold noise, its rate from the key |
| **DUST** | random clicks through a low-pass that follows the key |
| **GATES** | noise opened and closed by two triangles, one wandering off the other |
| **CRACKLE** | sparse clicks ringing on short pitched decays, over hiss |
| **NOR**, **XOR** | two oscillators combined bit by bit |
| **RING** | two oscillators multiplied |
| **CASCADE** | squares modulating squares, a sub-octave below |
| **CLANG** | two triangles at an inharmonic ratio, XOR'd; the ratio redrawn when two clocks meet |
| **CHUNKS** | a generated buffer of noise, saw and silent chunks, read at the key's speed and rebuilt in parts |
| **JUMP** | the buffer read with jumps at random |
| **WANDER** | a window wandering through the buffer |
| **BEND** | an address bit of the buffer forced to 0 or 1, flipped or held; which and how change on a clock |
| **QUAD** | four such bendings at once, each switched on and off by chance |
| **BLIPS** | decaying triangles fired by chance on the key's harmonics |
| **WALK** | a random walk over a scale's steps from the key |
| **CHORD** | triangles on the key (minor, major, open fifths or suspended), slowly beating and swelling |

Its level is measured when it is drawn, and never raised so far that its loudest click could clip. On
**AMBIENT**, RUSSOLO draws CHORD most, then DUST, CRACKLE, BLIPS, WALK and WANDER, never the logic kinds (NOR, XOR,
RING, CASCADE, CLANG), JUMP, BEND or QUAD; clean (9–10 bits), slow attacks, long releases, calmer chance, no major
chords. On **EXPERIMENTAL** it draws NOR, XOR, CLANG, BEND and QUAD most, never WALK or CHORD; crushed to 4–9 bits,
often sample-held, abrupt attacks and short releases, chance driving it hard.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | the kind's first parameter | named for what it does: RATE, TONE, WIDTH, HISS, RATIO, SPREAD, DENSITY, … |
| K2 | the kind's second parameter | CHANCE, DENSITY, WANDER, DRIFT, SUB, CLOCK, CHANGE, … |
| K3 | **CRUSH** | fewer bits and longer sample holds, or cleaner |
| K4 | **RANDOMNESS** | how much chance drives it; above the middle, stutters too (its sample frozen a moment) |
| K5 | **ATTACK** | 1/8 to 8 times as long |
| K6 | **RELEASE** | 1/8 to 8 times as long |
| K7 | **CHORD** | on the CHORD kind, the chord moved up to three types away (minor, major, open fifths, suspended) |
| K8 | **HOLD** | each sample held longer, or shorter |

### DRUID — strings and resonant bodies

DRUID is a fixed-point port of the resonators of **Mutable Instruments Rings** by Émilie Gillet (MIT licence). It
plays from the shared pool of voices. Each kind is one of Rings' resonators and an exciter:

| Resonator | |
| --- | --- |
| **String** | a Karplus-Strong string: a delay line read with Hermite interpolation, damped by a short FIR filter (brightness) and a state-variable low-pass, its decay set as a time to −60 dB. Dispersion (STRUCTURE) stretches the partials with an all-pass stage, adds noise to the period near the top, or below a quarter of the range bends the bridge so the string buzzes. The output is picked up at the position along the string |
| **Sympathetic strings** | a string and three more tuned to intervals of its note (octaves, fifths, a twelfth, chosen by STRUCTURE), driven only by the first string's sound; their pickup positions drift slowly |
| **Modal** | up to 16 two-pole resonators at the partials of a bar or plate, stretched or squeezed by STRUCTURE; each partial's level depends on the position |

| Exciter | |
| --- | --- |
| **Pluck** | Rings' plucker: a burst of noise as long as the position's share of the period, through a comb filter and a low-pass |
| **Strike** | a short pulse, longer and softer as the brightness goes down |
| **Bow** | noise for as long as the key is held, swelling in and dying away when released |
| **Dust** | random impacts for as long as the key is held |

| Kind | Resonator | Exciter |
| --- | --- | --- |
| **PLUCK** | string | pluck |
| **BOWED** | string | bow |
| **STRUCK** | string, stretched partials | strike |
| **SITAR** | string, buzzing bridge | pluck |
| **RATTLE** | string | dust |
| **HARP** | sympathetic strings | pluck |
| **DRONE** | sympathetic strings | bow |
| **BELL** | modal | strike |
| **BOWL** | modal | bow |
| **PLATE** | modal | dust |

On **AMBIENT**, DRUID favours BOWED, HARP, DRONE, BELL and BOWL, with long decays that carry on after the key is
released. On **EXPERIMENTAL** it favours RATTLE and PLATE, with shorter decays, faster dust, extreme positions and a
stronger damping when the key is released. Releasing a key damps the sound by an amount set per sound, from
ringing on to stopping short. Each voice's delay lines are 16-bit and use the memory an ANIMA voice would use: a
string reaches down to about 29 Hz, and sympathetic strings that would not fit their shorter lines are tuned an
octave up. HARP and DRONE (a string with three sympathetic strings) are the heaviest sounds of all the engines.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **STRUCTURE** | on a string, its dispersion (below a quarter, a buzzing bridge; above, the partials stretched, then noise in the period); on sympathetic strings, the intervals they are tuned to; on a body, how its partials are spaced, from squeezed through harmonic to bell-like |
| K2 | **BRIGHTNESS** | the exciter's low-pass and the string's or body's high-frequency loss |
| K3 | **DAMPING** | how long it rings, from about 70 ms to about 18 s to −60 dB, and near the top on without decay |
| K4 | **POSITION** | where it is plucked or struck and where it is picked up: which partials sound |
| K5 | **EXCITATION** | the bow's or the dust's level; a strike's or a pluck's, half to twice |
| K6 | **BOW** | the bow's attack, up to 0.75 s shorter or longer |
| K7 | **DUST** | the dust's rate, up to 30 impacts a second more or fewer |
| K8 | **MUTE** | how much the sound is damped when the key is released |

### BAYAREA — a West Coast voice

BAYAREA is a fixed-point port of the bayarea opcode of Vascular, contributed by its author. It plays from the
shared pool of voices. Each voice has:

- an oscillator that morphs from sine through triangle and square to saw (SHAPE), its frequency modulated
  linearly, through zero, by two sines at ratios of it (0.66 and 3.3 by default, other ratios on some sounds), each
  ratio wandering slightly and slowly;
- a wavefolder (FOLDS), reflecting the wave back into range;
- a 12 kHz smoothing low-pass and a trace of noise;
- either a **low-pass gate**, where a resonant low-pass and an amplifier are struck together by one-shot envelopes
  (an attack of 2–3 ms and a decay), or a **sustaining envelope** over two resonant low-passes (an attack while the
  key is held, a release when it is let go). The filters sweep from 300 Hz to the sound's peak frequency, higher
  for louder notes;
- a saturating output stage (tanh, then a soft clip) with its own drive;
- a bank of three resonators (508, 270 and 1153 Hz, ringing for 1–1.4 s), mixed in by the sound's amount;
- an LFO (triangle, ramp, square or random steps) that moves the shape, the folds, the two FM depths, the attack,
  the peak, the decay and the resonator mix, by amounts drawn with the sound.

Changed from the opcode: the square and saw are band-limited with polyBLEP instead of being summed from 44
partials (too costly for this chip), and the opcode's reverb is replaced by the track's sends to the master delay
and reverb, keeping only its resonator bank.

| Kind | Envelope | |
| --- | --- | --- |
| **BONGO** | gate | low peak, short decay, little folding |
| **PLUCK** | gate | brighter, square to saw, short to medium decay |
| **CHIME** | gate | the upper modulator deep, long decay, resonators |
| **FOLDED** | gate | heavy folding |
| **SWELL** | sustain | slow attack (0.3–3 s), long release |
| **DRONE** | sustain | slower attack (0.5–4 s), the lower modulator deep, long release |

On **AMBIENT**, BAYAREA favours BONGO, CHIME, SWELL and DRONE. On **EXPERIMENTAL** it favours PLUCK and FOLDED, with
more drive and folding and other FM ratios more often. The percussive kinds (CHIME, FOLDED, BONGO) end quickly and
free their voices.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **SHAPE** | the oscillator from sine through triangle and square to saw |
| K2 | **FOLDS** | the wavefolder's drive |
| K3 | **FM** | both modulators' depth |
| K4 | **DECAY** | the gate's decay, or the sustain's release, from an eighth to eight times the sound's own |
| K5 | **ATTACK** | 1/8 to 8 times as long |
| K6 | **FM RATIO** | both modulators' ratios, 1/4 to 4 times |
| K7 | **SPRING** | the resonator bank's mix |
| K8 | **DRIVE** | the output stage's drive, 1/4 to 4 times |

### HIVE — a swarm of oscillators

HIVE is a port of the SWARMER machine of DigiRust, contributed by its author, extended here with inharmonic and
textural kinds. It plays from the shared pool of voices. Each voice has seven oscillators, and each sound draws,
for every one of them:

- its place relative to the note: the note itself, an octave or a fifth, a step of a cluster (quarter-tones,
  seconds), or an inharmonic partial (the modes of a free bar, a bell's partials, a stretched harmonic series, or
  ratios at random);
- its share of the spread that DETUNE sets (symmetric, clustered, to one side, at random, or in pairs);
- its wave, a position along sine, triangle, saw and square (neighbouring waves crossfaded);
- its level, and which of three LFOs moves its pitch and its level, and at what phase. The LFOs' rates are drawn
  too, from about 0.02 Hz to a few Hz depending on the kind; MOVEMENT scales their depth.

The seventh oscillator is the main one, set against the other six by MIX. Each voice also has a slow noise on every
oscillator's pitch; how closely it follows the key (fully for the melodic kinds, partly or hardly at all for the
textural ones); noise; a ring modulation of the first oscillator by the main one; a 4-pole filter (two
state-variable filters, low-pass, band-pass or high-pass) whose cutoff follows half the key's movement and is swept
by its own envelope and by the first LFO; an ADSR envelope on the level; and a drive (tanh).

Changed from DigiRust: the waves are computed as they play (the sine from its table, the triangle directly, the saw
and square band-limited with polyBLEP) instead of read from band-limited tables, which would not fit in RAM here;
the filter, envelopes and drive are Tonitor's own. Silent oscillators are skipped; with three HIVE voices or more
sounding, oscillators 12 dB under the loudest are skipped too, to save the chip's time.

| Kind | |
| --- | --- |
| **SUB** | the main oscillator one or two octaves down, short attack; sometimes an octave up in the swarm |
| **LEAD** | brighter, short attack, resonant |
| **STAB** | short attack and decay, little or no sustain, a strong filter sweep; sometimes a fifth and an octave in the swarm |
| **PAD** | slow attack (0.2–3 s), wide detune, long release; sometimes octaves in the swarm |
| **DRONE** | slower attack, full sustain, long release (1.5–6 s); sometimes fifths and octaves stacked |
| **WASH** | the slowest attack (0.8–5 s), the longest release (3–9 s), often noise and a moving filter |
| **CLUSTER** | the oscillators at quarter-tone and semitone steps around the note, slowly beating |
| **METAL** | inharmonic partials (bar, bell, stretched or random), struck: short attack, a long decay, often a ring modulation |
| **SPECTRA** | inharmonic partials whose levels and pitches turn slowly (or shimmer), the key hardly followed: a spectrum evolving over seconds |
| **GROUND** | two octaves down, a dense inharmonic cluster with noise, a slowly moving filter, driven: rumble |
| **AIR** | filtered noise and one to seven high, drifting whistles, band-pass or high-pass, the filter swept: wind |
| **TIDES** | the oscillators gliding apart and back by up to five semitones on slow LFOs, sometimes on inharmonic partials |

CLUSTER, METAL, SPECTRA, GROUND, AIR and TIDES are meant for non-melodic, textural use. On **AMBIENT**, HIVE favours
PAD, DRONE, WASH, SPECTRA and TIDES, and makes the LFOs slower, the sound darker and the attacks longer. On
**EXPERIMENTAL** it favours STAB, CLUSTER, METAL, GROUND and AIR, and adds more noise to the pitches, more drive and,
sometimes, a ring modulation. AIR is the lightest kind; LEAD the heaviest.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **DETUNE** | the swarm's spread around its places, from none to twice the sound's own |
| K2 | **MOVEMENT** | the LFOs' depth on the pitches, the levels and the filter, from none to twice |
| K3 | **MIX** | the swarm against the main oscillator; **NOISE** (the noise's level) on GROUND and AIR |
| K4 | **CUTOFF** | the filter's cutoff |
| K5 | **RESONANCE** | the filter's resonance |
| K6 | **FILTER ENV** | how far the filter's envelope sweeps it |
| K7 | **RING** | the ring modulation's amount |
| K8 | **SHIMMER** | how deeply the LFOs move the oscillators' levels |

### OBRADOR — an analog polysynth

An analog polysynth voice after the Oberheim SEM and the two-voice Oberheims, written for Tonitor with OberVave
(an Oberheim-style firmware for the FM-1, by zvenson) as its frame of reference; the same engine as Tonitor's on
the FM-1, with four more macros here. It plays from the shared pool of voices. Per voice: two oscillators (saw,
pulse or both, band-limited), the second detuned or an interval away, hard-synced or cross-modulating now and
then; in the lush kinds a second voice card, detuned against the first; a drive into the SEM's 12 dB filter, its
output morphing from low-pass through notch to high-pass, its resonance soft-limited; a second low-pass stage for
24 dB, or the band-pass; filter and amplifier envelopes on analog curves; an LFO that comes in after a delay; and
each card's own small offsets of tune and cutoff with a slow drift. The pitch wheel bends it.

| Kind | |
| --- | --- |
| **STRINGS** | two cards of saws, a slow attack, a delayed vibrato: the string pad |
| **BRASS** | the envelope opening the 24 dB low-pass |
| **PWM PAD** | two pulses, their widths moving slowly |
| **BASS** | the second oscillator an octave down, the 24 dB low-pass, a plucked envelope |
| **SYNC** | the second oscillator synced, its pitch swept by the envelope |
| **NOTCH** | the filter near its notch, the LFO moving it: a phasing sweep |
| **METAL** | cross-modulation at an odd interval through the band-pass: bells, clangs |
| **DRONE** | two cards a fifth or an octave apart, the cutoff and the morph moving slowly |
| **KEYS** | a struck filter, no sustain |
| **SWELL** | the filter opening slowly under a held chord |

On **AMBIENT** it is softer, slower and darker, and favours STRINGS, PWM PAD, DRONE and SWELL. On **EXPERIMENTAL**
it is harder (more drive, more resonance, cross-modulation now and then), and favours SYNC, NOTCH and METAL.

**Macros.**

| Knob | Macro | |
| --- | --- | --- |
| K1 | **CUTOFF** | the filter's cutoff, four octaves either way |
| K2 | **MORPH** | the filter: low-pass, notch, high-pass |
| K3 | **RESONANCE** | the filter's resonance |
| K4 | **DETUNE** | the oscillators' and the cards' detune, from none to three times as wide |
| K5 | **FILTER ENV** | how far the filter's envelope sweeps it, 24 semitones either way |
| K6 | **LFO** | the LFO's depth, from none to three times; turned up, a vibrato of its own too |
| K7 | **DRIVE** | the drive into the filter |
| K8 | **VINTAGE** | the cards' offsets of tune and cutoff and their drift, from none to twice |
