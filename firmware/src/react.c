/* SPDX-License-Identifier: GPL-3.0-only
 * TONITOR: REACT, the other tracks improvising with the one played (SHIFT + FADER BANK: on or off, its pads up with
 * it; held 0.6 s, its pads up or down). The chosen track is the guide; on REACT's pads (the bottom row, 9 .. 14 the six
 * tracks) the others are taken in or out -- in lit, out dim, the guide's pad red a moment when pressed (it cannot
 * follow itself). Choosing another track makes it the guide.
 *
 * After the systems that do this well -- Pachet's Continuator, IRCAM's OMax and Somax, Lewis's Voyager:
 *
 * MEMORY. Everything the guide plays is kept, in order, as its own vocabulary: each event (a note, or a chord struck
 * together) with its level, its length, and the gap before it -- its steps of the scale, so it can be moved along
 * the scale and keep its shape (the last RX_H events). A gap longer than a breath begins a phrase.
 *
 * MUSICIANS. Each track taken in improvises from that memory, as a factor oracle is navigated: from the event it
 * played last, it goes on as the guide once went on (continuity), or jumps to another place in the memory where the
 * guide played the same intervals (the longest same context first, the same rhythm too; then shorter, looser), and
 * goes on from there. So its lines are the guide's own, recombined: new phrases in the guide's style, never a pattern
 * of its own. Its rhythm is the guide's -- the gaps and lengths of the events it plays, as played -- slowed or
 * quickened by its own factor. Each choice is weighed by how it fits the harmony now: what the guide holds, and the
 * pitch classes it played lately (fading over a second or two); its notes moved along the scale (none, a third up
 * or down) to fit. When the guide's harmony moves, a musician's long note that no longer fits lets go.
 *
 * THE ENSEMBLE. At a phrase of the guide's, after some seconds, the musicians regroup: each takes a way to play,
 * suited to its sound -- a texture (CALIGULA, RUSSOLO, the drones and washes: only slow, low, single notes), a
 * sustained sound (the guide's chords too), a plucked one -- and a register of its own, an octave or two from the
 * guide's:
 *   FOLLOWS    with the guide, as fast or half as fast, its harmony followed closely; quick to react to a new chord
 *   CONTRASTS  against it: slow and long under a busy guide, moving under one holding long chords; the other side of
 *              the keyboard, softer
 *   ECHOES     the guide's line a few events behind, as the guide plays them (so in its rhythm), a third or a sixth
 *              or an octave away; at a rest, the rest of it
 *   ANSWERS    in the guide's rest: its phrase continued as the guide might have (the continuation of its last
 *              notes, found in the memory), until the guide plays again
 *   RESTS      sits out this time
 * The guide's intensity sets how much each plays; its silence silences them (ANSWERS apart, in the rest).
 *
 * Its page (up with it; SHIFT + FADER BANK held, up or down): K1 and K2 stay the track's level and pan; K3 .. K6
 *   DENSITY     how much they play: how often a musician sounds a step, how long it waits (from twice to half
 *               the guide's gaps), how many sit out
 *   COMPLEXITY  the structure: low, single notes in plain rhythms (the guide's gaps rounded to halves and doubles of
 *               its usual one); high, harmonised notes (a third or a sixth under), notes moved further along the
 *               scale, steps split in two or three, notes pushed off the beat, paces in threes against twos
 *   CONTINUITY  low, they jump about the guide's memory, recombining it; high, they keep to its phrases as played
 *   SPREAD      how far from the guide's register: low, around its hands; high, from deep bass to high air
 * Its notes go as a sequencer's (the looper does not record them), never on the guide. It runs in the main loop. */

#define RX_H 96                  /* the memory: the guide's last events */
#define RX_Q 32                  /* notes to come and sounding */
#define RX_HD 10                 /* the guide's notes down */
#define RX_NONE 0xFFFFFFFFu
enum { RC_PLUCK, RC_SUST, RC_TEX };
enum { RM_REST, RM_FOLLOW, RM_CONTRA, RM_ECHO, RM_ANSWER, RM_N };
static const char *const RM_NAME[RM_N] = {"RESTS", "FOLLOWS", "CONTRASTS", "ECHOES", "ANSWERS"};

/* the families by name: textures, plucked, low */
static const char *const RX_TEXN[] = {"DRONE", "HAZE", "FOG", "WASH", "TIDE", "TIDES", "WIND", "STATIC", "SPECTRE",
                                      "SPECTRA", "BREATH", "DUST", "MURMUR", "SWARM", "DRIFT", "HOWL", "AIR", "CLUSTER",
                                      "SWELL", "ABYSS", "FRICTION", "BRUIT", "WARP", "GRIT"};
static const char *const RX_PLKN[] = {"PLUCK", "MALLET", "IMPACT", "BONGO", "STRUCK", "HARP", "SITAR", "CHIME", "BELL",
                                      "BELLS", "STAB", "GLASS", "SHARDS", "RATTLE", "PLATE", "RUBBER", "GLITCH", "BITS",
                                      "ITERATIF", "METAL", "MACHINE"};
static const char *const RX_BASSN[] = {"SUB", "GROUND", "ABYSS", "UNDERTOW"};

typedef struct {
    uint8_t n[3], nn;            /* its notes, low to high (a chord: up to three) */
    uint8_t vel, start, dg;      /* its level; a phrase's first; its top note's step of the scale (from MIDI 0) */
    uint16_t ioi, dur;           /* the gap before it, how long it was held (0: not yet let go), ms */
} rx_item_t;
typedef struct {
    uint8_t chr, bassy, mode;
    uint8_t tf;                  /* its pace: x tf / 4 the guide's times (2 half .. 16 four times as long) */
    uint8_t cont, pplay, pct;    /* how often it goes on as the guide went on; plays a step; its level, % */
    uint8_t lag;                 /* ECHOES: events behind */
    int8_t oct, tx, sh;          /* its register (octaves from the guide's); its steps moved; this line's shift */
    uint32_t pos;                /* the event it played last (from the first ever kept), RX_NONE: none */
    uint32_t next_at, ans_end, echoed;
    uint32_t taboo[3];
} rx_ag_t;

static struct {
    uint8_t on, mask, guide;
    struct { uint8_t note; uint32_t t, it; } hd[RX_HD];
    uint8_t nhd;
    uint16_t ch[12];             /* each pitch class lately (fading) */
    rx_item_t h[RX_H];
    uint32_t hn;                 /* events kept so far (the last RX_H in h) */
    uint32_t last_on, last_ev, typ, form_t, decay_t, ph0;
    uint8_t vel, mid, ans_done, turn;
    rx_ag_t a[NLAYER];
    uint32_t red_until, rt0;
    uint8_t rdown, rdone;
    int8_t k[4];                 /* the page's DENSITY COMPLEXITY CONTINUITY SPREAD, 0 .. 100 */
    struct { uint8_t layer, note, vel, on; uint32_t at, len; } q[RX_Q];   /* on: 1 to strike, 2 sounding */
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
static uint32_t rx_rest_ms(void) { return (uint32_t)clampi((int32_t)rx.typ * 2, 350, 2500); }   /* a breath */
static uint32_t rx_ans_ms(void) { return (uint32_t)clampi((int32_t)rx.typ * 3 / 2, 300, 2000); }
#define RX_DENS rx.k[0]
#define RX_CPLX rx.k[1]
#define RX_CONT rx.k[2]
#define RX_SPRD rx.k[3]
static const char *const RX_KNAME[4] = {"DENSITY", "COMPLEXITY", "CONTINUITY", "SPREAD"};
static int rx_in_ens(uint32_t l) { return l < NLAYER && l != rx.guide && ((rx.mask >> l) & 1u); }
static int rx_role(uint32_t l) { return rx_in_ens(l) ? rx.a[l].mode : -1; }   /* (the host tests) */

/* ---- the queue: a note of track l, struck in wait ms, held len ms */
static void rx_play(uint32_t l, uint32_t note, uint32_t vel, uint32_t len, uint32_t wait)
{
    uint32_t i, slot = RX_Q;
    if (note > 127u || !rx_in_ens(l))
        return;
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
    rx.q[slot].vel = (uint8_t)clampi((int32_t)vel, 8, 124);
    rx.q[slot].len = len < 40u ? 40u : len;
    rx.q[slot].at = fm1_ms + wait;
    rx.q[slot].on = 1;
}
static void rx_cut(uint32_t l)                      /* track l's notes (all if l is NLAYER) let go, none to come */
{
    uint32_t i;
    for (i = 0; i < RX_Q; i++)
        if (rx.q[i].on && (l >= NLAYER || rx.q[i].layer == l)) {
            if (rx.q[i].on == 2u)
                seq_press(rx.q[i].layer, rx.q[i].note, 0, 0);
            rx.q[i].on = 0;
        }
}

/* ---- the scale: steps of it counted from MIDI note 0, and back */
static uint32_t rx_n(void) { return SCALE_N[ui.scale]; }
static uint32_t rx_pcs(uint8_t *pc)                 /* the scale's pitch classes, low to high from C */
{
    uint32_t m = scale_mask(), k, n = 0;
    for (k = 0; k < 12u; k++)
        if ((m >> k) & 1u)
            pc[n++] = (uint8_t)k;
    return n;
}
static uint32_t rx_dg(uint32_t note)                /* note's step (a note off the scale: the step below it) */
{
    uint8_t pc[12];
    uint32_t n = rx_pcs(pc), k = 0;
    while (k + 1u < n && pc[k + 1u] <= note % 12u)
        k++;
    return (note / 12u) * n + k;
}
static uint32_t rx_note(int32_t dg)
{
    uint8_t pc[12];
    int32_t n = (int32_t)rx_pcs(pc);
    dg = clampi(dg, 0, 10 * n);
    return (uint32_t)clampi(dg / n * 12 + pc[dg % n], 0, 127);
}
/* how well pitch class pc fits the harmony now, 0 .. 256: what the guide holds counts most, then what it played lately */
static uint32_t rx_fit(uint32_t pc)
{
    uint32_t k, w[12], mx = 1;
    for (k = 0; k < 12u; k++)
        w[k] = rx.ch[k];
    for (k = 0; k < rx.nhd; k++)
        w[rx.hd[k].note % 12u] += 3000u;
    for (k = 0; k < 12u; k++)
        if (w[k] > mx)
            mx = w[k];
    return w[pc % 12u] * 256u / mx;
}

/* ---- the memory */
static rx_item_t *rx_it(uint32_t i) { return &rx.h[i % RX_H]; }
static int rx_kept(uint32_t i) { return i != RX_NONE && i < rx.hn && rx.hn - i <= RX_H; }
static int32_t rx_ival(uint32_t i)                  /* the step from the event before (99: a phrase's first) */
{
    if (!rx_kept(i) || !rx_kept(i - 1u) || rx_it(i)->start)
        return 99;
    return clampi((int32_t)rx_it(i)->dg - (int32_t)rx_it(i - 1u)->dg, -12, 12);
}
static uint32_t rx_dc(uint32_t i)                   /* its gap: short, usual, long */
{
    uint32_t g = rx_it(i)->ioi;
    return g * 10u < rx.typ * 7u ? 0u : g * 2u < rx.typ * 3u ? 1u : 2u;
}
/* do the events before and at i and j match, k back: lvl 0 the steps and the gaps, 1 the steps, 2 the direction */
static int rx_same(uint32_t i, uint32_t j, uint32_t k, uint32_t lvl)
{
    uint32_t m;
    for (m = 0; m < k; m++) {
        int32_t a, b;
        if (!rx_kept(i - m) || !rx_kept(j - m))
            return 0;
        a = rx_ival(i - m);
        b = rx_ival(j - m);
        if (lvl == 2u ? (a > 0) - (a < 0) != (b > 0) - (b < 0) || (a == 99) != (b == 99) : a != b)
            return 0;
        if (lvl == 0u && rx_dc(i - m) != rx_dc(j - m))
            return 0;
    }
    return 1;
}
/* event e's notes for musician l, moved tx steps and sh semitones: how many (into n) */
static uint32_t rx_render(uint32_t l, uint32_t e, int32_t tx, int32_t sh, uint32_t *n)
{
    const rx_item_t *it = rx_it(e);
    const rx_ag_t *a = &rx.a[l];
    uint32_t k, c = 0, lo = 0, hi = it->nn;
    if (a->chr == RC_TEX)                           /* a texture: the lowest note only; plucked: the top (now and then two) */
        hi = 1;
    else if (RX_CPLX < 35)                          /* (plain: one note, the top) */
        lo = it->nn - 1u;
    else if (a->chr == RC_PLUCK)
        lo = it->nn > 1u && rx_odds(75) ? it->nn - 1u : it->nn > 2u ? it->nn - 2u : 0u;
    for (k = lo; k < hi; k++)
        n[c++] = (uint32_t)clampi((int32_t)rx_note((int32_t)rx_dg(it->n[k]) + tx) + sh, 24, 108);
    if (a->chr != RC_TEX && c == 1u && RX_CPLX > 55 && rx_odds(RX_CPLX - (a->chr == RC_SUST ? 35 : 55)))
        n[c++] = (uint32_t)clampi((int32_t)rx_note((int32_t)rx_dg(it->n[hi - 1u]) + tx - (rx_odds(70) ? 2 : 5)) + sh, 24, 108);
    return c;                                       /* (rich: harmonised, a third or a sixth under) */
}
static uint32_t rx_fits(uint32_t l, uint32_t e, int32_t tx)   /* how well event e fits, moved tx steps */
{
    uint32_t n[3], c = rx_render(l, e, tx, 0, n), k, f = 0;
    for (k = 0; k < c; k++)
        f += rx_fit(n[k] % 12u);
    return c ? f / c : 0u;
}
/* the shift that puts event e (moved tx) in musician l's register */
static int32_t rx_reg(uint32_t l, uint32_t e, int32_t tx)
{
    int32_t c = clampi((int32_t)rx.mid + 12 * rx.a[l].oct, 30, 100), t = (int32_t)rx_note((int32_t)rx_dg(rx_it(e)->n[rx_it(e)->nn - 1u]) + tx), s = 0;
    if (rx.a[l].chr == RC_TEX || rx.a[l].bassy)
        c = clampi(c, RX_SPRD > 60 ? 26 : 30, 60);
    c = clampi(c, RX_SPRD > 60 ? 26 : 30, RX_SPRD > 60 ? 104 : 96);
    while (t + s > c + 6)
        s -= 12;
    while (t + s < c - 6)
        s += 12;
    return s;
}

/* ---- the musicians: what each is (by its sound) */
static int rx_listed(const char *s, const char *const *t, uint32_t n)
{
    while (n--)
        if (str_eq(s, t[n]))
            return 1;
    return 0;
}
static void rx_class(uint32_t l)
{
    rx_ag_t *a = &rx.a[l];
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
    a->bassy = (uint8_t)rx_listed(nm, RX_BASSN, sizeof RX_BASSN / sizeof RX_BASSN[0]);
    if (eng_organism(e) || (!own && rx_listed(nm, RX_TEXN, sizeof RX_TEXN / sizeof RX_TEXN[0])))
        a->chr = RC_TEX;                            /* (CALIGULA, RUSSOLO: organisms, never note by note) */
    else if (!own && rx_listed(nm, RX_PLKN, sizeof RX_PLKN / sizeof RX_PLKN[0]))
        a->chr = RC_PLUCK;
    else
        a->chr = v[2] < 40 && att < 120 && dec < 1500 ? RC_PLUCK : RC_SUST;
}
/* the ensemble regrouped: each musician a way to play (suited to its sound, unlike the others' where it can be), a
 * pace, a register */
static void rx_form(void)
{
    static const uint8_t W[3][RM_N] = {          /* (RESTS FOLLOWS CONTRASTS ECHOES ANSWERS, by plucked, sustained, texture) */
        {12, 22, 20, 20, 26}, {12, 34, 26, 18, 10}, {14, 50, 36, 0, 0}};
    uint32_t l, used = 0, nact = 0, nin = 0, octs = 0, nrest = 0, nall = 0;
    rx.form_t = fm1_ms + rx_in(0, 3000);         /* (the next, some seconds on: not on a clock) */
    for (l = 0; l < NLAYER; l++)
        nall += (uint32_t)rx_in_ens(l);
    for (l = 0; l < NLAYER; l++) {
        rx_ag_t *a = &rx.a[l];
        uint32_t m, tot = 0, d, was;
        uint8_t w[RM_N];
        if (!rx_in_ens(l))
            continue;
        nin++;
        rx_class(l);
        was = a->mode;
        for (m = 0; m < RM_N; m++) {
            w[m] = W[a->chr][m];
            if (m == RM_REST)                       /* (sparser, more sit out: one at most at the middle, none dense) */
                w[m] = (uint8_t)(nall < 3u || RX_DENS >= 80 || (nrest && RX_DENS >= 35) ? 0 : w[m] * (uint32_t)(160 - RX_DENS) / 100u);
            if (m == RM_ECHO || m == RM_ANSWER)     /* (more structure: more counterpoint) */
                w[m] = (uint8_t)(w[m] * (uint32_t)(50 + RX_CPLX) / 90u);
            if (m == RM_ANSWER && rx.typ > 900u)
                w[m] = (uint8_t)(w[m] / 4u);        /* (a guide holding long leaves few rests to answer) */
            if ((used >> m) & 1u && m != RM_REST)
                w[m] = (uint8_t)(w[m] / 3u);        /* (another plays so already: less likely) */
            if (m == was && m != RM_REST)
                w[m] = (uint8_t)(w[m] / 2u);        /* (and a change is welcome) */
            tot += w[m];
        }
        d = rx_in(0, tot - 1u);
        for (m = 0; m < RM_N - 1u && d >= w[m]; m++)
            d -= w[m];
        a->mode = (uint8_t)m;
        used |= 1u << m;
        nrest += m == RM_REST;
        nact += m != RM_REST;
        if (m != was)
            a->pos = RX_NONE;
        a->tx = 0;
        a->cont = 70;
        a->pplay = 85;
        if (RX_CPLX > 65 && rx_odds(RX_CPLX - 40))   /* (structure: a pace in threes against the guide's twos) */
            a->tf = 3;
        switch (m) {
        case RM_FOLLOW:
            a->tf = (uint8_t)(a->chr == RC_TEX ? 4u * rx_in(2, 4) : a->tf == 3u ? (rx_odds(50) ? 3u : 6u) : rx_odds(60) ? 4u : 8u);
            a->pct = 70;
            break;
        case RM_CONTRA:                         /* slow under a busy guide; moving under one holding long */
            a->tf = (uint8_t)(rx.typ < 350u ? rx_in(12, 16) : rx.typ > 900u ? (a->chr == RC_TEX ? 8u : rx_in(2, 4))
                              : rx_odds(50) ? 8u : RX_CPLX > 50 ? 3u : 2u);
            a->cont = 50;
            a->pplay = 70;
            a->pct = 58;
            break;
        case RM_ECHO: {
            static const int8_t TX[4] = {2, -2, -5, 0};
            a->tx = TX[rx_in(0, 3)];
            a->lag = (uint8_t)rx_in(1, 3);
            a->pct = 62;
            break;
        }
        case RM_ANSWER:
            a->tf = (uint8_t)(rx_odds(70) ? 4u : 6u);
            a->cont = 85;
            a->pplay = 95;
            a->pct = 76;
            break;
        default:
            break;
        }
        {   /* its register, as SPREAD has it -- around the guide's hands, an octave or two off, or up to three -- low
             * for a texture or a bass, the other side for a contrast, unlike the others' */
            static const int8_t O[3][4] = {{0, -1, 1, 0}, {-1, 1, -2, 2}, {-2, 2, -3, 3}};
            const int8_t *o = O[RX_SPRD < 20 ? 0 : RX_SPRD < 60 ? 1 : 2];
            uint32_t k = rx_in(0, 3), t;
            int down = a->chr == RC_TEX || a->bassy || (m == RM_CONTRA && rx.mid > 64u), up = m == RM_CONTRA && !down;
            for (t = 0; t < 8u; t++, k = (k + 1u) % 4u)
                if ((!down || o[k] <= 0) && (!up || o[k] >= 0) && !((octs >> (o[k] + 3)) & 1u))
                    break;
            a->oct = o[k];
            octs |= 1u << (o[k] + 3);
        }
        a->cont = (uint8_t)clampi(a->cont + (RX_CONT - 60) * 4 / 5, 5, 98);   /* (CONTINUITY: kept to the phrases, or not) */
        a->next_at = fm1_ms + rx_in(0, 200);
    }
    if (nin && !nact)                               /* (never all resting) */
        for (l = 0; l < NLAYER; l++)
            if (rx_in_ens(l)) {
                rx.a[l].mode = RM_FOLLOW;
                rx.a[l].tf = 4;
                rx.a[l].pct = 70;
                break;
            }
}

/* ---- a musician's next event: on as the guide went on, or a jump to where it played the same; fitted to the harmony */
static uint32_t rx_choose(uint32_t l, int32_t *tx)
{
    rx_ag_t *a = &rx.a[l];
    uint32_t cand[12], w[12], nc = 0, i, k, lvl, tot = 0, d, lo = rx.hn > RX_H ? rx.hn - RX_H : 0u, hi = rx.hn - 1u, on = 0;
    static const int8_t TXS[5] = {0, 2, -2, 4, -3};
    uint32_t ntx = RX_CPLX < 35 ? 1u : RX_CPLX > 65 ? 5u : 3u;   /* (moved along the scale: plain, none; rich, further) */
    if (a->mode != RM_ANSWER)
        hi = rx.hn > 2u ? rx.hn - 2u : 0u;          /* (not the guide's very last: that is an echo, not a line) */
    if (rx_kept(a->pos) && a->pos + 1u <= hi) {     /* on as it went on */
        cand[nc++] = a->pos + 1u;
        on = 1;
    }
    if (rx_kept(a->pos))                            /* .. or where the guide played the same: the longest same first */
        for (lvl = 0; lvl < 5u && nc < 2u; lvl++) {
            static const uint8_t K[5] = {3, 2, 2, 1, 1}, L[5] = {0, 0, 1, 1, 2};
            for (i = lo + K[lvl]; i < hi && nc < 12u; i++)
                if (i != a->pos && i + 1u != a->pos + 1u && rx_same(i, a->pos, K[lvl], L[lvl]))
                    cand[nc++] = i + 1u;
        }
    while (nc < 4u && hi >= lo) {                   /* (nothing like it: anywhere, a phrase's first preferred) */
        uint32_t j = lo + rx_in(0, hi - lo);
        for (k = 0; k < 6u && !rx_it(j)->start && j > lo; k++)
            j--;
        cand[nc++] = j;
        if (nc >= 4u || rx.hn < 2u)
            break;
    }
    for (i = 0; i < nc; i++) {                      /* each: how it fits (moved as fits best), the line going on, taboo */
        uint32_t best = 0, f, t;
        int32_t bt = a->tx;
        if (i == 0u && on) {
            best = rx_fits(l, cand[i], a->tx);
            w[i] = (1u + best * 4u / 64u) * (uint32_t)a->cont / 10u;
        } else {
            for (t = 0; t < ntx; t++) {
                int32_t x = a->mode == RM_ECHO ? a->tx : TXS[t];
                f = rx_fits(l, cand[i], x) + (x == 0 ? 20u : 0u);
                if (f > best) {
                    best = f;
                    bt = x;
                }
            }
            w[i] = (1u + best * 4u / 64u) * (100u - a->cont) / 10u / (nc > 1u ? nc - 1u : 1u) + 1u;
            cand[i] |= (uint32_t)(bt + 8) << 24;   /* (its move, kept with it: a jump) */
        }
        for (k = 0; k < 3u; k++)
            if (a->taboo[k] == (cand[i] & 0xFFFFFFu))
                w[i] = w[i] / 4u + 1u;
        tot += w[i];
    }
    if (!nc)
        return RX_NONE;
    d = rx_in(0, tot - 1u);
    for (i = 0; i + 1u < nc && d >= w[i]; i++)
        d -= w[i];
    if (cand[i] >> 24) {
        *tx = (int32_t)(cand[i] >> 24) - 8;
        a->sh = 0x7F;                               /* (a jump: its register found again) */
    } else
        *tx = a->tx;
    return cand[i] & 0xFFFFFFu;
}
/* a musician plays its next event, and waits as the guide waited after it */
static void rx_step(uint32_t l)
{
    rx_ag_t *a = &rx.a[l];
    int32_t tx = a->tx;
    uint32_t e = rx_choose(l, &tx), n[3], c, k, gap, len, vel;
    if (e == RX_NONE || !rx_kept(e)) {
        a->next_at = fm1_ms + rx.typ;
        return;
    }
    a->tx = (int8_t)tx;
    if (a->sh == 0x7F || !rx_kept(a->pos))
        a->sh = (int8_t)rx_reg(l, e, tx);
    a->pos = e;
    a->taboo[2] = a->taboo[1];
    a->taboo[1] = a->taboo[0];
    a->taboo[0] = e;
    gap = rx_kept(e + 1u) ? rx_it(e + 1u)->ioi : rx.typ;   /* (the guide's own wait after it: a breath, a phrase's end) */
    if (RX_CPLX < 35 && gap < rx.typ * 3u) {       /* (plain: rounded to a half, one, two or four of its usual gap) */
        uint32_t g = rx.typ / 2u;
        while (g * 3u < gap * 2u && g < rx.typ * 4u)
            g *= 2u;
        gap = g;
    }
    len = rx_it(e)->dur ? rx_it(e)->dur : gap * 9u / 10u;
    gap = (uint32_t)(((uint64_t)gap * a->tf / 4u * exp2_q16((50 - RX_DENS) * 1311)) >> 16);   /* (DENSITY: x 2 .. x 1/2) */
    gap = (uint32_t)clampi((int32_t)gap, 70, 9000);
    len = (uint32_t)clampi((int32_t)(len * a->tf / 4u) + (a->chr == RC_SUST ? 40 : 0), 50, 12000);
    vel = rx_it(e)->vel * a->pct / 100u;
    if (rx_odds(clampi(a->pplay * (25 + 3 * RX_DENS / 2) / 100, 3, 98))) {
        uint32_t at = 0;
        if (RX_CPLX > 70 && a->chr != RC_TEX && gap > 200u && rx_odds(RX_CPLX - 70))
            at = gap / (rx_odds(50) ? 4u : 3u);     /* (pushed off the beat) */
        c = rx_render(l, e, tx, a->sh, n);
        if (RX_CPLX > 60 && a->chr != RC_TEX && gap >= 240u && rx_odds((RX_CPLX - 60) * 3 / 2)) {
            /* split: the step in two or three -- the event, then the next as the guide went on, sooner */
            uint32_t parts = rx_odds(50) ? 2u : 3u, p, m[3], cm = rx_kept(e + 1u) ? rx_render(l, e + 1u, tx, a->sh, m) : 0u;
            uint32_t d = (gap - at) / parts;
            if (parts == 2u && rx_odds(50))
                d = (gap - at) * 2u / 3u;           /* (unevenly: long, short) */
            for (k = 0; k < c; k++)
                rx_play(l, n[k], vel - k * 6u, d * 9u / 10u, at + k * rx_in(0, 18));
            for (p = 1; p < parts && cm; p++)
                rx_play(l, p == 2u ? rx_note((int32_t)rx_dg(m[cm - 1u]) + (n[c - 1u] > m[cm - 1u] ? 1 : -1)) : m[cm - 1u], vel - 8u,
                        ((parts == 2u ? gap - at - d : d) * 9u) / 10u, at + p * d);
        } else
            for (k = 0; k < c; k++)
                rx_play(l, n[k], vel - k * 6u, len, at + k * rx_in(0, 18));
    }
    a->next_at = fm1_ms + gap;
}

/* ---- the guide heard */
static void rx_off(uint32_t note)
{
    uint32_t k;
    for (k = 0; k < rx.nhd; k++)
        if (rx.hd[k].note == note) {
            uint32_t d = fm1_ms - rx.hd[k].t, it = rx.hd[k].it;
            if (rx_kept(it) && rx_it(it)->dur < d)
                rx_it(it)->dur = (uint16_t)(d > 12000u ? 12000u : d);
            rx.hd[k] = rx.hd[--rx.nhd];
            break;
        }
    rx.last_ev = fm1_ms;
}
static void rx_hear(uint32_t note, uint32_t vel, int on)
{
    uint32_t now = fm1_ms, ioi, gap, l, i;
    rx_item_t *it;
    if (!rx.on || note > 127u)
        return;
    if (!on) {
        rx_off(note);
        return;
    }
    vel = vel ? vel : 100u;
    ioi = rx.last_on ? now - rx.last_on : 100000u;
    gap = rx.last_ev ? now - rx.last_ev : 100000u;
    rx.ch[note % 12u] = (uint16_t)(rx.ch[note % 12u] + 500u > 4000u ? 4000u : rx.ch[note % 12u] + 500u);
    rx.mid = (uint8_t)((rx.mid * 7u + note) / 8u);
    if (ioi < 45u && rx.hn) {                       /* (struck with the one before: a chord, one event) */
        it = rx_it(rx.hn - 1u);
        if (it->nn < 3u) {
            for (i = it->nn; i > 0u && it->n[i - 1u] > note; i--)
                it->n[i] = it->n[i - 1u];
            it->n[i] = (uint8_t)note;
            it->nn++;
            it->dg = (uint8_t)rx_dg(it->n[it->nn - 1u]);
        }
    } else {
        if (ioi >= 60u && ioi < 3000u && ioi < rx.typ * 3u)   /* its usual gap */
            rx.typ = (uint32_t)clampi((int32_t)rx.typ + ((int32_t)ioi - (int32_t)rx.typ) / 5, 90, 2500);
        it = rx_it(rx.hn);
        it->n[0] = (uint8_t)note;
        it->nn = 1;
        it->vel = (uint8_t)vel;
        it->dg = (uint8_t)rx_dg(note);
        it->ioi = (uint16_t)(ioi > 8000u ? 8000u : ioi);
        it->dur = 0;
        it->start = (uint8_t)(!rx.hn || (rx.nhd == 0u && gap > rx_rest_ms()));
        rx.hn++;
        rx.vel = (uint8_t)((rx.vel * 3u + vel) / 4u);
        rx.last_on = now;
        rx.ans_done = 0;
        if (it->start) {
            rx.ph0 = rx.hn - 1u;
            if ((int32_t)(now - rx.form_t) > 4500)   /* a phrase begins, some seconds on: the ensemble regroups */
                rx_form();
        }
        for (l = 0; l < NLAYER; l++) {
            rx_ag_t *a = &rx.a[l];
            if (!rx_in_ens(l))
                continue;
            if (a->mode == RM_ANSWER && (int32_t)(a->ans_end - now) > 0) {
                rx_cut(l);                          /* (the guide plays on: the answer stops) */
                a->ans_end = now;
            } else if (a->mode == RM_ECHO && rx.hn > a->lag && rx_odds(clampi(30 + RX_DENS, 5, 98))) {   /* the line, events behind */
                uint32_t e = rx.hn - 1u - a->lag, n[3], c, k;
                if (a->sh == 0x7F || it->start)
                    a->sh = (int8_t)rx_reg(l, e, a->tx);
                c = rx_render(l, e, a->tx, a->sh, n);
                for (k = 0; k < c; k++)
                    rx_play(l, n[k], rx_it(e)->vel * a->pct / 100u, rx_it(e)->dur ? rx_it(e)->dur : rx.typ, rx_in(0, 15));
                a->echoed = e + 1u;
            } else if (a->mode == RM_FOLLOW && (int32_t)(a->next_at - now) > 150 && rx_odds(40))
                a->next_at = now + rx_in(30, 120);  /* (with the guide: quick to come in) */
        }
    }
    if (rx.nhd < RX_HD) {
        rx.hd[rx.nhd].note = (uint8_t)note;
        rx.hd[rx.nhd].t = now;
        rx.hd[rx.nhd].it = rx.hn - 1u;
        rx.nhd++;
    }
    for (i = 0; i < RX_Q; i++)                      /* the harmony moved: a long note that no longer fits lets go */
        if (rx.q[i].on == 2u && (int32_t)(rx.q[i].at - now) > 300 && rx_fit(rx.q[i].note % 12u) < 30u && rx_odds(70))
            rx.q[i].at = now + rx_in(0, 60);
    rx.last_ev = now;
}

static void rx_reset(void)                          /* nothing heard yet (on, or a new guide) */
{
    uint32_t l;
    rx_cut(NLAYER);
    rx.nhd = 0;
    memset(rx.ch, 0, sizeof rx.ch);
    rx.hn = 0;
    rx.last_on = rx.last_ev = rx.form_t = 0;
    rx.typ = 500;
    rx.vel = 96;
    rx.mid = 60;
    rx.ans_done = 1;
    rx.guide = ui.sel;
    rx_ear = rx.on ? ui.sel : 0xFFu;
    for (l = 0; l < NLAYER; l++) {
        rx.a[l].pos = rx.a[l].echoed = RX_NONE;
        rx.a[l].mode = RM_REST;
        rx.a[l].sh = 0x7F;
    }
    rx_form();
}
static void rx_set(int on)
{
    static const int8_t KD[4] = {50, 40, 60, 40};
    if (!rx.k[0] && !rx.k[1] && !rx.k[2] && !rx.k[3])
        memcpy(rx.k, KD, sizeof rx.k);              /* (the page's values, first time on) */
    rx.on = (uint8_t)on;
    rx_reset();
}

/* the main loop, every pass: the guide's loops heard; the musicians' steps as they fall due; its rests answered,
 * its silence kept; the notes due */
static void rx_tick(void)
{
    uint32_t now = fm1_ms, i, l;
    if (rx.on && rx.guide != ui.sel)
        rx_reset();                                 /* (another track chosen: it leads now) */
    while (rx_lr != rx_lw) {                        /* the guide's loops' notes (the ISR's) */
        uint32_t n = rx_lq[rx_lr % 16u][0], v = rx_lq[rx_lr % 16u][1];
        RING_PUBLISH();
        rx_lr++;
        rx_hear(n, v, v != 0u);
    }
    if (rx.on && now - rx.decay_t >= 100u) {        /* the harmony lately, fading (a second or two) */
        rx.decay_t = now;
        for (i = 0; i < 12u; i++)
            rx.ch[i] = (uint16_t)(rx.ch[i] - rx.ch[i] / 14u);
    }
    if (rx.on && rx.last_on && rx.hn >= 2u) {
        uint32_t gap = now - rx.last_ev, playing = rx.nhd || gap < rx_rest_ms() * 2u;
        if (!rx.nhd && !rx.ans_done && gap > rx_ans_ms()) {   /* a rest: the echoes finish the line, one answers */
            uint32_t na = 0, k, j = 0, phl = 0;
            rx.ans_done = 1;
            for (l = 0; l < NLAYER; l++) {
                rx_ag_t *a = &rx.a[l];
                if (!rx_in_ens(l))
                    continue;
                if (a->mode == RM_ECHO && rx_kept(a->echoed)) {
                    uint32_t t = rx.typ / 2u, e;
                    for (e = a->echoed; e < rx.hn; e++) {
                        uint32_t n[3], c = rx_render(l, e, a->tx, a->sh == 0x7F ? 0 : a->sh, n);
                        if (e > a->echoed)
                            t += (uint32_t)clampi(rx_it(e)->ioi, 70, 1500);
                        for (k = 0; k < c; k++)
                            rx_play(l, n[k], rx_it(e)->vel * a->pct / 100u, rx_it(e)->dur ? rx_it(e)->dur : rx.typ, t);
                    }
                    a->echoed = RX_NONE;
                }
                na += a->mode == RM_ANSWER;
            }
            for (i = rx.ph0 + 1u; i < rx.hn && rx_kept(i); i++)
                phl += rx_it(i)->ioi;
            if (na)                                 /* (the answers take turns) */
                for (l = 0; l < NLAYER; l++)
                    if (rx_in_ens(l) && rx.a[l].mode == RM_ANSWER && j++ == rx.turn % na) {
                        rx_ag_t *a = &rx.a[l];
                        uint32_t best = RX_NONE, lvl;
                        rx.turn++;
                        for (lvl = 0; lvl < 3u && best == RX_NONE; lvl++)   /* where the guide once went on from here */
                            for (i = rx.hn > RX_H ? rx.hn - RX_H + 3u : 3u; i + 1u < rx.hn; i++)
                                if (rx_same(i, rx.hn - 1u, 3u - lvl, lvl) && (best == RX_NONE || rx_odds(50)))
                                    best = i;
                        a->pos = best != RX_NONE ? best : rx.ph0 ? rx.ph0 - 1u : RX_NONE;
                        a->sh = 0x7F;
                        a->ans_end = now + (uint32_t)clampi((int32_t)phl * 3 / 2, 1500, 8000);
                        a->next_at = now + (uint32_t)clampi((int32_t)rx.typ / 3, 60, 400);
                        break;
                    }
        }
        for (l = 0; l < NLAYER; l++) {              /* each musician, on its own time */
            rx_ag_t *a = &rx.a[l];
            int go;
            if (!rx_in_ens(l) || a->mode == RM_REST || a->mode == RM_ECHO)
                continue;
            go = a->mode == RM_ANSWER ? (int32_t)(a->ans_end - now) > 0 && !rx.nhd : playing;
            if (!go) {
                if ((int32_t)(a->next_at - now) < 0)
                    a->next_at = now;               /* (back in at once when the guide is) */
                continue;
            }
            if ((int32_t)(now - a->next_at) >= 0)
                rx_step(l);
        }
        if (playing && (int32_t)(now - rx.form_t) > 6000)
            rx_form();                              /* (playing on without a rest: the ensemble regroups all the same) */
        if (!rx.nhd && gap > (rx.typ * 10u > 8000u ? rx.typ * 10u : 8000u)) {   /* silence: all let go, waiting */
            rx_cut(NLAYER);
            rx.last_on = 0;
        }
    }
    for (i = 0; i < RX_Q; i++) {
        if (rx.q[i].on == 1u && (int32_t)(now - rx.q[i].at) >= 0) {
            uint32_t pv = press_vel;
            press_vel = rx.q[i].vel;
            seq_press(rx.q[i].layer, rx.q[i].note, 0, 1);
            press_vel = (uint8_t)pv;
            rx.q[i].on = 2;
            rx.q[i].at = now + rx.q[i].len;
        } else if (rx.q[i].on == 2u && (int32_t)(now - rx.q[i].at) >= 0) {
            seq_press(rx.q[i].layer, rx.q[i].note, 0, 0);
            rx.q[i].on = 0;
        }
    }
}

static void rx_knob(uint32_t j, int32_t s)        /* the page: K3 .. K6 */
{
    rx.k[j] = (int8_t)clampi(rx.k[j] + 2 * s, 0, 100);
    if (j == 3u || j == 2u)
        rx.form_t = fm1_ms - 5000u;                 /* (SPREAD, CONTINUITY: the ensemble regroups a second after) */
}
/* REACT's pads (the bottom row, 9 .. 14): track j in or out; the guide's, red a moment */
static void rx_pad_hit(uint32_t j)
{
    static char m[24];
    if (j >= NLAYER)
        return;
    if (j == ui.sel) {
        rx.red_until = fm1_ms + 1200u;
        say("GUIDE TRACK", 1200);
        return;
    }
    rx.mask = (uint8_t)(rx.mask ^ (1u << j));
    rx_cut(j);
    rx.a[j].pos = rx.a[j].echoed = RX_NONE;
    rx_form();                                      /* (the ensemble regrouped with it, or without) */
    m[0] = (char)('1' + j);
    m[1] = ' ';
    str_cpy(m + 2, (rx.mask >> j) & 1u ? "IN" : "OUT", sizeof m - 2u);
    say(m, 1200);
}
