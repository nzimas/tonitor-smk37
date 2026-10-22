/* SPDX-License-Identifier: GPL-3.0-only
 * TONITOR: REACT, the other tracks answering the one played (SHIFT + FADER BANK: on or off, its pads up with it;
 * held 0.6 s, its pads up or down). The chosen track is the guide; on REACT's pads (the bottom row, 9 .. 14 the six
 * tracks) the others are taken in or out -- in lit, out dim, the guide's pad red a moment when pressed (it cannot
 * follow itself). Choosing another track makes it the guide.
 *
 * It listens to everything the guide sounds -- its keys, latched notes, chords, MIDI in, its sequencers, its loops
 * -- and keeps: the notes down, each pitch class's weight lately, the last phrase (its notes and their spacing),
 * the pulse (the time between its notes, folded by halves and doubles into 180 .. 1200 ms, its beats locked to
 * the notes that fall near them), how busy it is, how hard it plays, where on the keyboard. From those, the chord:
 * the step of the scale whose triad best fits the notes down and lately, the lowest note counting most.
 *
 * Each track taken in plays a part, by its place after the guide: the first ORNAMENT (the chord's notes broken
 * above the guide, more of them a beat as the guide leaves room, fewer as it gets busy), the second BASS (the root
 * under it, on the bar and at each change of chord; walking as it grows), the third PAD (the chord held, voiced
 * near the guide's middle, moved to the nearest notes at each change), the fourth ECHO (the guide's notes answered
 * a beat or half a beat later, a third away, quieter), the fifth TALK (in the guide's rests, its last phrase
 * answered: moved onto the chord and to the other octave, backwards, upside down or slower; it stops when the guide
 * plays on). All of it in the scale, softer than the guide.
 *
 * It grows: the longer the guide plays, the more each part does (sevenths and ninths, walking, grace notes,
 * inversions); a few beats of silence and the parts let go and wait. Its notes go as a sequencer's (the looper
 * does not record them), one note each (not as the track's chord). It runs in the main loop on fm1_ms. */

#define RX_PH 12                 /* the guide's last phrase, notes */
#define RX_Q 28                  /* notes to come and sounding */
enum { RR_ORN, RR_BASS, RR_PAD, RR_ECHO, RR_TALK, RR_N };
static const char *const RR_NAME[RR_N] = {"ORNAMENT", "BASS", "PAD", "ECHO", "TALK"};

static struct {
    uint8_t on, mask, guide;
    uint32_t held[4];            /* the guide's notes down (a bit a MIDI note) */
    uint16_t w[12];              /* each pitch class's weight lately */
    uint8_t ph[RX_PH], ph_n;     /* the last phrase: its notes .. */
    uint16_t ph_dt[RX_PH];       /* .. and the time before each, ms */
    uint8_t talked;              /* (the phrase answered) */
    uint32_t last_on, beat_at, pulse, beat;   /* (beat_at 0: the clock stopped, waiting for the guide) */
    int32_t dens;                /* the guide's notes a beat lately, Q8 */
    uint8_t vel, lo, hi, mid;    /* how hard it plays, where */
    int8_t deg;                  /* the chord's root, a step of the scale (-1: none yet) */
    int8_t pad_deg[NLAYER];      /* (the chord a PAD holds) */
    uint8_t cur[NLAYER];         /* (an ORNAMENT's last note) .. */
    int8_t dir[NLAYER];          /* .. and which way it goes */
    uint16_t grow;               /* 0 .. 1000: how long the guide has been playing */
    uint32_t red_until;          /* (the guide's pad, pressed: red) */
    uint8_t rdown, rdone;        /* (SHIFT + FADER BANK held) */
    uint32_t rt0;
    struct { uint8_t layer, note, vel, on; uint32_t at, len; } q[RX_Q];   /* on: 1 to strike, 2 sounding */
    rnd_t r;
} rx;

static uint32_t rx_rnd(void)
{
    if (!rx.r.s)
        rnd_seed(&rx.r, fm1_ms ^ 0x5EAC7u);
    return rnd_next(&rx.r);
}
static int rx_odds(uint32_t pct) { return (rx_rnd() >> 8) % 100u < pct; }
static uint32_t rx_in(uint32_t lo, uint32_t hi) { return lo + (rx_rnd() >> 8) % (hi - lo + 1u); }
static uint32_t rx_vel(uint32_t pct) { return (uint32_t)clampi((int32_t)(rx.vel * pct / 100u), 12, 120); }
static uint32_t rx_cx(void) { return rx.grow / 250u; }   /* 0 .. 4: how much each part does */

/* the parts: track l's (by its place among those taken in, counting on from the guide), or -1 */
static int rx_role(uint32_t l)
{
    uint32_t k, r = 0;
    if (l == rx.guide || !((rx.mask >> l) & 1u))
        return -1;
    for (k = rx.guide + 1u; k % NLAYER != l; k++)
        if ((rx.mask >> (k % NLAYER)) & 1u)
            r++;
    return r < RR_N ? (int)r : -1;
}

/* the queue: a note of track l to strike in wait ms, for len ms, at vel */
static void rx_play(uint32_t l, uint32_t note, uint32_t vel, uint32_t len, uint32_t wait)
{
    uint32_t i, slot = RX_Q;
    if (note > 127u)
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
    rx.q[slot].vel = (uint8_t)clampi((int32_t)vel, 1, 126);
    rx.q[slot].len = len < 30u ? 30u : len;
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
    if (l >= NLAYER)
        for (l = 0; l < NLAYER; l++)
            rx.pad_deg[l] = -1;
    else
        rx.pad_deg[l] = -1;
}

/* the scale: its step i's pitch class; is the scale chromatic (then the chord in semitones) */
static uint32_t rx_sp(int32_t i)
{
    int32_t n = SCALE_N[ui.scale];
    return (ui.root + SCALE_STEP[ui.scale][((i % n) + n) % n]) % 12u;
}
static int rx_chrom(void) { return SCALE_N[ui.scale] >= 12u; }
/* the chord's tone k (0 root, 1 third, 2 fifth, 3 seventh, 4 ninth): its pitch class */
static uint32_t rx_tpc(uint32_t k)
{
    static const uint8_t SEMI[5] = {0, 4, 7, 10, 14};
    if (rx_chrom())
        return (rx_sp(rx.deg) + SEMI[k % 5u]) % 12u;
    return rx_sp(rx.deg + 2 * (int32_t)(k % 5u));
}
static uint32_t rx_cmask(uint32_t nt)               /* the chord's first nt tones, as pitch classes */
{
    uint32_t k, m = 0;
    for (k = 0; k < nt; k++)
        m |= 1u << rx_tpc(k);
    return m;
}
/* pitch class pc nearest note near (-6 .. +5) */
static uint32_t rx_near(uint32_t pc, int32_t near)
{
    int32_t d = (((int32_t)pc - near % 12 + 18) % 12) - 6;
    return (uint32_t)clampi(near + d, 0, 127);
}
/* note n in the scale (the nearest scale note, up first) */
static uint32_t rx_snap(int32_t n)
{
    uint32_t m = scale_mask();
    n = clampi(n, 0, 127);
    if ((m >> (n % 12)) & 1u)
        return (uint32_t)n;
    if ((m >> ((n + 1) % 12)) & 1u)
        return (uint32_t)clampi(n + 1, 0, 127);
    return (uint32_t)clampi(n - 1, 0, 127);
}
/* the next note of the chord (nt tones) from n, up (d 1) or down (-1) */
static uint32_t rx_next(uint32_t n, int32_t d, uint32_t nt)
{
    uint32_t m = rx_cmask(nt);
    int32_t t = (int32_t)n;
    do
        t += d;
    while (t > 0 && t < 127 && !((m >> (t % 12)) & 1u));
    return (uint32_t)clampi(t, 0, 127);
}
static uint32_t rx_low(void)                        /* the guide's lowest note down (128: none) */
{
    uint32_t k;
    for (k = 0; k < 128u; k++)
        if ((rx.held[k >> 5] >> (k & 31u)) & 1u)
            return k;
    return 128u;
}

/* the chord, from the notes down and lately: 1 if it changed */
static int rx_chord(void)
{
    uint32_t hm = 0, k, low = rx_low(), lp = low < 128u ? low % 12u : 99u, w[12];
    int32_t n = SCALE_N[ui.scale], i, best = -1;
    uint32_t bs = 0;
    for (k = 0; k < 128u; k++)
        if ((rx.held[k >> 5] >> (k & 31u)) & 1u)
            hm |= 1u << (k % 12u);
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

/* the parts */
static void rx_bass(uint32_t l, uint32_t k, uint32_t len, uint32_t wait, uint32_t pct)
{
    int32_t near = clampi((int32_t)rx.lo - 12, 28, 50);
    uint32_t n = rx_near(rx_tpc(k), near);
    rx_play(l, n, rx_vel(pct), len, wait);
}
static void rx_pad(uint32_t l)                      /* the chord held, voiced near the guide's middle */
{
    uint32_t cx = rx_cx(), nt = cx >= 2u ? 4u : 3u, k, last = 0;
    int32_t c = clampi((int32_t)rx.mid - 5, 48, 70);
    rx_cut(l);
    for (k = 0; k < nt; k++) {
        uint32_t t = (k == 0u && cx >= 3u) ? 4u : k;   /* (grown: the root left to the bass, a ninth for it) */
        uint32_t n = rx_near(rx_tpc(t), c - 4 + 4 * (int32_t)k);
        if (n == last)
            continue;
        rx_play(l, n, rx_vel(48), 60000u, k * 18u);   /* (a slow strum; held till the chord moves) */
        last = n;
    }
    rx.pad_deg[l] = rx.deg;
}
static void rx_on_chord(void)                       /* a new chord: the bass and the pads move at once */
{
    uint32_t l;
    for (l = 0; l < NLAYER; l++) {
        int r = rx_role(l);
        if (r == RR_BASS) {
            rx_cut(l);
            rx_bass(l, 0, rx.pulse * 2u, 0, 85);
        } else if (r == RR_PAD)
            rx_pad(l);
    }
}
static void rx_orn(uint32_t l)                      /* a beat of the chord broken above the guide */
{
    uint32_t cx = rx_cx(), sub, j, nt = cx >= 2u ? 4u : 3u, sm;
    int32_t top = clampi((int32_t)rx.hi + 15, 64, 100), bot = clampi((int32_t)rx.hi + 2, 52, 88);
    if (rx.dens > 3 * 256 || rx_odds(rx.dens > 2 * 256 ? 60u : 20u))
        return;                                     /* (the guide busy: room left; and a breath now and then) */
    sub = rx.dens > 2 * 256 ? 1u : rx.dens > 256 ? 2u : 2u + (cx > 2u ? 2u : cx);
    sm = rx.pulse / sub;
    if (rx.cur[l] < (uint32_t)bot || rx.cur[l] > (uint32_t)top)
        rx.cur[l] = (uint8_t)rx_near(rx_tpc(0), bot + 3);
    for (j = 0; j < sub; j++) {
        uint32_t n = rx_next(rx.cur[l], rx.dir[l] ? rx.dir[l] : 1, nt);
        if ((int32_t)n > top || (int32_t)n < bot) {
            rx.dir[l] = (int8_t)(rx.dir[l] > 0 ? -1 : 1);
            n = rx_next(rx.cur[l], rx.dir[l], nt);
        }
        if (cx >= 2u && sm > 160u && rx_odds(20u))   /* a grace note: the scale's neighbour, just before */
            rx_play(l, rx_snap((int32_t)n + (rx_odds(50u) ? 1 : -1) * 2), rx_vel(40), 60u, j * sm + sm - 70u);
        rx_play(l, n, rx_vel(j ? 52u : 68u), sm * 8u / 10u, j * sm);
        rx.cur[l] = (uint8_t)n;
    }
    if (rx_odds(15u))
        rx.dir[l] = (int8_t)-rx.dir[l];
}
static void rx_echo(uint32_t l, uint32_t note)     /* a guide's note answered a third away, later */
{
    uint32_t cx = rx_cx(), wait = cx >= 2u && rx_odds(50u) ? rx.pulse / 2u : rx.pulse, m = scale_mask();
    int32_t n;
    if (rx.dens > 2 * 256 && rx_odds(50u))
        return;
    if (cx >= 3u && rx.ph_n && rx_odds(30u))        /* (grown: now and then the phrase upside down) */
        n = (int32_t)rx_snap(2 * (int32_t)rx.ph[0] - (int32_t)note);
    else if (rx_chrom())
        n = (int32_t)note + (rx_odds(50u) ? 4 : -3);
    else
        n = (int32_t)lx_map(note, rx_odds(50u) ? 2 : -2, m);
    if (rx.mid > 72u)
        n -= 12;
    rx_play(l, (uint32_t)clampi(n, 24, 108), rx_vel(58), rx.pulse * 8u / 10u, wait);
}
static void rx_talk(uint32_t l)                    /* the guide's rest: its last phrase answered */
{
    uint32_t cx = rx_cx(), k = rx.ph_n < 3u + cx ? rx.ph_n : 3u + cx, s = rx.ph_n - k, i, form = rx_in(0, cx >= 1u ? 3u : 1u), t = 0;
    int32_t sh = rx.mid > 66u ? -12 : 12, first, d;
    first = (int32_t)rx.ph[form == 1u ? rx.ph_n - 1u : s] + sh;
    d = (int32_t)rx_near(rx_tpc(rx_in(0, 2)), first) - first;   /* (onto the chord) */
    for (i = 0; i < k; i++) {
        uint32_t ix = form == 1u ? rx.ph_n - 1u - i : s + i, gap;
        int32_t n = (int32_t)rx.ph[ix] + sh;
        if (form == 2u)                             /* upside down, about its first note */
            n = 2 * first - n;
        gap = i + 1u < k ? rx.ph_dt[form == 1u ? ix : ix + 1u] : rx.pulse;
        gap = (uint32_t)clampi((int32_t)gap, 90, (int32_t)rx.pulse * 2);
        if (form == 3u)
            gap = gap * 3u / 2u;                    /* slower */
        rx_play(l, rx_snap(n + d), rx_vel(i ? 64u : 74u), gap * 85u / 100u, t + rx.pulse / 4u);
        t += gap;
    }
}
/* a beat: what each part plays on it */
static void rx_beat(void)
{
    uint32_t l, cx = rx_cx(), bar = rx.beat % 4u;
    for (l = 0; l < NLAYER; l++)
        switch (rx_role(l)) {
        case RR_BASS:
            if (bar == 0u) {
                rx_cut(l);
                rx_bass(l, 0, cx ? rx.pulse * 9u / 10u : rx.pulse * 37u / 10u, 0, 85);
            } else if (cx >= 2u) {                  /* walking: the third, the fifth, a step to the next root */
                if (bar == 1u && rx_odds(60u))
                    rx_bass(l, 1, rx.pulse * 85u / 100u, 0, 70);
                else if (bar == 2u)
                    rx_bass(l, 2, rx.pulse * 85u / 100u, 0, 75);
                else if (bar == 3u) {
                    int32_t near = clampi((int32_t)rx.lo - 12, 28, 50);
                    uint32_t r = rx_near(rx_tpc(0), near);
                    rx_play(l, rx_snap((int32_t)r + (rx_odds(50u) ? 2 : -1)), rx_vel(65), rx.pulse * 8u / 10u, 0);
                    if (cx >= 3u && r + 12u < rx.lo && rx_odds(40u))   /* (and a push into the bar, still under the guide) */
                        rx_play(l, r + 12u, rx_vel(60), rx.pulse / 3u, rx.pulse / 2u);
                }
            } else if (cx && bar == 2u)
                rx_bass(l, 2, rx.pulse * 9u / 10u, 0, 75);
            break;
        case RR_PAD:
            if (rx.pad_deg[l] != rx.deg || (cx >= 3u && bar == 0u && rx_odds(25u)))
                rx_pad(l);
            break;
        case RR_ORN:
            rx_orn(l);
            break;
        default:
            break;
        }
}

/* the guide's note, struck (vel) or let go */
static void rx_hear(uint32_t note, uint32_t vel, int on)
{
    uint32_t now = fm1_ms, ioi, l, pc = note % 12u;
    if (!rx.on || note > 127u)
        return;
    if (!on) {
        rx.held[note >> 5] &= ~(1u << (note & 31u));
        return;
    }
    rx.held[note >> 5] |= 1u << (note & 31u);
    rx.w[pc] = (uint16_t)(rx.w[pc] + 300u > 4000u ? 4000u : rx.w[pc] + 300u);
    rx.vel = (uint8_t)((rx.vel * 3u + (vel ? vel : 100u)) / 4u);
    rx.mid = (uint8_t)((rx.mid * 7u + note) / 8u);
    rx.lo = (uint8_t)(note < rx.lo ? note : rx.lo + (rx.lo < rx.mid && rx_odds(20u)));
    rx.hi = (uint8_t)(note > rx.hi ? note : rx.hi - (rx.hi > rx.mid && rx_odds(20u)));
    ioi = rx.last_on ? now - rx.last_on : 100000u;
    if (ioi >= 60u) {                               /* a new note (not a chord's others) */
        if (ioi < 2000u) {                          /* the pulse: the gap, folded by halves and doubles near it */
            uint32_t f = ioi;
            while (f > rx.pulse * 3u / 2u && f > 300u)
                f /= 2u;
            while (f < rx.pulse * 2u / 3u && f * 2u <= 1200u)
                f *= 2u;
            rx.pulse = (uint32_t)clampi((int32_t)rx.pulse + ((int32_t)f - (int32_t)rx.pulse) / 6, 180, 1200);
        }
        if (!rx.beat_at) {                          /* (the clock stopped: it starts on this note, a downbeat) */
            rx.beat_at = now;
            rx.beat = 0;
        } else {                                    /* the beats locked to the notes near them */
            int32_t e = (int32_t)(now - (rx.beat_at - rx.pulse));
            if (e > (int32_t)rx.pulse / 2)
                e -= (int32_t)rx.pulse;
            if (e > -(int32_t)rx.pulse / 4 && e < (int32_t)rx.pulse / 4)
                rx.beat_at += (uint32_t)(e / 2);
        }
        rx.dens += 256;
        if (ioi > rx.pulse * 5u / 2u) {             /* a new phrase */
            rx.ph_n = 0;
            rx.talked = 0;
        }
        if (rx.ph_n >= RX_PH) {
            uint32_t k;
            for (k = 0; k + 1u < RX_PH; k++) {
                rx.ph[k] = rx.ph[k + 1u];
                rx.ph_dt[k] = rx.ph_dt[k + 1u];
            }
            rx.ph_n = RX_PH - 1u;
        }
        rx.ph[rx.ph_n] = (uint8_t)note;
        rx.ph_dt[rx.ph_n++] = (uint16_t)(ioi > 4000u ? 4000u : ioi);
        rx.last_on = now;
        rx.talked = 0;
        for (l = 0; l < NLAYER; l++) {
            int r = rx_role(l);
            if (r == RR_TALK)
                rx_cut(l);                          /* (the guide plays on: TALK stops) */
            else if (r == RR_ECHO)
                rx_echo(l, note);
        }
    } else if (rx.ph_n && note > rx.ph[rx.ph_n - 1u])
        rx.ph[rx.ph_n - 1u] = (uint8_t)note;        /* (a chord: its top note, the tune's) */
    if (rx_chord())
        rx_on_chord();
}

static void rx_reset(void)                          /* nothing heard yet (on, or a new guide) */
{
    rx_cut(NLAYER);
    memset(rx.held, 0, sizeof rx.held);
    memset(rx.w, 0, sizeof rx.w);
    rx.ph_n = 0;
    rx.talked = 1;
    rx.last_on = rx.beat_at = 0;
    rx.pulse = 500;
    rx.dens = 0;
    rx.vel = 96;
    rx.lo = rx.hi = rx.mid = 60;
    rx.deg = -1;
    rx.guide = ui.sel;
    rx_ear = rx.on ? ui.sel : 0xFFu;
}
static void rx_set(int on)
{
    rx.on = (uint8_t)on;
    rx_reset();
    rx.grow = 0;
}

/* the main loop, every pass */
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
    if (rx.on && rx.beat_at && (int32_t)(now - rx.beat_at) >= 0) {
        int held = (rx.held[0] | rx.held[1] | rx.held[2] | rx.held[3]) != 0u;
        if (!held && now - rx.last_on > rx.pulse * 8u) {   /* quiet a while: the parts let go and wait */
            rx_cut(NLAYER);
            rx.beat_at = 0;
            rx.deg = -1;
            rx.grow = (uint16_t)(rx.grow / 2u);
            memset(rx.w, 0, sizeof rx.w);
        } else {
            rx.beat_at += rx.pulse;
            if ((int32_t)(now - rx.beat_at) >= 0)
                rx.beat_at = now + rx.pulse;        /* (fallen behind: on from now) */
            for (i = 0; i < 12u; i++)
                rx.w[i] = (uint16_t)(rx.w[i] * 7u / 8u);
            rx.dens -= rx.dens / 3;
            if (now - rx.last_on < rx.pulse * 2u || held)
                rx.grow = (uint16_t)(rx.grow + 3u > 1000u ? 1000u : rx.grow + 3u);
            if (rx.deg < 0 && rx_chord())
                rx_on_chord();
            if (rx.deg >= 0)
                rx_beat();
            rx.beat++;
        }
    }
    if (rx.on && !rx.talked && rx.ph_n >= 3u && rx.deg >= 0 && now - rx.last_on > rx.pulse * 5u / 4u) {
        rx.talked = 1;                              /* the guide resting: TALK answers its phrase */
        for (l = 0; l < NLAYER; l++)
            if (rx_role(l) == RR_TALK)
                rx_talk(l);
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

/* REACT's pads (the bottom row, 9 .. 14): track j in or out; the guide's, red a moment */
static void rx_pad_hit(uint32_t j)
{
    static char m[24];
    int r;
    m[0] = (char)('1' + j);
    m[1] = ' ';
    if (j >= NLAYER)
        return;
    if (j == ui.sel) {
        rx.red_until = fm1_ms + 1200u;
        say("GUIDE TRACK", 1200);
        return;
    }
    rx.mask = (uint8_t)(rx.mask ^ (1u << j));
    rx_cut(NLAYER);                                 /* (the parts dealt again: each starts afresh) */
    r = rx_role(j);
    str_cpy(m + 2, !((rx.mask >> j) & 1u) ? "OUT" : r >= 0 ? RR_NAME[r] : "IN", sizeof m - 2u);
    say(m, 1200);
}
