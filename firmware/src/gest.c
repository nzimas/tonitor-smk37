/* SPDX-License-Identifier: GPL-3.0-only
 * TONITOR: the gesture layers -- three a track, the OVERDUBS page's pads 6 .. 8 (after the note loops' five). A gesture
 * layer is the track's knobs as they were turned, kept as events: the filter, the granulator, the crush, the
 * distortion, the delay and reverb sends, the level, the pan and the eight macros of the sound it plays. It plays them
 * back over and over at its own length, whatever the loops' -- so it drifts against them, and a sweep made over one
 * part of the loop comes back over another: by design, not by accident.
 *
 *   an empty gesture pad pressed      recording, at once: every turn of those knobs on that track kept, from now
 *   the same pad again                its end: the layer's length (from the first press), on and playing
 *   REC armed, then a knob turned     (before any note) recording into the first empty layer, from that turn; REC
 *                                     again ends it -- as a loop is recorded, but of the knobs (loops full: REC
 *                                     arms for this alone)
 *   a recorded pad pressed            on or off
 *   a recorded pad held 0.8 s         emptied
 *
 * A layer plays while its track's loops play, or freely on a track with no loops; it starts from its beginning as the
 * loops are started. What it plays sets the knobs' values (as the hand would; the hand may turn them between its
 * events). One layer records at a time. The events of all the layers share one store (GEV_N); one emptied, the others
 * close up. Sessions keep them (sessions.c, format 33); snapshots and the clipboard do not. All in the main loop */

#define GL_N 3u                  /* gesture layers a track (the OVERDUBS page's pads 6 .. 8) */
#define GEV_N 1024u              /* the events of every layer between them (4 KiB, the pool -- all it has room for) */
#define GP_N (GP_MAC0 + 8u)      /* the controls (GP_*: ui.c) */
/* an event: its time from the layer's start (ms, 20 bits: up to 17 minutes), the control (4 bits), its value (8) */
#define GE(t, p, v) (((uint32_t)(t) & 0xFFFFFu) | (uint32_t)(p) << 20 | (uint32_t)(uint8_t)(int8_t)(v) << 24)
#define GE_T(e) ((e) & 0xFFFFFu)
#define GE_P(e) ((e) >> 20 & 15u)
#define GE_V(e) ((int8_t)(uint8_t)((e) >> 24))
#define GL_TMAX 0xFFFFFu
typedef struct {
    uint16_t first, n;           /* its events in gev[] */
    uint32_t len;                /* ms (0: empty) */
    uint8_t on;
    uint16_t ix;                 /* the next event to play */
    uint32_t pos;                /* where it is, ms */
} glayer_t;
static uint32_t gev[GEV_N] POOL;
static glayer_t gl[NLAYER][GL_N];
static uint32_t gev_used;
static struct {
    uint8_t on, l, k, full;      /* recording: track l's layer k (its events the last in gev[]) */
    uint32_t t0;
} grec;
static struct {
    int8_t k;                    /* a recorded pad down (its layer, -1 none), since t0 */
    uint8_t l, done;
    uint32_t t0;
} gpad = {-1, 0, 0, 0};
static uint32_t gtick_at;
static uint8_t garm;                     /* REC armed for a gesture alone (the loops full): its track + 1 */
static uint8_t gplay_was[NLAYER];

static int gest_has(uint32_t l, uint32_t k) { return gl[l][k].len != 0u; }
static int gest_rec_on(uint32_t l, uint32_t k) { return grec.on && grec.l == l && grec.k == k; }
static int gest_is_on(uint32_t l, uint32_t k) { return gl[l][k].on; }

/* a control's value given back to the track: the knob's value, then to the ISR */
static void gest_apply(uint32_t l, uint32_t p, int32_t v)
{
    if (p >= GP_MAC0) {
        ui.mac[l][ui.engine[l]][p - GP_MAC0] = (int8_t)clampi(v, -MAC_MAX, MAC_MAX);
        mac_apply(l);
        return;
    }
    switch (p) {
    case GP_FLT: ui.flt[l] = (int8_t)clampi(v, -FLT_MAX, FLT_MAX); break;
    case GP_GR: ui.gr[l] = (int8_t)clampi(v, -FLT_MAX, FLT_MAX); break;
    case GP_CR: ui.cr[l] = (int8_t)clampi(v, -FLT_MAX, FLT_MAX); break;
    case GP_DIST: ui.dist[l] = (int8_t)clampi(v, 0, 100); break;
    case GP_DSEND: ui.dsend[l] = (int8_t)clampi(v, 0, 100); break;
    case GP_SEND: ui.send[l] = (int8_t)clampi(v, 0, 100); break;
    case GP_LEVEL: ui.level[l] = (int8_t)clampi(v, 0, LEVEL_MAX); break;
    default: ui.pan[l] = (int8_t)clampi(v, -PAN_MAX, PAN_MAX); break;
    }
    level_pan_apply(l);
}

/* track l's layer k recording, from now */
static void gest_start(uint32_t l, uint32_t k)
{
    char m[16] = "G";
    glayer_t *g = &gl[l][k];
    memset(g, 0, sizeof *g);
    g->first = (uint16_t)gev_used;
    grec.on = 1;
    grec.l = (uint8_t)l;
    grec.k = (uint8_t)k;
    grec.full = 0;
    grec.t0 = fm1_ms;
    garm = 0;
    m[1] = (char)('1' + k);
    str_cpy(m + 2, " REC", 5);
    say(m, 1500);
    ui.force = 1;
}
/* the knobs (ui.c): control p of track l turned to v -- kept if that layer records (turns 15 ms apart or less: the
 * last one's value). REC armed and no note played yet: this turn starts a take of the knobs, in the first empty
 * layer (the loop's arming let go) */
static void gest_rec(uint32_t l, uint32_t p, int32_t v)
{
    glayer_t *g;
    uint32_t t;
    if (!grec.on && l == ui.sel && (loop[l].rec == LR_ARMED || garm == l + 1u)) {
        uint32_t k;
        for (k = 0; k < GL_N && gl[l][k].len; k++)
            ;
        if (k == GL_N) {
            say("3 GESTURES", 1500);             /* (every layer taken: empty one first) */
            return;
        }
        if (loop[l].rec == LR_ARMED)
            ev_post(EV_LOOP_REC, l, 0, 0);       /* (the loop's arming let go: this take is of the knobs) */
        gest_start(l, k);
    }
    if (!grec.on || grec.l != l)
        return;
    g = &gl[l][grec.k];
    t = fm1_ms - grec.t0;
    if (t > GL_TMAX)
        return;
    if (g->n && GE_P(gev[g->first + g->n - 1u]) == p && t - GE_T(gev[g->first + g->n - 1u]) <= 15u) {
        gev[g->first + g->n - 1u] = GE(GE_T(gev[g->first + g->n - 1u]), p, v);
        return;
    }
    if (gev_used >= GEV_N) {
        if (!grec.full)
            say("GESTURES FULL", 1500);
        grec.full = 1;
        return;
    }
    gev[gev_used++] = GE(t, p, v);
    g->n++;
}
static void gest_rec_end(void)
{
    glayer_t *g = &gl[grec.l][grec.k];
    uint32_t t = fm1_ms - grec.t0;
    char m[16] = "G";
    g->len = t < 100u ? 100u : t > GL_TMAX ? GL_TMAX : t;
    g->on = 1;
    g->ix = 0;
    g->pos = 0;
    grec.on = 0;
    m[1] = (char)('1' + grec.k);
    m[2] = 0;
    say(m, 1200);
    ui.force = 1;
}
/* track l's layer k emptied: its events out of the store, the others' moved up */
static void gest_empty(uint32_t l, uint32_t k)
{
    glayer_t *g = &gl[l][k];
    uint32_t f = g->first, n = g->n, i, j;
    if (gest_rec_on(l, k))
        grec.on = 0;
    if (n) {
        for (i = f; i + n < gev_used; i++)
            gev[i] = gev[i + n];
        gev_used -= n;
        for (i = 0; i < NLAYER; i++)
            for (j = 0; j < GL_N; j++)
                if (gl[i][j].n && gl[i][j].first > f)
                    gl[i][j].first = (uint16_t)(gl[i][j].first - n);
    }
    memset(g, 0, sizeof *g);
}
/* REC pressed on track l (ui.c): a take of the knobs ended (1); or, the loops full, REC armed for one (2), or that
 * let go (3); or nothing of the gestures' (0: the looper's) */
static int gest_rec_press(uint32_t l, int loops_full)
{
    if (grec.on) {
        gest_rec_end();
        return 1;
    }
    if (garm) {                                  /* (armed for a gesture alone: let go) */
        garm = 0;
        say("REC OFF", 900);
        return 3;
    }
    if (loops_full) {
        garm = (uint8_t)(l + 1u);
        say("ARMED G", 1200);
        return 2;
    }
    return 0;
}
static int gest_armed(uint32_t l) { return garm == l + 1u; }
static int gest_rec_track(uint32_t l) { return grec.on && grec.l == l ? (int)grec.k + 1 : 0; }
static void gest_reset(void)                    /* (a session loaded or begun: none) */
{
    garm = 0;
    memset(gl, 0, sizeof gl);
    gev_used = 0;
    grec.on = 0;
    gpad.k = -1;
}

/* the OVERDUBS page: gesture pad k (0 .. 2) of the chosen track pressed */
static void gest_pad_down(uint32_t k)
{
    uint32_t l = ui.sel;
    if (k >= GL_N)
        return;
    if (grec.on) {                               /* recording: ended (another pad: that one, then press again) */
        gest_rec_end();
        return;
    }
    if (!gest_has(l, k)) {                       /* empty: recording, from now */
        gest_start(l, k);
        return;
    }
    gpad.k = (int8_t)k;                          /* recorded: on or off when let go soon, emptied when held */
    gpad.l = (uint8_t)l;
    gpad.done = 0;
    gpad.t0 = fm1_ms;
}

/* the main loop, every pass: a recorded pad held or let go; each layer a step on, its events given as they come */
static void gest_tick(void)
{
    uint32_t now = fm1_ms, dt = now - gtick_at, l, k;
    gtick_at = now;
    if (gpad.k >= 0) {
        char m[16] = "G";
        m[1] = (char)('1' + gpad.k);
        m[2] = 0;
        if (smk_pad_on[8u + 5u + (uint32_t)gpad.k] && !gpad.done && (uint32_t)(now - gpad.t0) >= 800u) {
            gpad.done = 1;
            gest_empty(gpad.l, (uint32_t)gpad.k);
            flash(m, "EMPTY", LAYER_COL[gpad.l]);
            ui.force = 1;
        } else if (!smk_pad_on[8u + 5u + (uint32_t)gpad.k]) {
            if (!gpad.done && gest_has(gpad.l, (uint32_t)gpad.k)) {
                glayer_t *g = &gl[gpad.l][gpad.k];
                g->on = (uint8_t)!g->on;
                str_cpy(m + 2, g->on ? " ON" : " OFF", 5);
                say(m, 900);
                ui.force = 1;
            }
            gpad.k = -1;
        }
    }
    if (dt > 200u)
        dt = 200u;                               /* (a long pass -- a save: not a leap) */
    for (l = 0; l < NLAYER; l++) {
        const looper_t *lp = &loop[l];
        int run = !lp->ntrk || lp->play, start = run && lp->ntrk && !gplay_was[l];
        gplay_was[l] = (uint8_t)(lp->ntrk && lp->play);
        for (k = 0; k < GL_N; k++) {
            glayer_t *g = &gl[l][k];
            if (!g->len || !g->n || gest_rec_on(l, k))
                continue;
            if (start) {                         /* (the loops started: from its beginning) */
                g->pos = 0;
                g->ix = 0;
            }
            if (!g->on || !run)
                continue;
            g->pos += dt;
            while (1) {
                while (g->ix < g->n && GE_T(gev[g->first + g->ix]) <= g->pos) {
                    uint32_t e = gev[g->first + g->ix];
                    gest_apply(l, GE_P(e), GE_V(e));
                    g->ix++;
                }
                if (g->pos < g->len)
                    break;
                g->pos -= g->len;                /* round: from its beginning again */
                g->ix = 0;
            }
        }
    }
}

/* ------------------------------------------------------ the session's --- */
static void ses_out(const void *src, uint32_t n);   /* (sessions.c) */
static void ses_in(void *dst, uint32_t n);
/* each track's layers: on, length, how many events, then the events (format 33) */
static void gest_emit(void)
{
    uint32_t l, k;
    for (l = 0; l < NLAYER; l++)
        for (k = 0; k < GL_N; k++) {
            const glayer_t *g = &gl[l][k];
            uint32_t h[2];
            uint32_t n = gest_rec_on(l, k) ? 0u : g->n;   /* (one being recorded: not kept) */
            h[0] = gest_rec_on(l, k) ? 0u : g->len;
            h[1] = n | (uint32_t)g->on << 16;
            ses_out(h, sizeof h);
            ses_out(gev + g->first, n * 4u);
        }
}
static void gest_load(void)
{
    uint32_t l, k, i;
    gest_reset();
    for (l = 0; l < NLAYER; l++)
        for (k = 0; k < GL_N; k++) {
            glayer_t *g = &gl[l][k];
            uint32_t h[2], n;
            ses_in(h, sizeof h);
            n = h[1] & 0xFFFFu;
            g->first = (uint16_t)gev_used;
            for (i = 0; i < n; i++) {
                uint32_t e;
                ses_in(&e, 4u);
                if (h[0] && gev_used < GEV_N && GE_P(e) < GP_N) {
                    gev[gev_used++] = e;
                    g->n++;
                }
            }
            g->len = h[0] > GL_TMAX ? GL_TMAX : h[0];
            g->on = (uint8_t)((h[1] >> 16 & 1u) && g->len);
        }
}
