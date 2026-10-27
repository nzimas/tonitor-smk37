/* SPDX-License-Identifier: GPL-3.0-only
 * TONITOR: the gesture loops -- eight a track, apart from the note loops altogether. The sixth button of the top row
 * puts them on the bottom row of pads (GESTURES; again: the pads freed). A gesture loop is the knobs and faders as
 * they were moved, kept as events, played back round and round at its own length, whatever else plays -- held notes,
 * loops, sequencers: it knows nothing of them, and drifts against them.
 *
 *   a pad held 0.8 s          armed (it blinks red; ARMED): what was in it, emptied
 *   then a knob or a fader    recording, from that move (red): every move of what follows kept
 *   the pad pressed           the end: as long as from that first move to now; it loops at once, on
 *   a recorded pad pressed    on or off (on: from its beginning)
 *   an armed pad pressed      armed no more
 *
 * What a gesture loop keeps: the chosen track's filter, granulator, crush, distortion, delay and reverb sends, level,
 * pan and the eight macros of the sound it plays (the track it was armed on); and faders 2 and 3, SPACE and the
 * master filter. What it plays sets those values (as the hand would: the hand may move them between its events, and
 * a fader's own position is left behind until it moves again). One gesture loop is armed or records at a time. The
 * events of all of them share one store (GEV_N); one emptied, the others close up. Sessions keep them (sessions.c,
 * format 34); snapshots and the clipboard do not. All in the main loop */

#define GL_N 8u                  /* gesture loops a track (the bottom row's eight pads) */
#define GEV_N 1024u              /* the events of all of them between them (4 KiB, the pool -- all it has room for) */
#define GP_N (GP_MFLT + 1u)      /* the controls (GP_*: ui.c) */
/* an event: its time from the loop's start (ms, 19 bits: up to 8.7 minutes), the control (5 bits), its value (8) */
#define GE(t, p, v) (((uint32_t)(t) & 0x7FFFFu) | (uint32_t)(p) << 19 | (uint32_t)(uint8_t)(int8_t)(v) << 24)
#define GE_T(e) ((e) & 0x7FFFFu)
#define GE_P(e) ((e) >> 19 & 31u)
#define GE_V(e) ((int8_t)(uint8_t)((e) >> 24))
#define GL_TMAX 0x7FFFFu
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
    uint8_t armed, on, l, k, full;   /* track l's loop k armed, or recording (its events the last in gev[]) */
    uint32_t t0;
} grec;
static struct {
    int8_t k;                    /* a pad down (its loop, -1 none), since t0; held: done */
    uint8_t l, done;
    uint32_t t0;
} gpad = {-1, 0, 0, 0};
static uint32_t gtick_at;

static int gest_has(uint32_t l, uint32_t k) { return gl[l][k].len != 0u; }
static int gest_rec_on(uint32_t l, uint32_t k) { return grec.on && grec.l == l && grec.k == k; }
static int gest_armed_on(uint32_t l, uint32_t k) { return grec.armed && grec.l == l && grec.k == k; }
static int gest_is_on(uint32_t l, uint32_t k) { return gl[l][k].on; }
static int gest_rec_track(uint32_t l) { return grec.on && grec.l == l ? (int)grec.k + 1 : 0; }
static int gest_armed(uint32_t l) { return grec.armed && grec.l == l; }

/* a control's value given back: the track's knob value (or a fader's), then to the ISR */
static void gest_apply(uint32_t l, uint32_t p, int32_t v)
{
    uint32_t i;
    if (p >= GP_MAC0 && p < GP_MAC0 + NMAC) {
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
    case GP_PAN: ui.pan[l] = (int8_t)clampi(v, -PAN_MAX, PAN_MAX); break;
    case GP_SPACE:                               /* (fader 2: every track's sends, the decay, the repeats) */
        fd_space = (uint8_t)clampi(v, 0, 100);
        for (i = 0; i < NLAYER; i++)
            level_pan_apply(i);
        rv_apply();
        dl_apply();
        return;
    default:                                     /* (fader 3: the master filter) */
        mflt_v = (int8_t)clampi(v, -MF_MAX, MF_MAX);
        return;
    }
    level_pan_apply(l);
}

/* the knobs and the faders (ui.c): control p turned to v (of track l; the faders' -- l any -- are kept by whichever
 * loop records). The first move after arming starts the recording; moves 15 ms apart or less: the last one's value */
static void gest_rec(uint32_t l, uint32_t p, int32_t v)
{
    glayer_t *g;
    uint32_t t;
    int global = p >= GP_SPACE;
    if (grec.armed && (global || l == grec.l)) {   /* the first move: recording, from now */
        grec.armed = 0;
        grec.on = 1;
        grec.t0 = fm1_ms;
        grec.full = 0;
        gl[grec.l][grec.k].first = (uint16_t)gev_used;
        gl[grec.l][grec.k].n = 0;
        ui.force = 1;
    }
    if (!grec.on || (!global && grec.l != l))
        return;
    g = &gl[grec.l][grec.k];
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
/* track l's loop k emptied: its events out of the store, the others' moved up */
static void gest_empty(uint32_t l, uint32_t k)
{
    glayer_t *g = &gl[l][k];
    uint32_t f = g->first, n = g->n, i, j;
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
static void gest_rec_end(void)                  /* the recording ended: it loops, on */
{
    glayer_t *g = &gl[grec.l][grec.k];
    uint32_t t = fm1_ms - grec.t0;
    char m[8] = "G";
    g->len = t < 100u ? 100u : t > GL_TMAX ? GL_TMAX : t;
    g->on = 1;
    g->ix = 0;
    g->pos = 0;
    grec.on = 0;
    fmt_int(m + 1, grec.k + 1);
    say(m, 1200);
    ui.force = 1;
}
/* track l's loop k armed: anything armed or recording before, let go (or ended); what was in it, emptied */
static void gest_arm(uint32_t l, uint32_t k)
{
    if (grec.on)
        gest_rec_end();
    grec.armed = 0;
    if (gl[l][k].len || gl[l][k].n)
        gest_empty(l, k);
    grec.armed = 1;
    grec.l = (uint8_t)l;
    grec.k = (uint8_t)k;
    say("ARMED", 1200);
    ui.force = 1;
}
static void gest_reset(void)                    /* (a session loaded or begun: none) */
{
    memset(gl, 0, sizeof gl);
    gev_used = 0;
    grec.on = grec.armed = 0;
    gpad.k = -1;
}

/* the GESTURES page: pad k (0 .. 7) of the chosen track pressed */
static void gest_pad_down(uint32_t k)
{
    if (k >= GL_N)
        return;
    gpad.k = (int8_t)k;
    gpad.l = (uint8_t)ui.sel;
    gpad.done = 0;
    gpad.t0 = fm1_ms;
}

/* the main loop, every pass: a pad held or let go; each gesture loop a step on, its events given as they come */
static void gest_tick(void)
{
    uint32_t now = fm1_ms, dt = now - gtick_at, l, k;
    gtick_at = now;
    if (gpad.k >= 0) {
        uint32_t pk = (uint32_t)gpad.k, pl = gpad.l;
        if (smk_pad_on[8u + pk] && !gpad.done && (uint32_t)(now - gpad.t0) >= 800u) {
            gpad.done = 1;                       /* held: armed */
            gest_arm(pl, pk);
        } else if (!smk_pad_on[8u + pk]) {
            if (!gpad.done) {                    /* let go soon */
                char m[12] = "G";
                fmt_int(m + 1, (int32_t)pk + 1);
                if (gest_rec_on(pl, pk))
                    gest_rec_end();              /* recording: its end, and it loops */
                else if (gest_armed_on(pl, pk)) {
                    grec.armed = 0;              /* armed, nothing yet: no more */
                    say("NOT ARMED", 900);
                } else if (gest_has(pl, pk)) {
                    glayer_t *g = &gl[pl][pk];
                    g->on = (uint8_t)!g->on;
                    g->pos = 0;                  /* (on: from its beginning) */
                    g->ix = 0;
                    str_cpy(m + str_len(m), g->on ? " ON" : " OFF", 5);
                    say(m, 900);
                } else
                    say("HOLD TO ARM", 1200);
                ui.force = 1;
            }
            gpad.k = -1;
        }
    }
    if (dt > 200u)
        dt = 200u;                               /* (a long pass -- a save: not a leap) */
    for (l = 0; l < NLAYER; l++)
        for (k = 0; k < GL_N; k++) {
            glayer_t *g = &gl[l][k];
            if (!g->len || !g->n || !g->on || gest_rec_on(l, k))
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

/* ------------------------------------------------------ the session's --- */
static void ses_out(const void *src, uint32_t n);   /* (sessions.c) */
static void ses_in(void *dst, uint32_t n);
/* each track's gesture loops: length, how many events and on, then the events (format 34; 33: three a track, the
 * controls 4 bits) */
static void gest_emit(void)
{
    uint32_t l, k;
    for (l = 0; l < NLAYER; l++)
        for (k = 0; k < GL_N; k++) {
            const glayer_t *g = &gl[l][k];
            uint32_t h[2], n = gest_rec_on(l, k) ? 0u : g->n;   /* (one being recorded: not kept) */
            h[0] = gest_rec_on(l, k) ? 0u : g->len;
            h[1] = n | (uint32_t)g->on << 16;
            ses_out(h, sizeof h);
            ses_out(gev + g->first, n * 4u);
        }
}
static void gest_load(uint32_t format)
{
    uint32_t l, k, i, nk = format >= 34u ? GL_N : 3u;
    gest_reset();
    for (l = 0; l < NLAYER; l++)
        for (k = 0; k < nk; k++) {
            glayer_t *g = &gl[l][k];
            uint32_t h[2], n;
            ses_in(h, sizeof h);
            n = h[1] & 0xFFFFu;
            g->first = (uint16_t)gev_used;
            for (i = 0; i < n; i++) {
                uint32_t e;
                ses_in(&e, 4u);
                if (format < 34u)                /* (format 33: 20-bit times, 4-bit controls) */
                    e = GE(e & 0xFFFFFu, e >> 20 & 15u, (int8_t)(uint8_t)(e >> 24));
                if (h[0] && gev_used < GEV_N && GE_P(e) < GP_N) {
                    gev[gev_used++] = e;
                    g->n++;
                }
            }
            g->len = h[0] > GL_TMAX ? GL_TMAX : h[0];
            g->on = (uint8_t)((h[1] >> 16 & 1u) && g->len);
        }
}
