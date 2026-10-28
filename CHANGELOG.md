# Changelog

This is the changelog of the Tonitor port for the M-VAVE SMK-37 Pro, newest first. The port began from Tonitor 0.53 for the FM-1 (commit c39d18f of nzimas/tonitor). Build 1 never shipped, so the list starts at BUILD 2. For the history before the port, see the [FM-1 Tonitor changelog](https://nzimas.github.io/tonitor/changelog.html).

## BUILD 43

28 October 2026

Changed
- Fader 1's morph by hand has its own setting: the morph time (K3 on the SNAPSHOTS page) goes below OFF to MANUAL.
  In MANUAL, tapping a snapshot's pad does not recall it: it becomes the target (its pad blinks) and nothing changes
  until fader 1 moves. From wherever the fader stands to the end furthest from it (the top, or near the top the
  bottom), it takes the machine from the snapshot it is at to the target, back and forth as you move; at the end
  the target is reached exactly and becomes the current snapshot, and the fader lets go until the next tap. With any
  other morph time, a pad recalls as before and fader 1 does nothing. (Before, fader 1 morphed to the snapshot
  framed with < >, starting from the bottom, which a tap on a pad could not reach.)

## BUILD 42

27 October 2026

Changed
- The GESTURES pads glow faintly sky blue while the page is up, the empty ones too, so the pads show they are the
  gesture loops' before any is recorded. A recorded loop that is off is half-lit; on, lit.

## BUILD 41

27 October 2026

Changed
- Gestures are now **gesture loops**, apart from the note loops altogether: eight a track, on the bottom row of pads
  when the sixth button of the top row is pressed (GESTURES; lit while up). A pad held 0.8 s is armed (blinking red,
  ARMED); the first move of a knob or fader records from there; the pad pressed ends it and it loops at once, at
  its own length, whatever else plays. A recorded pad pressed switches it off or on (from its beginning); an armed
  one pressed lets it go. Besides the track's knobs (filter, granulator, crush, distortion, sends, level, pan, the
  eight macros), faders 2 and 3 (SPACE, the master filter) are recorded too. Sessions (format 34) keep them;
  the gesture layers of build 39 load into the first three.
- The note loops are as before build 39: eight a track on the OVERDUBS page, REC arming them alone.

## BUILD 40

27 October 2026

Changed
- Gesture layers record with REC too, as loops do: REC armed, then a knob turned before any note, the take is of
  the knobs. It goes into the first empty gesture layer, starting with that turn (REC lit, G1 REC on the screen);
  REC again ends it and it plays at once. A note played first makes it a loop as before. With five loops already,
  REC arms for a take of the knobs alone (ARMED G); held, REC still empties the looper. Before, only the gesture
  pads recorded, and arming REC and turning a knob recorded nothing.

## BUILD 39

27 October 2026

Added
- Gesture layers: three a track, G1 to G3, on pads 14 to 16 of the OVERDUBS page (PLAY held). An empty pad
  pressed records the track's knobs as they are turned (the filter, granulator, crush, distortion, delay and
  reverb sends, level, pan and the eight macros); pressed again it ends, its length the time between the two
  presses, and it plays them back, round and round, at that length. A recorded pad pressed switches the layer off
  or on; held 0.8 s, empties it. A layer's own length drifts against the loops', so its gestures fall on other parts
  of the loop at every round: by design. A layer plays while the track's loops play, or freely on a track without
  loops. The layers share a store of 1024 moves (GESTURES FULL). Sessions (format 33) keep them; earlier sessions
  load with none.

Changed
- A track records up to five loops (the first and four overdubs: pads 9 to 13), eight before. A session with more
  still loads and plays them all; pads 14 to 16 are the gesture layers.

## BUILD 38

27 October 2026

Added
- Faders 1 to 3, live controls over the whole instrument, never saved (fader 4 stays the master level):
  - Fader 1, MORPH: a morph by hand between the snapshot you are on (bottom) and the one framed on the SNAPSHOTS
    page (top). Values that glide follow the fader; what has no in-between switches at the middle, in either
    direction; a track whose sound differs passes through silence there. It takes hold only once it has been at
    the bottom.
  - Fader 2, SPACE: every track's reverb and delay sends, the reverb's decay and the delay's repeats raised
    together, towards a wash at the top. The stored values are left as they are.
  - Fader 3, the master filter: low-pass below the middle, off at the middle (with a small dead zone), high-pass
    above, over the whole mix after the delay and the reverb. It glides smoothly.
  - Each fader shows its dial while it moves: MORPH 1 TO 3 and how far, SPACE and its share, FILTER LP 1.2K.

## BUILD 37

27 October 2026

Fixed
- The track filter (K3) moved in steps: each click of the knob jumped the cutoff about 0.6 of an octave at once,
  and the filter's settings changed only every 32 samples. Now a click turned slowly moves it a third of an octave
  (turned fast, two thirds, so the whole way still takes a turn and a half), and the cutoff and resonance glide
  there over about 25 ms, recalculated four times as often while they move: a smooth, continuous sweep, as an
  analog filter's knob. Modulation of the filter (MOD) keeps its quick response, so fast LFOs are not smoothed away.

## BUILD 36

27 October 2026

Brought in from Tonitor for the FM-1 (its builds 55, 61, 62 and 68).

Added
- GLOBAL SETUP has an AUDIO section: USB, OUT: STEREO (the master, as before) or TRACKS (tracks 1 to 6, each on its
  own mono channel, after its level, before its pan and the master delay and reverb), to record the tracks apart.
  Changed, the keyboard leaves the USB a moment and comes back as the new device. Settings kept by earlier builds
  load as they were, at STEREO.
- While REACT, SHADOW or ARP is on, PLAY flashes with each note the guide plays (keys, chords, loops, BALLSY,
  BRICKLAYER, MIDI in), on every page but BALLSY's and BRICKLAYER's.

Changed
- The DELAY and REVERB pages: every row's name and value in one size (it changed from row to row). The delay's TIME
  in seconds, without the unit (0.02 .. 0.99, then 1.0 .. 2.0); FEEDBACK is named REPEATS.
- USB audio: the next packet is queued within 0.1 ms of the last (looked to 10 000 times a second, 2 000 before), so
  six channels never miss a frame; the ring is kept fuller.

Fixed
- USB audio on macOS 12 to 15: the keyboard now lists its audio streaming interface before its MIDI one. macOS up to
  15 drops the MIDI interface from that list as it reads it, and with MIDI listed first no audio input appeared
  there (MIDI still worked). The device's version number changes, so a computer reads its description again.

## BUILD 35

27 October 2026

Brought in from Tonitor for the FM-1 (its builds 58 and 61).

Fixed
- Starting up and starting a new (empty) session were slow: every track drew a sound for each of the eight
  engines, 48 in all, and measured each one by rendering seconds of it. Now only the sound each track plays is
  measured; another engine's is measured the first time the track moves to it (a moment, then), or a snapshot
  moves it there. On the computer an empty session now takes about a fifth of the work. Sessions, snapshots and
  copy and paste keep sounds not yet measured as they are; presets are always saved measured.
- A loop's first note could be heard only in the first round. A note held from the loop's first moment to its last
  was tied over the end as a drone, never struck again, so a plucked sound (DRUID's PLUCK) died away and stayed
  silent. Now a tied note that has died away (more than 18 dB under its loudest) is let go at the end and struck
  again at the start; one still sounding goes on as one note. The same note struck at the start and again, held,
  at the end is no longer taken for one note.

## BUILD 34

27 October 2026

Brought in from Tonitor for the FM-1 (its builds 59, 65 and 66), fitted to the SMK-37's panel.

Added
- SCALE held 0.8 s draws a new scale and root at random. The SCALE page shows them for 1.5 s, over any page, then
  the page that was up returns; the loops follow the new scale. SCALE pressed and let go still opens and closes the
  SCALE page (on release now). On the FM-1 this is SHIFT + black key 1.

Changed
- At start, and in a new (empty) session, the scale and root are drawn at random (until now C major).
- The splash screen shows the Tonitor logo: the ring mark in the middle, in the six tracks' colours, the lettering
  under it, the build below. It is drawn from the logo's shapes at start, with smoothed edges, not kept as an image.

## BUILD 33

27 October 2026

Brought in from Tonitor for the FM-1 (its builds 63 to 67), fitted to the SMK-37's panel.

Added
- OBRADOR, an eighth engine: an analog polysynth after the Oberheim SEM and the two-voice Oberheims. Two
  band-limited oscillators a voice (saw, pulse; detune, intervals, hard sync, cross-modulation), a second voice card
  in the lush kinds, the SEM's 12 dB filter morphing low-pass, notch, high-pass (24 dB and band-pass too), filter and
  amplifier envelopes on analog curves, a delayed LFO, each card's own offsets and drift. Ten kinds: STRINGS, BRASS,
  PWM PAD, BASS, SYNC, NOTCH, METAL, DRONE, KEYS, SWELL. Eight macros: CUTOFF, MORPH, RESONANCE, DETUNE (as on the
  FM-1) and, on K5 to K8, FILTER ENV, LFO, DRIVE, VINTAGE. The pitch wheel and the mod wheel move it as the others.
- Snapshots: 16 a session, one on each pad, each the whole machine at a moment: every track's engine, the sound
  on each of its eight engines with its macros and envelope, level, pan, sends, filter, granulator, crush,
  distortion, octave, transposition, path, chord mode, modulation, latched notes and chords (and their
  velocities), BALLSY and BRICKLAYER (its notes, STUTTER and climbs), and the loops (which loops, where each stood
  against the others, which are taken out, play, direction, speed, VARY, STUTTER, climbs); the reverb, the delay,
  the scale, the root, the loops' transposition and REACT, SHADOW and ARP. SAVE opens the SNAPSHOTS page: a pad
  let go soon recalls its snapshot, held 0.8 s stores the current state, with SHIFT held empties it; < > frame one
  and SAVE held stores into it. K3 sets the morph time, 0 to 30 s: continuous values change gradually, pitch and
  what has no in-between change at once halfway, a track whose sound changes fades out and back in.
- Loops held by a snapshot stay in memory after the looper is emptied, sharing its room (LOOPER FULL when there is
  none left).
- Sessions (format 32) keep OBRADOR's sounds, the snapshots with the loops they hold, the morph time, and REACT,
  SHADOW and ARP (not kept before). Earlier sessions load as before, with a new OBRADOR sound and no snapshots.
- The PRESETS page names each preset by its family, numbered within it in the order of the slots (BRASS 1, BRASS 2,
  STRINGS 1), each name in the largest size it fits; the title is the engine's name alone.

Changed
- SHIFT + SAVE opens the SESSIONS page (SAVE did before). GLOBAL SETUP is now SHIFT + SAVE held 0.8 s.

Fixed
- A sound chosen by family (SHIFT + < / >, SHIFT + HOME) came back as another sound, often of another family, when
  kept as its seed: from a preset, and from a session for the engines the track was not playing. Each engine now
  draws the family from the seed whether one is given or not, and sessions and presets keep the family with the
  seed, so the seed and the family make the same sound again. Sounds drawn without a family are unchanged. A
  preset saved by an earlier build from a sound chosen by family now loads in that family, but not as the same
  sound.

## BUILD 32

23 October 2026

Fixed
- On BRICKLAYER's page, LOOP's pads 12–14 (STUTTER, CLIMB DOWN, CLIMB UP) now set BRICKLAYER's own options, as
  pads 9–11, 15 and 16 already set its direction and speed. Before, they changed the track's loops instead. The
  pads show BRICKLAYER's settings there too.

## BUILD 31

23 October 2026

Changed
- The sound engines now run from faster memory. Six tracks on six different engines, one note each, used 146 % of the processor's time per block before: the audio could fall behind. Now they use 82 % (peak 96 %). Two notes on each of six tracks: 211 % before, 123 % now. An idle instrument went from 35 % to 14 %. You hear more voices before notes are cut.
- The reverb runs at half the sample rate (22.05 kHz). Its times and decay are kept; it sounds a little darker. It and the delay switch off while nothing goes in and their tails are gone.
- When the instrument has to let a voice go (too many voices, or a stolen voice), it takes the quietest tail first and fades it out over 6 ms. The cuts are quieter.
- HIVE is lighter: silent oscillators are skipped, and with three HIVE voices or more, those 12 dB under the loudest are skipped too.

## BUILD 30

23 October 2026

Fixed
- Latched notes are kept. When the instrument was overloaded it could let go of a latched drone, because nothing else is held so long. A new note could also take a latched voice when every voice was held. Neither happens now.
- A latched key pressed alone now strikes again, still latched. Before, it did nothing (only SHIFT let it go), so a latched note that had faded or been cut left its key dead on that track.

## BUILD 29

23 October 2026

Changed
- The six tracks start with these engines, in a new session too: FM, ANIMA, CALIGULA, HIVE, DRUID, BAYAREA.
- ARP: when you play a new chord, each track takes it at its next step, once the chord has stood for 90 ms. A chord never changes between two notes, so the pulse stays unbroken.
- Soft playing on the keys is heard. A slowly timed strike counts as a firmer one, and no key plays below velocity 16. Before, soft keys could sound silent on engines that need velocity.

## BUILD 28

23 October 2026

Added
- ARP, a third mode on REACT's page. With FADER BANK held, < and > now step REACT, SHADOW, ARP. In ARP, the tracks taken in break the chord you hold into arpeggios. They start again with each new chord. If you let go, they keep the last chord for a while. A single note is arpeggiated over its octaves. It follows keys, loops, BALLSY and BRICKLAYER.
- Six knobs on ARP's page, K3 to K8, in two rows of three, in lime: RATE (a step every 2 s down to 62 ms), RELATION (each track's pace against the rate), MOTION (each track's direction, the second against the first), MUTATE (patterns that change as they repeat), RANGE (1 to 4 octaves, the tracks apart) and CHANCE (steps left out, or struck twice). Textures arpeggiate four times slower.

Fixed
- Every sound now ends. Many sounds left a faint residue after release that never stopped, and only HOME silenced it. Twelve seconds after a note, 19 of 40 FM sounds and 24 of 40 DRUID sounds were still sounding. Now, once you let go of a key, its voice fades out over 0.19 s if it has been 66 dB under its loudest for 0.2 s, if it has not fallen by 1.5 dB in 2 s, or 30 s after release at the latest. Held and latched notes are not touched.

## BUILD 27

23 October 2026

Added
- Five new FM sound families learned from a collection of DX7 voices: SHAPER, LADDER, REVERSE, CREEP and CLANG. They are drawn on every path. The older families draw exactly as before. A sixth family, DIVE, was left out because it bends the pitch too much. REACT treats CLANG as a struck sound.

## BUILD 26

23 October 2026

Fixed
- SHADOW's note length is now worked out in a way the SMK-37's chip can run. Builds 24 and 25 used a calculation the chip cannot do.

## BUILD 25

22 October 2026

Added
- FM sounds are now drawn from a newer generation. GLISSE and RUBBER are no longer drawn as families of their own. WARP takes their weight, and now and then also GLISSE's long glide (with its siren), RUBBER's bounce, or both, and RUBBER's plain carrier ratios. A GLISSE or RUBBER asked for is a WARP, and SHIFT + < > steps over them. Sounds saved before this build are drawn exactly as they were.

Changed
- REACT keeps to the scale and the harmony. Every note it plays is snapped to the scale, where before a note held at the edge of the range could fall off it. A long note over what you hold moves to the nearest scale note that fits the harmony. In a test, 91 % of the notes fell in the chord held, up from 74 %.

## BUILD 24

22 October 2026

Added
- SHADOW, a second mode beside REACT. The tracks taken in shadow what you play (keys, chords, loops, BALLSY, BRICKLAYER). Each stays its own distance behind, at its own pace. Now and then one plays backwards or to and fro, rests, and comes in again. Rhythm is loosened, and notes are moved along the scale. On its page, < and > choose REACT or SHADOW. Four knobs: DELAY (right behind to 15 s), DRIFT (a little loose to half or twice the pace), VARIATION (how often its way changes) and TRANSPOSE (as played to a third to a sixth away). The memory it plays from grows to 128 events, each with its time.

Changed
- SHIFT + FADER BANK now turns the mode on or off, with no page. FADER BANK held brings the mode's page and the track pads up or down. FADER BANK pressed alone is still the ENVELOPE page. You can leave the page while the mode runs.

## BUILD 23

22 October 2026

Added
- REACT's page. With REACT on, its page shows the name of the knob turned last, its value large, and K3 to K6 side by side with a bar each. K1 and K2 stay level and pan.
  - DENSITY: how often they play, how long they wait, how many sit out.
  - COMPLEXITY: from single notes in plain rhythms up to harmonised notes, split and pushed steps, and threes against twos.
  - CONTINUITY: recombining what you played freely, or keeping to your phrases.
  - SPREAD: around your hands, or from deep bass to high air.

## BUILD 22

22 October 2026

Changed
- REACT is rebuilt a second time, on the model of the Continuator, OMax and Voyager. The other tracks improvise from a memory of what you played (its last 96 events: notes or chords, levels, lengths and gaps). Each one goes on as you went on, or jumps to where you played the same intervals and rhythm. It prefers what fits the harmony now. Their rhythm is yours, slowed or quickened.
- The ensemble regroups at your phrases, and every 6 to 9 seconds of unbroken playing. Each musician FOLLOWS, CONTRASTS, ECHOES, ANSWERS (continues your phrase in your rests) or RESTS, as suits its sound, each in its own register. Textures play slow, low, single notes. Long notes that stop fitting a new chord let go.

## BUILD 21

22 October 2026

Changed
- REACT is rebuilt. The beat grid, the arpeggios and the walking bass are gone. The other tracks only react to what you do: a note, a chord, a hold, a rest, an accent, a repeated phrase. They take their timing from your gaps, lengths and fastest notes. When you stop, they stop.
- Parts are dealt by sound. CALIGULA, RUSSOLO and drones get TEXTURE and never play note by note. Sustained sounds get PAD or SHADOW, plucked ones ANSWER, ORNAMENT or ECHO. The parts are TEXTURE, PAD, BASS, SHADOW, ECHO, ANSWER and ORNAMENT, and a second voice of a part is made unlike the first. Who joins each phrase is drawn from how intense your playing is and how long it has gone on.
- A held chord alone sets off no pattern. Silence brings silence.

## BUILD 20

22 October 2026

Added
- REACT: the other tracks answer the one you play. SHIFT + FADER BANK turns it on. Its pads are on the bottom row: tracks that are in are lit, and the guide's pad turns red when pressed. Held, the pads go up or down. REACT listens to everything the chosen track sounds (keys, latches, chords, MIDI in, sequencers, loops) for the chord, the pulse, the phrase, how busy and how hard. The tracks taken in play ORNAMENT, BASS, PAD, ECHO and TALK (call and response in your rests), in the scale. They grow more elaborate as you keep playing and let go after a silence.

## BUILD 19

22 October 2026

Fixed
- CALIGULA's tail ends at low notes. At C2 the feedback and delay settled on a faint residue that never fell far enough for the voice to end. It ran on silently, used processor time and kept its pad lit. The cut-off level is now higher (below -54 dB), and the voice ends.

## BUILD 18

22 October 2026

Added
- MIDI OUT on the 3.5 mm jack. Every message sent over USB goes out of the jack too, with or without a computer. Switch it in GLOBAL SETUP > MIDI > PORTS > TRS OUT. MIDI in works over USB only.
- A track that is not chosen but sounding has its pad half way between dim and full.

Changed
- The two rows of six buttons on the right are lit dim while idle, without flicker, and full when active.
- The instrument has its own USB product number, so computers read "SMK-37 Tonitor" afresh. You may need to reconnect it or reselect it in your software.

## BUILD 17

22 October 2026

Fixed
- A transposition keeps the notes' velocities. Latched notes remember the velocity they were struck at. Moved with SHIFT + OCT (with or without a track's pad), they sound at it again, not at full. Loops' sounding notes moved at once also take the velocity of their voices. Sessions keep the velocities. Sessions saved before this build are not affected.

## BUILD 16

22 October 2026

Added
- SHIFT held and a key pressed latches the note at once, and letting go of the key does not end it. A latched key pressed with SHIFT held lets it go. The FM-1's order (keys held, then SHIFT) still latches too.
- SHIFT tapped twice within 0.35 s lets every latched note go, on every track (LATCH OFF).

## BUILD 15

22 October 2026

Added
- FADER BANK opens the ENVELOPE page, up or down, with its A D S R on K3 to K6. It is lit while up.

Changed
- The track view's knobs are reordered: K1 level, K2 pan, K3 filter, K4 grain, K5 crush, K6 distortion, K7 delay send, K8 reverb send. Distortion has its own knob now. SHIFT + K5 no longer does it. The pages' four values stay on K3 to K6. With MACRO on, K1 to K8 are the eight macros, in knob order.

## BUILD 14

22 October 2026

Changed
- About 15 KB of memory freed (from 98168 to 83352 bytes used). No sound changes: the engines sound as before.
- The clipboard is now kept in the sessions' flash storage. A copy writes the track into it, and the audio may hush for a moment when a sector is erased. A paste reads it back. The clipboard is let go at power-on, and it is not in the backup.

## BUILD 13

22 October 2026

Added
- BRICKLAYER, with its button held, gets the loops' full options on pads 9 to 16, in violets: REVERSE, PENDULUM, RANDOM, STUTTER, CLIMB DOWN, CLIMB UP, HALF, DOUBLE. Its CLIMB moves it a step of the scale after each run of its steps. Its STUTTER wanders: the same step again, the one before, the next, or the one after. Sessions keep these options. Earlier sessions load with them off.
- PLAY held for 0.6 s shows the chosen track's loops on the bottom row of pads, in a soft red: half-lit while in, dim while out. A pad takes its loop out or brings it back. Held again, the pads are freed.
- Up to eight loops per track, a pad each. The ninth is refused with 8 LOOPS.

Changed
- The loops' STUTTER wanders. At the end of each eighth of the loop, the next one is drawn: the same again, the one before, the next (twice as likely) or the one after.

## BUILD 12

22 October 2026

Added
- The pitch wheel bends the chosen track by up to 2 semitones. Every engine bends, FM, ANIMA, CALIGULA, RUSSOLO, DRUID, BAYAREA and HIVE alike.
- The mod wheel adds vibrato (5.5 Hz, up to 0.4 semitone) and pushes macro 1 up (each engine's main tone control).
- BRICKLAYER held shows its direction (pads 9 to 11, violet) and speed (pads 15 and 16, a pinker violet) on the bottom row, like the loops'. Held again, the pads are freed. Held while off, it turns BRICKLAYER on with them. Pressed off, they go.

Changed
- While a sequencer plays (BALLSY or BRICKLAYER), PLAY blinks on every page and stops it from there. On the sequencer's own page it starts and stops it, as before.

## BUILD 11

22 October 2026

Added
- BALLSY (the 4th top-row button) and BRICKLAYER (the 5th). Pressed, the chosen track's sequencer is on (its page up) or off (stopped). Held while on, its page goes up or down. PLAY starts and stops it on its page, as on the FM-1. The button is lit while on and blinks while the sequencer plays. It is still one sequencer per track.
- With MACRO on, EDIT shows over the sound, in red.

Changed
- MACRO pressed now only turns the macros on or off. BALLSY has its own button.
- In GLOBAL SETUP, VELOCITY has the curves EVEN, SOFT, HARD and FIXED, and a FIXED row. Names and values sit side by side without overlapping.

## BUILD 10

22 October 2026

Added
- MACRO (the third top-row button). Pressed, the macros go on or off (lit while on). The FM-1's EDIT is behind it. With macros on, K1 to K8 turn the eight macros of the chosen track's sound, each with its dial. Macros 5 to 8 are new (1 to 4 as they were; at 0 every sound is as drawn):
  - FM: ATTACK, RELEASE, LFO RATE, VELOCITY (velocity on brightness; FM voices now know their note's velocity)
  - ANIMA: ATTACK, RELEASE, GRAIN SIZE, PHASER
  - CALIGULA: ATTACK, RELEASE, SPREAD, DELAY TIME
  - RUSSOLO: ATTACK, RELEASE, CHORD, HOLD
  - DRUID: EXCITATION, BOW, DUST, MUTE
  - BAYAREA: ATTACK, FM RATIO, SPRING, DRIVE
  - HIVE: RESONANCE, FILTER ENV, RING, SHIMMER
- MIDI CC 20 to 23 control macros 5 to 8. The modulation matrix gets MAC5 to MAC8 as destinations.

Changed
- Eight macros per sound instead of four. Earlier sessions load with their four macros. Saved presets are carried over to the new format at the first save. Older versions' presets still load.

## BUILD 9

22 October 2026

Added
- STUTTER (pad 12 of LOOP's pads): each loop repeats the eighth of it that it is in (the last eighth, to its end). What it sounds is let go each time. Turn it off and the loop goes on from there. Not in RANDOM.
- CLIMB DOWN (pad 13) and CLIMB UP (pad 14): each pass is a step of the scale lower or higher, round an octave of it. The two exclude each other.

Changed
- SCATTER and DRIFT, added in build 8, are gone. Earlier sessions load with the loops' options let go.

## BUILD 8

22 October 2026

Added
- Three options on LOOP's pads 12 to 14, each on or off (a magenta red, lit while on), combinable with direction and speed and kept by sessions:
  - SCATTER: each recorded pitch is played as another of the loop's own, drawn afresh each pass. The rhythm is kept.
  - DRIFT: each pass starts 1/50 of the loop later, so it slides out of phase with the other tracks' loops. Not in RANDOM.
  - CLIMB: each pass is a step of the scale higher, round an octave of it.

## BUILD 7

22 October 2026

Added
- LOOP (the second top-row button). Pressed, the chosen track's loop options show on the bottom row of pads (lit while up). Pressed again, the pads are freed.
- Pads 9 to 11 are REVERSE, PENDULUM and RANDOM (red; none lit means forward). Pads 15 and 16 are HALF and DOUBLE (a warmer red; none lit means normal). Pressing the lit pad again goes back. Direction and speed combine, as on the FM-1. BRICKLAYER has its own page. Pads 12 to 14 are dark, for later.

## BUILD 6

22 October 2026

Changed
- Chord mode and its shapes page are tied together. CHORD held with chord mode off turns the shapes up and chord mode on. CHORD pressed with chord mode off takes the shapes off the pads too. Choosing a track without chords closes them as well.

## BUILD 5

22 October 2026

Added
- CHORD (the first top-row button). Pressed, the chosen track's chord mode goes on or off (lit while on). Held, the chord shapes show on the bottom row of pads. Held again, the pads are freed.
- Pads 1 to 8 of the bottom row choose TRIAD, INV 1, INV 2, SUS2, SUS4, OPEN, FOURTHS and FIFTHS (new: stacked fifths of the scale). They are blue, the chosen one bright and the others dim.

## BUILD 4

22 October 2026

Added
- Knobs per track: K1 the track's level, K2 pan, K3 filter, K4 grain, K5 crush, K6 delay send, K7 reverb send. K8 is free. The pages' four values are on K3 to K6. SHIFT + K5 is the distortion.
- The master volume is on fader 4 (the rightmost).
- In GLOBAL SETUP, a VELOCITY section under KEYS: CURVE (LINEAR, LIGHT, HEAVY, FIXED), LOWEST and HIGHEST (the range it maps into) and FIXED AT.
- The looper records each note's velocity (in 16 steps). Played backwards, a note-off carries its note-on's velocity. A loop can last at most 12.7 minutes and closes by itself there.

Changed
- Pads 7 and 8 (the paths) are white now, unlike the tracks' colours.
- Earlier sessions' loops play at full velocity, as before.

## BUILD 3

22 October 2026

Added
- Pads 7 and 8 of the top row are the chosen track's AMBIENT and EXPERIMENTAL paths (blue and red). Lit while on, dim while off.
- The bottom row of buttons is lit while its page is up: SAVE (SESSIONS, GLOBAL SETUP), ARP (SCALE, PRESETS) and SEQ (its pages). HOME is lit while held.
- < and > (under the screen) do everything the FM-1's PRESETS knob did, a step a press, SHIFT combinations too. K4 has no job for now.

Changed
- The USB product name is SMK-37 Tonitor, and the installer says keyboard.

## BUILD 2

22 October 2026

Added
- All 37 keys (C3 to C6) play, with their velocity. With a scale, every key, black ones too, plays the next step from the root on the lowest key. CHROMATIC (new, last in the list) is the keyboard as printed.
- The six tracks on the first six pads of the top row: the chosen one in its colour, the others dim (blinking on the clipboard). The rest are dark. The pads play no notes.
- SHIFT is KNOB BANK, lit while held.

Changed
- A track's octave 0 is centred for the scale's length: 12 steps C3 to C6; 7 and 8 steps an octave lower; 6 steps one more; 5 steps two lower. OCT stops where a key would leave MIDI's range. Changing the scale or root brings the octaves back into range.
- The FM-1's black-key functions (paths, ENVELOPE, EDIT, CHORD, the loops' direction and speed) have no control yet. Their states are kept for the pads.
- Earlier sessions load with their latches let go, because latches now cover 37 keys.
