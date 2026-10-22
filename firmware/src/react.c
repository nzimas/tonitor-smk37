/* SPDX-License-Identifier: GPL-3.0-only
 * TONITOR: REACT, the other tracks answering the one played (SHIFT + FADER BANK: on or off, its pads up with it;
 * held 0.6 s, its pads up or down). The chosen track is the guide; on REACT's pads (the bottom row, 9 .. 14 the six
 * tracks) the others are taken in or out -- in lit, out dim, the guide's pad red a moment when pressed (it cannot
 * follow itself). Choosing another track makes it the guide.
 *
 * Nothing here keeps time of its own: no clock, no grid, no pattern. Every note the others play answers something
 * the guide did -- a note, a chord, a note held, a rest, an accent, a phrase played again -- and takes its timing
 * from the guide's own: its gaps, its lengths, its fastest notes. The guide stops, they stop.
 *
 * It listens to everything the guide sounds (its keys, latched notes, chords, MIDI in, its sequencers, its loops)
 * and keeps: the notes down and how long each is held; each pitch class's weight lately (the chord) and over a long
 * while (the tonal centre); the phrase now and the one before (notes, gaps, lengths, levels -- a phrase ends at a
 * rest); its usual gap and its fastest; how hard and how busy it plays (its intensity); where on the keyboard.
 *
 * Each track taken in is a musician with a part suited to its sound: a texture (CALIGULA, RUSSOLO, the drones and
 * washes), a sustained sound or a plucked one (by the family's name, else by the envelope). The parts:
 *   TEXTURE   the tonal centre held long, moved only when the centre moves; a colour struck at a climax
 *   PAD       the chord held, voice-led to the nearest notes when the guide's harmony moves, now and then a
 *             suspension resolving after one of the guide's own gaps; swells as the guide grows louder; at a rest it
 *             rings on a while, as long as the phrase asked, then lets go
 *   BASS      the root under the guide when its harmony moves, at a phrase's start, at an accent (a plucked bass also
 *             with each chord the guide strikes); an inversion when the guide plays a phrase again
 *   SHADOW    the guide's notes that matter (the long, the accented, a phrase's first) shadowed a third or a sixth
 *             below, or in contrary motion; held as long as the guide holds its note
 *   ECHO      the phrase imitated: a note or a few behind (riding on the guide's own rhythm), or after one of its
 *             gaps; a third away, a sixth, the octave, onto the chord, upside down
 *   ANSWER    in the guide's rest, its phrase answered in its own rhythm: imitated onto the chord, a fragment
 *             backwards or upside down, or -- the guide repeating itself -- carried on as a sequence. Two answering
 *             take turns, the other now and then answering the answer. The guide plays on: the answer stops.
 *   ORNAMENT  a note the guide holds, decorated once: a turn, an appoggiatura, a mordent, a trill, a run up into
 *             it, a bell of colour above -- at the speed of the guide's fastest notes, never the same twice running
 * A guide's climax (its highest, harder) brings them in together. The parts are dealt by sound: the best suited
 * each to the part it suits best; more tracks than parts, a second voice of a part, made unlike the first.
 *
 * Who joins a phrase is drawn afresh at each phrase, from how intensely the guide plays and how long it has been
 * playing: soft and sparse, the harmony and the answers; harder and longer, the shadow, the echoes, the ornaments.
 * Its notes go as a sequencer's (the looper does not record them), one note each. It runs in the main loop. */

#define RX_PH 12                 /* a phrase, notes */
#define RX_Q 32                  /* notes to come and sounding */
#define RX_HD 10                 /* the guide's notes down */
enum { RC_PLUCK, RC_SUST, RC_TEX };
enum { RR_TEX, RR_PAD, RR_ANS, RR_BASS, RR_SHAD, RR_ORN, RR_ECHO, RR_N };
#define RR_NONE 0xFFu
static const char *const RR_NAME[RR_N] = {"TEXTURE", "PAD", "ANSWER", "BASS", "SHADOW", "ORNAMENT", "ECHO"};
/* how well a sound suits a part (0: never), by its kind: plucked, sustained, texture */
static const uint8_t RR_AFF[3][RR_N] = {{0, 0, 8, 4, 5, 8, 6}, {0, 8, 4, 5, 7, 0, 5}, {10, 6, 0, 3, 0, 0, 0}};
enum { TR_THIRD_UP, TR_THIRD_DN, TR_SIXTH_DN, TR_OCT, TR_CHORD, TR_INV, TR_N };
enum { G_TURN, G_APPOG, G_MORD, G_TRILL, G_SLIDE, G_BELL, G_N };
#define QF_FOLLOW 1u             /* (a note let go when the guide lets go its note src) */
#define QF_ECHO 2u               /* (its length the guide's note's, once known) */
#define QF_LONG 4u               /* (held till moved: a PAD's, a BASS's, a TEXTURE's) */

/* the families by name: textures, plucked, low */
static const char *const RX_TEXN[] = {"DRONE", "HAZE", "FOG", "WASH", "TIDE", "TIDES", "WIND", "STATIC", "SPECTRE",
                                      "SPECTRA", "BREATH", "DUST", "MURMUR", "SWARM", "DRIFT", "HOWL", "AIR", "CLUSTER",
                                      "SWELL", "ABYSS", "FRICTION", "BRUIT", "WARP", "GRIT"};
static const char *const RX_PLKN[] = {"PLUCK", "MALLET", "IMPACT", "BONGO", "STRUCK", "HARP", "SITAR", "CHIME", "BELL",
                                      "BELLS", "STAB", "GLASS", "SHARDS", "RATTLE", "PLATE", "RUBBER", "GLITCH", "BITS",
                                      "ITERATIF", "METAL", "MACHINE"};
static const char *const RX_BASSN[] = {"SUB", "GROUND", "ABYSS", "UNDERTOW"};

typedef struct { uint8_t n, note[RX_PH], vel[RX_PH]; uint16_t ioi[RX_PH], dur[RX_PH]; } rx_ph_t;
typedef struct {
    uint8_t role, chr, var, bassy;
    uint8_t en;                  /* (joining this phrase) */
    uint8_t mode;                /* SHADOW: 0 parallel, 1 contrary; ECHO: notes behind (0: after a gap) */
    uint8_t tr, last_tr, last_g; /* ECHO's transformation, the one before; ORNAMENT's last gesture */
    uint8_t cur, keep, lvl;      /* the last note; TEXTURE's note; the level its long notes were struck at */
    uint8_t done;                /* ECHO: the phrase's notes echoed so far */
    int8_t sh;                   /* (this phrase's octave shift, 0x7F: not yet) */
    uint32_t swell_t;
} rx_mus_t;

static struct {
    uint8_t on, mask, guide;
    struct { uint8_t note, vel, ix; uint32_t t; } hd[RX_HD];
    uint8_t nhd;
    uint16_t w[12], wl[12];      /* each pitch class lately; over a long while */
    rx_ph_t ph, pv;              /* the phrase now, the one before */
    uint8_t rep;                 /* (this phrase repeats the one before) */
    uint8_t ans_done, orn_done, ans_turn, chord_n, breath;
    uint32_t last_on, last_ev, typ, fast, climax_t, chord_due, ctr_t;
    uint8_t vel, E, lo, hi, mid, top;
    int8_t deg, inv;             /* the chord's root (a step of the scale; -1 none); the bass on its third or fifth */
    uint16_t grow;
    rx_mus_t m[NLAYER];
    uint32_t red_until, rt0;
    uint8_t rdown, rdone;
    struct { uint8_t layer, note, vel, on, src, fl; uint32_t at, t0, len; } q[RX_Q];   /* on: 1 to strike, 2 sounding */
    rnd_t r;
} rx;

static void env_native(uint32_t l, int8_t *v);
static int32_t cur_kind(uint32_t l);
static const char *kind_name(uint32_t e, int32_t k);

static uint32_t rx_rnd(void)
{
    if (!rx.r.s)
        rnd_seed(&rx.r, fm1_ms ^ 0x5EAC7u);
    return rnd_next(&rx.r);
}
static int rx_odds(int32_t pct) { return (int32_t)((rx_rnd() >> 8) % 100u) < pct; }
static uint32_t rx_in(uint32_t lo, uint32_t hi) { return lo + (rx_rnd() >> 8) % (hi - lo + 1u); }
static uint32_t rx_cx(void) { return rx.grow / 220u; }   /* 0 .. 4: how much the parts dare */
static uint32_t rx_v(uint32_t ref, uint32_t pct) { return (uint32_t)clampi((int32_t)(ref * pct / 100u), 10, 122); }
static uint32_t rx_rest_ms(void) { return (uint32_t)clampi((int32_t)rx.typ * 2, 350, 2500); }   /* a phrase's end */
static uint32_t rx_ans_ms(void) { return (uint32_t)clampi((int32_t)rx.typ * 3 / 2, 300, 2000); }   /* .. the answer's turn */
static uint32_t rx_hold_ms(void) { return (uint32_t)clampi((int32_t)rx.typ * 3 / 2, 280, 1500); }   /* a note held */
static int rx_role(uint32_t l)
{
    if (l >= NLAYER || l == rx.guide || !((rx.mask >> l) & 1u) || rx.m[l].role == RR_NONE)
        return -1;
    return rx.m[l].role;
}

/* ---- the queue: a note of track l, struck in wait ms, held len ms */
static int rx_play(uint32_t l, uint32_t note, uint32_t vel, uint32_t len, uint32_t wait, uint32_t src, uint32_t fl)
{
    uint32_t i, slot = RX_Q;
    if (note > 127u || l == rx.guide)
        return -1;
    for (i = 0; i < RX_Q; i++)
        if (!rx.q[i].on) {
            slot = i;
            break;
        }
    if (slot == RX_Q) {                             /* (all taken: the soonest to end let go, its place taken) */
        slot = 0;
        for (i = 1; i < RX_Q; i++)
            if (rx.q[i].on == 2u && (rx.q[slot].on != 2u || (int32_t)(rx.q[i].at - rx.q[slot].at) < 0))
                slot = i;
        if (rx.q[slot].on == 2u)
            seq_press(rx.q[slot].layer, rx.q[slot].note, 0, 0);
    }
    rx.q[slot].layer = (uint8_t)l;
    rx.q[slot].note = (uint8_t)note;
    rx.q[slot].vel = (uint8_t)clampi((int32_t)vel, 1, 126);
    rx.q[slot].len = len < 30u ? 30u : len;
    rx.q[slot].at = fm1_ms + wait;
    rx.q[slot].src = (uint8_t)src;
    rx.q[slot].fl = (uint8_t)fl;
    rx.q[slot].on = 1;
    rx.m[l].cur = (uint8_t)note;
    return (int)slot;
}
static void rx_kill(uint32_t i)
{
    if (rx.q[i].on == 2u)
        seq_press(rx.q[i].layer, rx.q[i].note, 0, 0);
    rx.q[i].on = 0;
}
static void rx_cut(uint32_t l)                      /* track l's notes (all if l is NLAYER) let go, none to come */
{
    uint32_t i;
    for (i = 0; i < RX_Q; i++)
        if (rx.q[i].on && (l >= NLAYER || rx.q[i].layer == l))
            rx_kill(i);
}
static void rx_cut_long(uint32_t l)                 /* .. only its held ones */
{
    uint32_t i;
    for (i = 0; i < RX_Q; i++)
        if (rx.q[i].on && rx.q[i].layer == l && (rx.q[i].fl & QF_LONG))
            rx_kill(i);
}
static int rx_sounding(uint32_t l)                  /* track l holds a long note */
{
    uint32_t i;
    for (i = 0; i < RX_Q; i++)
        if (rx.q[i].on && rx.q[i].layer == l && (rx.q[i].fl & QF_LONG))
            return 1;
    return 0;
}
static void rx_let_ring(uint32_t l, uint32_t ms)    /* track l's held notes: let go in ms */
{
    uint32_t i;
    for (i = 0; i < RX_Q; i++)
        if (rx.q[i].on == 2u && rx.q[i].layer == l && (rx.q[i].fl & QF_LONG) && (int32_t)(rx.q[i].at - (fm1_ms + ms)) > 0)
            rx.q[i].at = fm1_ms + ms;
}

/* ---- the scale and the chord */
static uint32_t rx_sp(int32_t i)                    /* the scale's step i: its pitch class */
{
    int32_t n = SCALE_N[ui.scale];
    return (ui.root + SCALE_STEP[ui.scale][((i % n) + n) % n]) % 12u;
}
static int rx_chrom(void) { return SCALE_N[ui.scale] >= 12u; }
static uint32_t rx_tpc(uint32_t k)                  /* the chord's tone k (root, third, fifth, seventh, ninth) */
{
    static const uint8_t SEMI[5] = {0, 4, 7, 10, 14};
    if (rx.deg < 0)
        return ui.root;
    if (rx_chrom())
        return (rx_sp(rx.deg) + SEMI[k % 5u]) % 12u;
    return rx_sp(rx.deg + 2 * (int32_t)(k % 5u));
}
static uint32_t rx_cmask(uint32_t nt)
{
    uint32_t k, m = 0;
    for (k = 0; k < nt; k++)
        m |= 1u << rx_tpc(k);
    return m;
}
static uint32_t rx_near(uint32_t pc, int32_t near)   /* pitch class pc nearest note near (-6 .. +5) */
{
    int32_t d = (((int32_t)pc - near % 12 + 18) % 12) - 6;
    return (uint32_t)clampi(near + d, 0, 127);
}
static uint32_t rx_snap(int32_t n)                  /* the nearest scale note (up first) */
{
    uint32_t m = scale_mask();
    n = clampi(n, 0, 127);
    if ((m >> (n % 12)) & 1u)
        return (uint32_t)n;
    if ((m >> ((n + 1) % 12)) & 1u)
        return (uint32_t)clampi(n + 1, 0, 127);
    return (uint32_t)clampi(n - 1, 0, 127);
}
static uint32_t rx_next(uint32_t n, int32_t d, uint32_t nt)   /* the next chord note from n, up or down */
{
    uint32_t m = rx_cmask(nt);
    int32_t t = (int32_t)n;
    do
        t += d;
    while (t > 0 && t < 127 && !((m >> (t % 12)) & 1u));
    return (uint32_t)clampi(t, 0, 127);
}
static uint32_t rx_step(uint32_t n, int32_t s)      /* s steps of the scale from n */
{
    return rx_chrom() ? (uint32_t)clampi((int32_t)n + s, 0, 127) : lx_map(n, s, scale_mask());
}
/* the chord from the notes down and lately (the lowest down counting most): 1 if it changed */
static int rx_chord(void)
{
    uint32_t hm = 0, k, lp = 99u, w[12], bs = 0;
    int32_t n = SCALE_N[ui.scale], i, best = -1;
    for (k = 0; k < rx.nhd; k++) {
        hm |= 1u << (rx.hd[k].note % 12u);
        if (lp == 99u || rx.hd[k].note < lp)
            lp = rx.hd[k].note;
    }
    if (lp != 99u)
        lp %= 12u;
    for (k = 0; k < 12u; k++)
        w[k] = rx.w[k] + (((hm >> k) & 1u) ? 3000u : 0u) + (k == lp ? 2000u : 0u);
    for (i = 0; i < n; i++) {
        uint32_t r = rx_sp(i), t = rx_chrom() ? (r + 4u) % 12u : rx_sp(i + 2), f = rx_chrom() ? (r + 7u) % 12u : rx_sp(i + 4);
        uint32_t s = w[r] * 4u + w[t] * 2u + w[f] * 2u;
        if (i == rx.deg)
            s += s / 6u;                            /* (the chord there stays unless another fits clearly better) */
        if (s > bs) {
            bs = s;
            best = i;
        }
    }
    if (best < 0 || best == rx.deg)
        return 0;
    rx.deg = (int8_t)best;
    return 1;
}

/* ---- the musicians: their kind, their parts, their registers */
static int rx_listed(const char *s, const char *const *t, uint32_t n)
{
    while (n--)
        if (str_eq(s, t[n]))
            return 1;
    return 0;
}
static void rx_class(uint32_t l)
{
    rx_mus_t *m = &rx.m[l];
    uint32_t e = ui.engine[l], k;
    const char *nm = kind_name(e, cur_kind(l));
    int own = ui.env_on[l][e] != 0;
    int8_t v[4];
    int32_t att, dec;
    if (own)
        for (k = 0; k < 4u; k++)
            v[k] = ui.env[l][e][k];
    else
        env_native(l, v);
    att = env_ms(0, v[0]);
    dec = env_ms(1, v[1]);
    m->bassy = (uint8_t)rx_listed(nm, RX_BASSN, sizeof RX_BASSN / sizeof RX_BASSN[0]);
    if (eng_organism(e) || (!own && rx_listed(nm, RX_TEXN, sizeof RX_TEXN / sizeof RX_TEXN[0])))
        m->chr = RC_TEX;                            /* (CALIGULA, RUSSOLO: organisms, never note by note) */
    else if (!own && rx_listed(nm, RX_PLKN, sizeof RX_PLKN / sizeof RX_PLKN[0]))
        m->chr = RC_PLUCK;
    else
        m->chr = v[2] < 40 && att < 120 && dec < 1500 ? RC_PLUCK : RC_SUST;
}
static uint32_t rx_aff(uint32_t l, uint32_t r)
{
    uint32_t a = RR_AFF[rx.m[l].chr][r];
    return a && r == RR_BASS && rx.m[l].bassy ? a + 6u : a;
}
/* the parts dealt: the best suited pair first (a tie to the part listed first), each part once; the musicians left,
 * a second voice of the part each suits best (never a second bass) */
static void rx_deal(void)
{
    static const uint8_t PRI[RR_N] = {RR_TEX, RR_PAD, RR_ANS, RR_BASS, RR_SHAD, RR_ORN, RR_ECHO};
    uint32_t l, k, took = 0, used = 0;
    for (l = 0; l < NLAYER; l++) {
        rx.m[l].role = RR_NONE;
        rx.m[l].var = 0;
        if (l != rx.guide && ((rx.mask >> l) & 1u))
            rx_class(l);
    }
    for (;;) {
        uint32_t bl = NLAYER, br = 0, ba = 3;
        for (k = 0; k < RR_N; k++) {
            uint32_t r = PRI[k];
            if ((used >> r) & 1u)
                continue;
            for (l = 0; l < NLAYER; l++)
                if (l != rx.guide && ((rx.mask >> l) & 1u) && !((took >> l) & 1u) && rx_aff(l, r) > ba) {
                    ba = rx_aff(l, r);
                    bl = l;
                    br = r;
                }
        }
        if (bl == NLAYER)
            break;
        rx.m[bl].role = (uint8_t)br;
        took |= 1u << bl;
        used |= 1u << br;
    }
    for (l = 0; l < NLAYER; l++)
        if (l != rx.guide && ((rx.mask >> l) & 1u) && !((took >> l) & 1u)) {
            uint32_t r, br = RR_NONE, ba = 0;
            for (r = 0; r < RR_N; r++)
                if (r != RR_BASS && rx_aff(l, r) > ba) {
                    ba = rx_aff(l, r);
                    br = r;
                }
            rx.m[l].role = (uint8_t)br;
            rx.m[l].var = 1;
        }
    for (l = 0; l < NLAYER; l++) {
        rx.m[l].keep = 0;
        rx.m[l].sh = 0x7F;
    }
}
/* the middle of track l's register, by its part (the guide's range moves it) */
static int32_t rx_centre(uint32_t l)
{
    const rx_mus_t *m = &rx.m[l];
    int32_t lo = rx.lo, hi = rx.hi, mid = rx.mid, up = mid > 66 ? -12 : 12;
    switch (m->role) {
    case RR_BASS:
        return clampi(lo - 12, 30, 52);
    case RR_PAD:
        return clampi(mid - 5 + (m->var ? 9 : 0), 48, 78);
    case RR_TEX:
        return m->var ? clampi(hi + 7, 60, 96) : clampi(lo - 7, 33, 60);
    case RR_ORN:
        return m->var ? clampi(mid - 12, 40, 72) : clampi(hi + 9, 62, 100);
    case RR_ECHO:
    case RR_ANS:
        return clampi(mid + (m->var ? -up : up), 36, 96);
    default:
        return mid;
    }
}
/* a phrase's octave shift for track l: chosen at its first note, so its line keeps its shape */
static int32_t rx_shift(uint32_t l, int32_t first)
{
    rx_mus_t *m = &rx.m[l];
    if (m->sh == 0x7F) {
        int32_t c = rx_centre(l), s = 0;
        while (first + s > c + 6)
            s -= 12;
        while (first + s < c - 6)
            s += 12;
        m->sh = (int8_t)clampi(s, -48, 48);
    }
    return m->sh;
}
static uint32_t rx_lvl(uint32_t l, uint32_t ref)    /* softer than the guide: projection, not loudness */
{
    static const uint8_t PCT[RR_N] = {50, 48, 72, 82, 62, 58, 60};
    return rx_v(ref, PCT[rx.m[l].role] - (rx.m[l].var ? 8u : 0u));
}

/* ---- the parts */
static void rx_bass(uint32_t l, uint32_t ref)       /* the root (or an inversion) under the guide */
{
    rx_mus_t *m = &rx.m[l];
    uint32_t n = rx_near(rx_tpc(rx.inv), rx_centre(l));
    if (rx.nhd && n >= rx.lo)
        n -= 12u;
    rx_cut(l);
    rx_play(l, n, rx_lvl(l, ref), m->chr == RC_PLUCK ? rx.typ * 2u : 60000u, rx_in(0, 12), 0, QF_LONG);
}
static void rx_pad(uint32_t l)                      /* the chord, voice-led from what it held */
{
    rx_mus_t *m = &rx.m[l];
    uint32_t cx = rx_cx(), nt = cx >= 2u ? 4u : 3u, k, last = 0, sus = cx >= 1u && !rx_chrom() && rx_odds(25);
    int32_t c = rx_centre(l), spread = m->var ? 7 : 4;
    rx_cut_long(l);
    for (k = 0; k < nt; k++) {
        uint32_t t = (k == 0u && cx >= 3u && rx_odds(50)) ? 4u : k;   /* (the root left to the bass: a ninth) */
        uint32_t n = rx_near(rx_tpc(t), c - spread + spread * (int32_t)k), v = rx_lvl(l, rx.vel);
        if (n == last)
            continue;
        if (t == 1u && sus) {                       /* a suspension: the fourth, falling to the third after a gap */
            uint32_t d = (uint32_t)clampi((int32_t)rx.typ, 250, 1200);
            rx_play(l, rx_step(n, 1), v, d, k * 23u, 0, QF_LONG);
            rx_play(l, n, v, 60000u, k * 23u + d, 0, QF_LONG);
        } else
            rx_play(l, n, v, 60000u, k * 23u, 0, QF_LONG);
        last = n;
    }
    m->lvl = rx.E;
}
static void rx_tex(uint32_t l, int strike)          /* the tonal centre, held; moved only when it moves */
{
    rx_mus_t *m = &rx.m[l];
    uint32_t k, c = 0;
    for (k = 1; k < 12u; k++)
        if (rx.wl[k] > rx.wl[c])
            c = k;
    if (!rx.wl[c])
        return;
    if (m->var)                                     /* (a second texture: the centre's fifth, above) */
        c = rx_chrom() ? (c + 7u) % 12u : rx_step(c + 60u, 4) % 12u;
    if (m->keep && m->keep % 12u != c && rx.wl[m->keep % 12u] * 4u > rx.wl[c] * 3u && !strike)
        return;                                     /* (the centre has not clearly moved) */
    if (m->keep && m->keep % 12u == c && !strike && rx_sounding(l))
        return;
    rx_cut_long(l);
    m->keep = (uint8_t)rx_near(c, rx_centre(l));
    rx_play(l, m->keep, rx_lvl(l, rx.vel), 60000u, 0, 0, QF_LONG);
    m->lvl = rx.E;
    m->swell_t = fm1_ms;
}
static int rx_struct(uint32_t ioi, uint32_t vel)    /* a note that matters: long-spaced, accented, or a phrase's first */
{
    return rx.ph.n <= 1u || ioi >= rx.typ * 2u / 3u || vel >= rx.vel + 8u;
}
static void rx_shadow(uint32_t l, uint32_t note, uint32_t vel, int32_t dir)
{
    rx_mus_t *m = &rx.m[l];
    uint32_t n;
    if (m->mode == 1u && dir && m->cur && m->cur < note) {   /* contrary motion: the next chord note the other way */
        n = rx_next(m->cur, dir > 0 ? -1 : 1, 4);
        if (n + 2u >= note || n + 16u < note)
            n = rx_step(note, -2);
    } else {
        n = rx_step(note, m->var ? -5 : -2);        /* a third below (a second voice: a sixth) */
        if (rx_cx() >= 2u && rx_odds(30))
            n = rx_next(note, -1, 4);               /* (grown: the chord's note below, whatever it is) */
    }
    rx_play(l, n, rx_lvl(l, vel), m->chr == RC_PLUCK ? rx.typ : 60000u, rx_in(5, 25), note, QF_FOLLOW);
}
static uint32_t rx_tr(uint32_t l, uint32_t n)       /* ECHO's transformation of a note of the phrase */
{
    rx_mus_t *m = &rx.m[l];
    int32_t f = rx.ph.n ? rx.ph.note[0] : (int32_t)n, t;
    switch (m->tr) {
    case TR_THIRD_UP:
        t = (int32_t)rx_step(n, 2);
        break;
    case TR_THIRD_DN:
        t = (int32_t)rx_step(n, -2);
        break;
    case TR_SIXTH_DN:
        t = (int32_t)rx_step(n, -5);
        break;
    case TR_CHORD:
        t = (int32_t)rx_near(rx_tpc(rx_in(0, 2)), (int32_t)n);
        break;
    case TR_INV:
        t = (int32_t)rx_snap(2 * f - (int32_t)n);
        break;
    default:
        t = (int32_t)n;
        break;
    }
    return (uint32_t)clampi(t + rx_shift(l, f), 24, 108);
}
static void rx_echo(uint32_t l, uint32_t note, uint32_t vel)   /* the guide's note just played */
{
    rx_mus_t *m = &rx.m[l];
    if (m->mode) {                                  /* some notes behind: the one that many back, sounding now */
        int32_t i = (int32_t)rx.ph.n - 1 - (int32_t)m->mode;
        if (i >= 0) {
            uint32_t d = rx.ph.dur[i] ? rx.ph.dur[i] : rx.typ * 9u / 10u;
            rx_play(l, rx_tr(l, rx.ph.note[i]), rx_lvl(l, rx.ph.vel[i]), d, rx_in(0, 15), 0, 0);
            m->done = (uint8_t)(i + 1);
        }
    } else                                          /* after one of the guide's gaps (two, a second voice three) */
        rx_play(l, rx_tr(l, note), rx_lvl(l, vel), rx.typ * 9u / 10u, rx.typ * (2u + m->var), note, QF_ECHO);
}
static void rx_echo_rest(uint32_t l)                /* the phrase over: the notes still behind, in its own rhythm */
{
    rx_mus_t *m = &rx.m[l];
    uint32_t i, t = (uint32_t)clampi((int32_t)rx.typ / 2, 80, 600);
    if (!m->mode)
        return;
    for (i = m->done; i < rx.ph.n; i++) {
        uint32_t d = rx.ph.dur[i] ? rx.ph.dur[i] : rx.typ * 9u / 10u;
        if (i > m->done)
            t += (uint32_t)clampi(rx.ph.ioi[i], 70, 1500);
        rx_play(l, rx_tr(l, rx.ph.note[i]), rx_lvl(l, rx.ph.vel[i]), d, t, 0, 0);
    }
    m->done = rx.ph.n;
}
/* ANSWER: the phrase just ended, answered in its own rhythm. form 0: imitated onto the chord; 1: its end
 * backwards; 2: its end upside down; 3: carried on as a sequence (the guide repeating itself) */
static uint32_t rx_answer(uint32_t l, uint32_t form, uint32_t start)
{
    const rx_ph_t *p = &rx.ph;
    uint32_t n = p->n, k = form == 0u || form == 3u ? n : (n < 4u ? n : rx_in(3, 4)), s = n - k, i, t = start, ix;
    int32_t first = p->note[form == 1u ? n - 1u : s], d = 0, seq = 0, aug = rx.typ < 230u ? 3 : 2;
    rx.m[l].sh = 0x7F;
    if (form == 0u)
        d = (int32_t)rx_near(rx_tpc(rx_in(0, 2)), first) - first;
    if (form == 3u)
        seq = rx_odds(50) ? 1 : -1;
    for (i = 0; i < k; i++) {
        uint32_t gap, dur;
        int32_t nt;
        ix = form == 1u ? n - 1u - i : s + i;
        nt = p->note[ix];
        if (form == 2u)
            nt = 2 * first - nt;
        else if (form == 3u)
            nt = (int32_t)rx_step((uint32_t)nt, seq * 2 * (i >= k / 2u ? 2 : 1));
        nt = (int32_t)rx_snap(nt + d);
        gap = i + 1u < k ? p->ioi[form == 1u ? ix : ix + 1u] : 0u;
        gap = (uint32_t)clampi((int32_t)gap * aug / 2, 70, 1500);
        dur = p->dur[ix] ? (uint32_t)clampi(p->dur[ix] * aug / 2, 50, 2000) : gap * 4u / 5u;
        rx_play(l, (uint32_t)clampi(nt + rx_shift(l, first + d), 24, 108), rx_lvl(l, p->vel[ix]), dur, t, 0, 0);
        if (t > start + 5000u)
            break;
        t += gap;
    }
    return t;
}
static void rx_ornament(uint32_t l, uint32_t note) /* a held note decorated, once */
{
    rx_mus_t *m = &rx.m[l];
    uint32_t g, sp = (uint32_t)clampi((int32_t)rx.fast * 2 / 3, 60, 180), v = rx_lvl(l, rx.vel) + 16u, t = 0, i, k;
    int32_t c = rx_centre(l), n = (int32_t)rx_near(note % 12u, c);
    do
        g = rx_in(0, rx_cx() >= 1u ? G_N - 1u : G_SLIDE);
    while (g == m->last_g);
    m->last_g = (uint8_t)g;
    switch (g) {
    case G_TURN:                                    /* above, the note, below, the note */
        rx_play(l, rx_step((uint32_t)n, 1), v, sp, t, 0, 0);
        rx_play(l, (uint32_t)n, v - 8u, sp, t += sp, 0, 0);
        rx_play(l, rx_step((uint32_t)n, -1), v - 8u, sp, t += sp, 0, 0);
        rx_play(l, (uint32_t)n, v, sp * 3u, t + sp, 0, 0);
        break;
    case G_APPOG:                                   /* the step above, leaning, falling to the note */
        rx_play(l, rx_step((uint32_t)n, 1), v + 6u, sp * 3u, 0, 0, 0);
        rx_play(l, (uint32_t)n, v - 10u, sp * 4u, sp * 3u, 0, 0);
        break;
    case G_MORD:
        rx_play(l, (uint32_t)n, v, sp / 2u + 20u, 0, 0, 0);
        rx_play(l, rx_step((uint32_t)n, 1), v - 10u, sp / 2u + 20u, sp / 2u + 20u, 0, 0);
        rx_play(l, (uint32_t)n, v, sp * 3u, sp + 40u, 0, 0);
        break;
    case G_TRILL:                                   /* slowing as it goes */
        k = rx_in(4, 7);
        for (i = 0; i < k; i++) {
            uint32_t d = sp / 2u + i * sp / 6u;
            rx_play(l, i & 1u ? rx_step((uint32_t)n, 1) : (uint32_t)n, v - (i & 1u) * 8u, d, t, 0, 0);
            t += d;
        }
        break;
    case G_SLIDE:                                   /* a run up the scale into it */
        k = rx_in(2, 4);
        for (i = 0; i < k; i++)
            rx_play(l, rx_step((uint32_t)n, (int32_t)i - (int32_t)k), v - 14u + i * 4u, sp, i * sp * 3u / 4u, 0, 0);
        rx_play(l, (uint32_t)n, v, sp * 3u, k * sp * 3u / 4u, 0, 0);
        break;
    default:                                        /* a bell of colour above: the chord's ninth or sixth */
        rx_play(l, rx_near(rx_odds(50) ? rx_tpc(4) : rx_sp(rx.deg + 5), c + 5), v - 6u, rx.typ * 3u, 0, 0, 0);
        break;
    }
}

/* ---- what each musician does when the guide does something */
static void rx_phrase_new(void)                     /* a phrase begins: who joins it, and how */
{
    uint32_t l, cx = rx_cx();
    if (rx.ph.n >= 2u)
        rx.pv = rx.ph;
    rx.ph.n = 0;
    rx.rep = 0;
    rx.inv = 0;
    for (l = 0; l < NLAYER; l++) {
        rx_mus_t *m = &rx.m[l];
        int r = rx_role(l);
        int32_t p = 100;
        if (r < 0)
            continue;
        m->sh = 0x7F;
        m->done = 0;
        if (r == RR_SHAD) {
            p = 25 + rx.E / 3 + (int32_t)cx * 8;
            m->mode = (uint8_t)(cx >= 1u && rx_odds(40));
        } else if (r == RR_ECHO) {
            p = 20 + rx.E / 4 + (int32_t)cx * 10 - (m->en ? 20 : 0);   /* (seldom two phrases running) */
            m->mode = (uint8_t)(rx_odds(60) ? rx_in(1, cx >= 2u ? 3u : 2u) : 0u);
            do
                m->tr = (uint8_t)rx_in(0, cx >= 2u ? TR_N - 1u : cx ? TR_CHORD : TR_OCT);
            while (m->tr == m->last_tr);
            m->last_tr = m->tr;
        } else if (r == RR_ORN)
            p = 40 + (int32_t)cx * 12 + (rx.E < 50u ? 10 : 0);
        m->en = (uint8_t)rx_odds(clampi(p, 0, 95));
    }
}
static void rx_on_chord(void)                       /* the guide's harmony moved: the bass, the pads */
{
    uint32_t l;
    for (l = 0; l < NLAYER; l++) {
        int r = rx_role(l);
        if (r == RR_BASS)
            rx_bass(l, rx.vel);
        else if (r == RR_PAD)
            rx_pad(l);
    }
}
static void rx_climax(void)                         /* the guide's highest, harder: all in together */
{
    uint32_t l;
    rx.climax_t = fm1_ms;
    for (l = 0; l < NLAYER; l++)
        switch (rx_role(l)) {
        case RR_BASS:
            rx_bass(l, 120);
            break;
        case RR_PAD:
            rx_pad(l);
            rx_play(l, rx_near(rx_tpc(rx_in(1, 4)), rx_centre(l) + 12), rx_lvl(l, 120), 60000u, 30, 0, QF_LONG);
            break;
        case RR_TEX:
            rx_play(l, rx_near(rx_tpc(4), rx_centre(l) + 19), rx_lvl(l, 90), rx.typ * 6u, 0, 0, 0);
            break;
        case RR_SHAD:
            rx_play(l, rx.ph.n ? (uint32_t)rx.ph.note[rx.ph.n - 1u] - 12u : 60u, rx_lvl(l, 120), rx.typ * 2u, 10, 0, 0);
            break;
        default:
            break;
        }
}

/* the guide's note, struck (vel) or let go */
static void rx_off(uint32_t note)
{
    uint32_t now = fm1_ms, i, k;
    for (k = 0; k < rx.nhd; k++)
        if (rx.hd[k].note == note) {
            uint32_t d = now - rx.hd[k].t, ix = rx.hd[k].ix;
            if (ix < rx.ph.n && !rx.ph.dur[ix])
                rx.ph.dur[ix] = (uint16_t)(d > 8000u ? 8000u : d);
            for (i = 0; i < RX_Q; i++) {
                if (!rx.q[i].on || rx.q[i].src != note)
                    continue;
                if (rx.q[i].fl & QF_FOLLOW) {        /* (the shadow lets go with it) */
                    if (rx.q[i].on == 2u)
                        rx.q[i].at = now;
                    else
                        rx.q[i].on = 0;
                } else if (rx.q[i].fl & QF_ECHO) {   /* (the echo as long as it was held) */
                    if (rx.q[i].on == 1u)
                        rx.q[i].len = d;
                    else
                        rx.q[i].at = rx.q[i].t0 + d;
                    rx.q[i].src = 0xFF;
                }
            }
            rx.hd[k] = rx.hd[--rx.nhd];
            break;
        }
    rx.last_ev = now;
}
static void rx_hear(uint32_t note, uint32_t vel, int on)
{
    uint32_t now = fm1_ms, ioi, gap, l, pc = note % 12u, chordal, fresh = 0;
    int32_t dir = 0;
    if (!rx.on || note > 127u)
        return;
    if (!on) {
        rx_off(note);
        return;
    }
    vel = vel ? vel : 100u;
    ioi = rx.last_on ? now - rx.last_on : 100000u;
    gap = rx.last_ev ? now - rx.last_ev : 100000u;
    chordal = ioi < 45u;
    rx.w[pc] = (uint16_t)(rx.w[pc] + 300u > 4000u ? 4000u : rx.w[pc] + 300u);
    rx.wl[pc] = (uint16_t)(rx.wl[pc] + 60u > 12000u ? 12000u : rx.wl[pc] + 60u);
    rx.mid = (uint8_t)((rx.mid * 7u + note) / 8u);
    rx.lo = (uint8_t)(note < rx.lo ? note : rx.lo + (rx.lo < rx.mid && rx_odds(20)));
    rx.hi = (uint8_t)(note > rx.hi ? note : rx.hi - (rx.hi > rx.mid && rx_odds(20)));
    if (rx.nhd < RX_HD) {
        rx.hd[rx.nhd].note = (uint8_t)note;
        rx.hd[rx.nhd].vel = (uint8_t)vel;
        rx.hd[rx.nhd].t = now;
        rx.hd[rx.nhd].ix = 0xFF;
        rx.nhd++;
    }
    if (!chordal) {
        if (ioi >= 60u && ioi < 3000u && ioi < rx.typ * 3u) {   /* its usual gap, its fastest */
            rx.typ = (uint32_t)clampi((int32_t)rx.typ + ((int32_t)ioi - (int32_t)rx.typ) / 5, 90, 2000);
            rx.fast = ioi < rx.fast ? ioi : rx.fast + (ioi - rx.fast) / 16u;
        }
        if (!rx.ph.n || (rx.nhd == 1u && gap > rx_rest_ms())) {
            rx_phrase_new();                        /* (after a rest: a new phrase) */
            fresh = 1;
        }
        if (rx.ph.n >= RX_PH) {
            uint32_t k;
            for (k = 0; k + 1u < RX_PH; k++) {
                rx.ph.note[k] = rx.ph.note[k + 1u];
                rx.ph.vel[k] = rx.ph.vel[k + 1u];
                rx.ph.ioi[k] = rx.ph.ioi[k + 1u];
                rx.ph.dur[k] = rx.ph.dur[k + 1u];
            }
            rx.ph.n = RX_PH - 1u;
            for (k = 0; k < rx.nhd; k++)
                if (rx.hd[k].ix != 0xFFu)
                    rx.hd[k].ix = rx.hd[k].ix ? (uint8_t)(rx.hd[k].ix - 1u) : 0xFFu;
            for (k = 0; k < NLAYER; k++)
                if (rx.m[k].done)
                    rx.m[k].done--;
        }
        if (rx.ph.n)
            dir = (int32_t)note - (int32_t)rx.ph.note[rx.ph.n - 1u];
        rx.ph.note[rx.ph.n] = (uint8_t)note;
        rx.ph.vel[rx.ph.n] = (uint8_t)vel;
        rx.ph.ioi[rx.ph.n] = (uint16_t)(fresh ? 0u : ioi > 8000u ? 8000u : ioi);
        rx.ph.dur[rx.ph.n] = 0;
        rx.hd[rx.nhd - 1u].ix = rx.ph.n;
        rx.ph.n++;
        rx.E = (uint8_t)clampi(((int32_t)rx.E * 5 + (int32_t)vel + (ioi < rx.typ ? 25 : 0)) / 6, 0, 127);
        rx.vel = (uint8_t)((rx.vel * 3u + vel) / 4u);
        rx.grow = (uint16_t)(rx.grow + 3u > 1000u ? 1000u : rx.grow + 3u);
        rx.last_on = now;
        rx.chord_n = 1;
        rx.orn_done = rx.ans_done = rx.breath = 0;
        if (rx.ph.n == 3u && rx.pv.n >= 3u &&       /* the guide playing a phrase again: the parts vary */
            (rx.ph.note[1] - rx.ph.note[0]) == (rx.pv.note[1] - rx.pv.note[0]) &&
            (rx.ph.note[2] - rx.ph.note[1]) == (rx.pv.note[2] - rx.pv.note[1])) {
            rx.rep = 1;
            if (rx_cx() >= 1u && rx_odds(45))
                rx.inv = (int8_t)rx_in(1, 2);       /* (the bass on the third or the fifth) */
        }
        if (rx_chord())
            rx.chord_due = now + 40u;               /* (a chord comes a note at a time: settled first) */
        for (l = 0; l < NLAYER; l++) {
            rx_mus_t *m = &rx.m[l];
            switch (rx_role(l)) {
            case RR_ANS:
                rx_cut(l);                          /* (the guide plays on: the answer stops) */
                break;
            case RR_SHAD:
                if (m->en && rx_struct(ioi, vel))
                    rx_shadow(l, note, vel, dir);
                break;
            case RR_ECHO:
                if (m->en && !(rx.E > 100u && rx_odds(40)))
                    rx_echo(l, note, vel);
                break;
            case RR_BASS:
                if (rx.deg >= 0 && !rx.chord_due && (fresh || vel >= rx.vel + 20u || !rx_sounding(l)))
                    rx_bass(l, vel);
                break;
            case RR_TEX:
                if (!rx_sounding(l))
                    rx_tex(l, 0);                    /* (in from the guide's first notes) */
                break;
            case RR_PAD:
                if (rx.deg >= 0 && !rx.chord_due && (!rx_sounding(l) ||
                    (m->chr == RC_SUST && (rx.E > m->lvl + 30u || rx.E + 30u < m->lvl))))
                    rx_pad(l);                       /* (back after a rest; or the guide much louder or softer) */
                break;
            default:
                break;
            }
        }
        if (rx_cx() >= 1u && rx.ph.n >= 2u && note >= rx.top && vel >= rx.vel + 15u && now - rx.climax_t > 6000u)
            rx_climax();
        if (note > rx.top)
            rx.top = (uint8_t)note;
    } else {
        if (rx.ph.n && note > rx.ph.note[rx.ph.n - 1u]) {
            rx.ph.note[rx.ph.n - 1u] = (uint8_t)note;   /* (a chord: its top note, the tune's) */
            rx.hd[rx.nhd - 1u].ix = (uint8_t)(rx.ph.n - 1u);
        }
        if (++rx.chord_n == 2u)                     /* the guide struck a chord: a plucked bass with it */
            for (l = 0; l < NLAYER; l++)
                if (rx_role(l) == RR_BASS && rx.m[l].chr == RC_PLUCK)
                    rx.chord_due = now + 40u;
        if (rx_chord())
            rx.chord_due = now + 40u;
    }
    rx.last_ev = now;
}

static void rx_reset(void)                          /* nothing heard yet (on, or a new guide) */
{
    rx_cut(NLAYER);
    memset(rx.hd, 0, sizeof rx.hd);
    rx.nhd = 0;
    memset(rx.w, 0, sizeof rx.w);
    memset(rx.wl, 0, sizeof rx.wl);
    rx.ph.n = rx.pv.n = 0;
    rx.ans_done = rx.orn_done = rx.breath = 1;
    rx.last_on = rx.last_ev = rx.chord_due = 0;
    rx.typ = 500;
    rx.fast = 250;
    rx.vel = 96;
    rx.E = 60;
    rx.lo = rx.hi = rx.mid = rx.top = 60;
    rx.deg = -1;
    rx.inv = 0;
    rx.guide = ui.sel;
    rx_ear = rx.on ? ui.sel : 0xFFu;
    rx_deal();
}
static void rx_set(int on)
{
    rx.on = (uint8_t)on;
    rx_reset();
    rx.grow = 0;
}

/* the main loop, every pass: the guide's loops heard; its holds, rests and silences answered; the notes due */
static void rx_tick(void)
{
    uint32_t now = fm1_ms, i, l, gap;
    if (rx.on && rx.guide != ui.sel)
        rx_reset();                                 /* (another track chosen: it leads now) */
    while (rx_lr != rx_lw) {                        /* the guide's loops' notes (the ISR's) */
        uint32_t n = rx_lq[rx_lr % 16u][0], v = rx_lq[rx_lr % 16u][1];
        RING_PUBLISH();
        rx_lr++;
        rx_hear(n, v, v != 0u);
    }
    if (rx.on && rx.last_on) {
        gap = now - rx.last_ev;
        if (rx.chord_due && (int32_t)(now - rx.chord_due) >= 0) {
            rx.chord_due = 0;
            rx_on_chord();
        }
        if (now - rx.ctr_t > 900u) {                /* (now and then: the sounds may have changed; the centre) */
            uint8_t chr[NLAYER];
            int ch = 0;
            rx.ctr_t = now;
            for (l = 0; l < NLAYER; l++)
                chr[l] = rx.m[l].chr;
            for (l = 0; l < NLAYER; l++)
                if (rx_role(l) >= 0) {
                    rx_class(l);
                    ch |= chr[l] != rx.m[l].chr;
                }
            if (ch) {
                rx_cut(NLAYER);
                rx_deal();
            }
            for (l = 0; l < NLAYER; l++)
                if (rx_role(l) == RR_TEX && (rx.nhd || now - rx.last_on < rx_rest_ms()))
                    rx_tex(l, rx.E > rx.m[l].lvl + 35u && now - rx.m[l].swell_t > 3000u);
        }
        if (rx.nhd && !rx.orn_done && now - rx.last_on > rx_hold_ms()) {   /* a note held: decorated */
            uint32_t top = 0;
            rx.orn_done = 1;
            for (i = 0; i < rx.nhd; i++)
                if (rx.hd[i].note > top)
                    top = rx.hd[i].note;
            for (l = 0; l < NLAYER; l++)
                if (rx_role(l) == RR_ORN && rx.m[l].en && rx_odds(70))
                    rx_ornament(l, top);
            for (l = 0; l < NLAYER; l++)            /* (grown: a long hold coloured -- the pad's ninth) */
                if (rx_role(l) == RR_PAD && rx_cx() >= 2u && rx_odds(35))
                    rx_play(l, rx_near(rx_tpc(4), rx_centre(l) + 7), rx_lvl(l, rx.vel), 60000u, 0, 0, QF_LONG);
        }
        if (!rx.nhd && !rx.ans_done && rx.ph.n >= 2u && gap > rx_ans_ms()) {   /* a rest: answered */
            uint32_t na = 0, first = NLAYER, second = NLAYER, end, phl = 0;
            rx.ans_done = 1;
            for (i = 1; i < rx.ph.n; i++)
                phl += rx.ph.ioi[i];
            for (l = 0; l < NLAYER; l++) {
                int r = rx_role(l);
                if (r == RR_ECHO && rx.m[l].en)
                    rx_echo_rest(l);
                else if (r == RR_ANS)
                    na++;
                else if ((r == RR_PAD || r == RR_BASS) && !rx.breath)
                    rx_let_ring(l, (uint32_t)clampi((int32_t)phl / 2, 600, 4000));   /* (ringing as long as the phrase asked) */
            }
            rx.breath = 1;
            if (na && rx_odds(rx.ph.n < 3u ? 40 : 60 + (int32_t)rx_cx() * 10)) {
                uint32_t turn = rx.ans_turn++ % na, j = 0;
                for (l = 0; l < NLAYER; l++)
                    if (rx_role(l) == RR_ANS) {
                        if (j == turn)
                            first = l;
                        else if (second == NLAYER)
                            second = l;
                        j++;
                    }
                {
                    uint32_t form = rx.rep ? 3u : rx.ph.n >= 5u && rx_odds(60) ? rx_in(1, rx_cx() ? 2u : 1u) : 0u;
                    end = rx_answer(first, form, (uint32_t)clampi((int32_t)rx.typ / 4, 60, 400));
                }
                if (second != NLAYER && rx_cx() >= 1u && rx_odds(40))
                    rx_answer(second, rx_in(1, 2), end + (uint32_t)clampi((int32_t)rx.typ / 2, 100, 600));   /* (the answer answered) */
            }
        }
        if (!rx.nhd && gap > (rx.typ * 10u > 8000u ? rx.typ * 10u : 8000u)) {   /* silence: all let go, waiting */
            rx_cut(NLAYER);
            rx.last_on = 0;
            rx.deg = -1;
            rx.ph.n = 0;
            rx.grow = (uint16_t)(rx.grow / 2u);
            rx.E = 60;
            memset(rx.w, 0, sizeof rx.w);
            for (l = 0; l < NLAYER; l++)
                rx.m[l].keep = 0;
        }
    }
    for (i = 0; i < RX_Q; i++) {
        if (rx.q[i].on == 1u && (int32_t)(now - rx.q[i].at) >= 0) {
            uint32_t pv = press_vel;
            press_vel = rx.q[i].vel;
            seq_press(rx.q[i].layer, rx.q[i].note, 0, 1);
            press_vel = (uint8_t)pv;
            rx.q[i].on = 2;
            rx.q[i].t0 = now;
            rx.q[i].at = now + rx.q[i].len;
        } else if (rx.q[i].on == 2u && (int32_t)(now - rx.q[i].at) >= 0) {
            seq_press(rx.q[i].layer, rx.q[i].note, 0, 0);
            rx.q[i].on = 0;
        }
    }
}

/* REACT's pads (the bottom row, 9 .. 14): track j in or out (said with its part); the guide's, red a moment */
static void rx_pad_hit(uint32_t j)
{
    static char m[24];
    int r;
    if (j >= NLAYER)
        return;
    if (j == ui.sel) {
        rx.red_until = fm1_ms + 1200u;
        say("GUIDE TRACK", 1200);
        return;
    }
    rx.mask = (uint8_t)(rx.mask ^ (1u << j));
    rx_cut(NLAYER);                                 /* (the parts dealt again) */
    rx_deal();
    r = rx_role(j);
    m[0] = (char)('1' + j);
    m[1] = ' ';
    str_cpy(m + 2, !((rx.mask >> j) & 1u) ? "OUT" : r >= 0 ? RR_NAME[r] : "IN", sizeof m - 2u);
    say(m, 1200);
}
