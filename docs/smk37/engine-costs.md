# What each engine costs

The SMK-37 renders all six tracks on one core, a block of 32 samples (0.73 ms) at a time. Every sounding
voice takes a share of that block. When the total passes **82 %**, the firmware protects the audio: it ends
the quietest fading tails first, then the quietest voices, never a latched note. Below 82 % nothing is cut.

The figures below were measured on the keyboard itself (BUILD 31, a diagnostic build that plays each engine
alone on track 1 and reports over USB). Use them to plan how much you can stack.

## The fixed part

| What | Load |
|---|---|
| Nothing playing | 14 % |
| One track sounding, with its delay and reverb sends | about 24 % |

That leaves about **58 %** for the voices before the protection starts.

## Each engine

*Voice* is the cost of one note held. *Notes* is about how many notes one track can hold on its own before
the protection starts (the instrument has 12 voices in all).

| Engine | Voice | Notes | Remarks |
|---|---|---|---|
| RUSSOLO | 2–2.5 % | any | one noise organism a track: more notes cost nothing more |
| FM | 5–6 % | 10–12 | the lightest of the voiced engines |
| ANIMA | 8.5–11 % | 5–6 | PLUCK lightest, FOG and BREATH heaviest |
| BAYAREA | 9.5–10 % | 6 | percussive sounds (CHIME, FOLDED, BONGO) end quickly and free their voices |
| DRUID, strings and bodies | 12–13 % | 4 | PLUCK, BOWED, STRUCK, SITAR, RATTLE, BELL, BOWL, PLATE |
| DRUID, sympathetic | 25 % | 2 | HARP and DRONE: a string with three sympathetic strings |
| HIVE | 9–22 % | 2–6 | AIR lightest; SPECTRA and PAD about 16 %; LEAD heaviest |
| CALIGULA | 17–23 % a track | any | one organism a track, whatever the number of notes |

## All six tracks

| Six tracks, each a different engine (FM, ANIMA, CALIGULA, HIVE, DRUID, BAYAREA) | Load |
|---|---|
| one note each | 81 % (peaks 93 %) |
| two notes each | 123 %: the protection thins the voices |
| six tracks of FM, two notes each, for comparison | 73 % |

## Track effects

Each track's own effects add to its cost while they are on (measured in BUILD 30, before the audio code was
moved to fast RAM, so they are now somewhat lower):

| Effect | Added |
|---|---|
| distortion | about 4 % |
| granulator | about 4.5 % |
| filter | about 1.5 % |
| crush | under 0.5 % |

The master delay and reverb are shared by all tracks; they cost nothing while everything is silent.

## Rules of thumb

- Heavy pairs: DRUID DRONE or HARP, HIVE LEAD and CALIGULA each take a quarter of the budget or close to it.
  Two or three of them sounding at once leave little for the other tracks.
- Long ambient layers are cheapest on FM and RUSSOLO; ANIMA and BAYAREA sit in the middle.
- Latched notes are never cut, so latching many heavy voices leaves the protection only the other tracks to
  thin.
