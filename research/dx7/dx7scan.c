/* SPDX-License-Identifier: GPL-3.0-only
 * DX7 research: every voice of a 155-byte voice file rendered through the firmware's own FM core (fm6_core.c, via
 * fm.c's fm_render) and measured. Built on the host harness, as tests/golden is:
 *   clang -O2 -std=gnu99 -w -Ifirmware/src -Ifirmware/hal -Ibuild/gen research/dx7/dx7scan.c -o build/dx7scan
 *   build/dx7scan voices155.bin FIRST COUNT > part.csv
 * One line a voice: middle C held 4 s, released for 2 s; the level measured as fm_measure gives it. */
#define main host_main
#include "../../tests/host/host.c"
#undef main

#define SR 44100
#define HOLD_S 4
#define TAIL_S 2
#define NS ((HOLD_S + TAIL_S) * SR)
static float snd[NS + 8192];

static double rms_at(double t, uint32_t w)              /* the RMS of w samples from t seconds, dB re full scale */
{
    uint32_t a = (uint32_t)(t * SR), i;
    double e = 0;
    for (i = 0; i < w && a + i < NS; i++)
        e += (double)snd[a + i] * snd[a + i];
    return 10.0 * log10(e / w + 1e-9);
}
/* a 8192-point spectrum from t: the centroid (Hz) and how much of the energy (60 Hz .. 10 kHz) lies off the grid of
 * the half fundamental (+-12 Hz): 0 harmonic, ~0.8 noise or a bell */
static void spec_at(double t, double f0, double *cent, double *inh, double *pow)
{
    static double re[8192], im[8192];
    uint32_t a = (uint32_t)(t * SR), k, lo = 60 * 8192 / SR, hi = 10000 * 8192 / SR;
    double on = 0, all = 0, cw = 0, h = f0 / 2;
    for (k = 0; k < 8192u; k++) {
        re[k] = snd[a + k] * (0.5 - 0.5 * cos(6.283185307179586 * k / 8192));
        im[k] = 0;
    }
    fft(re, im, 8192u);
    for (k = lo; k < hi; k++) {
        double f = (double)k * SR / 8192, pw = re[k] * re[k] + im[k] * im[k], m = fmod(f, h);
        all += pw;
        cw += pw * f;
        if (m < 12 || h - m < 12)
            on += pw;
    }
    *pow = 10.0 * log10(all / 4096.0 + 1e-9);
    *cent = all > 0 ? cw / all : 0;
    *inh = all > 0 ? 1.0 - on / all : 0;
}

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "rb");
    uint32_t first = (uint32_t)atoi(argv[2]), count = (uint32_t)atoi(argv[3]), v;
    static fm_patch_t pt;
    if (!f)
        return 1;
    for (v = first; v < first + count; v++) {
        fm_voice_t fv;
        int32_t out[CTL], g;
        uint32_t b, i, n = 0, nb = NS / CTL, off = HOLD_S * SR / CTL, k;
        double pk = -200, tpk = 0, t90 = -1, lv[8], c[3], ih[3], pw[3], am = 0, f0, br;
        static const double T[8] = {0.02, 0.15, 0.5, 1.0, 2.0, 3.8, 4.3, 5.0};
        memset(&pt, 0, sizeof pt);
        if (fseek(f, (long)v * 155, SEEK_SET) || fread(pt.p, 1, 155, f) != 155)
            break;
        fm6_sanitize(pt.p);
        pt.family = FF_WARP;
        pt.gen = GEN_NOW;
        br = fm_measure(&pt);
        g = pt.gain;
        memset(&fv, 0, sizeof fv);
        fm_note_on(&fv, &pt, 60, 1);
        for (b = 0; b < nb; b++) {
            if (b == off)
                fm_note_off(&fv, &pt);
            if (!fm_render(&fv, &pt, out, 0))
                memset(out, 0, sizeof out);
            for (i = 0; i < CTL; i++)
                snd[n++] = (float)out[i] / 32768.0f;
        }
        memset(snd + n, 0, 8192 * sizeof(float));
        {   /* the loudness curve: 512-sample windows */
            double w[NS / 512], prev = 0;
            uint32_t nw = HOLD_S * SR / 512, m = 0;
            for (k = 0; k < nw; k++) {
                w[k] = rms_at(k * 512.0 / SR, 512);
                if (w[k] > pk)
                    pk = w[k], tpk = k * 512.0 / SR;
            }
            for (k = 0; k < nw; k++)
                if (w[k] >= pk - 1.0) {
                    t90 = k * 512.0 / SR;
                    break;
                }
            for (k = (uint32_t)(1.0 * SR / 512); k < nw; k++) {   /* movement: how the level jitters, held (dB a window) */
                if (k > (uint32_t)(1.0 * SR / 512) && w[k] > pk - 50)
                    am += fabs(w[k] - prev), m++;
                prev = w[k];
            }
            am = m ? am / m : 0;
        }
        for (k = 0; k < 8u; k++)
            lv[k] = rms_at(T[k], 2048) - pk;
        f0 = 440.0 * pow(2.0, (60 + (int)pt.p[FP_TRNSP] - 24 - 69) / 12.0);
        spec_at(0.08, f0, &c[0], &ih[0], &pw[0]);
        spec_at(0.6, f0, &c[1], &ih[1], &pw[1]);
        spec_at(3.0, f0, &c[2], &ih[2], &pw[2]);
        printf("%u,%d,%d,%.1f,%.3f,%.3f", v, g, (int)br, pk, t90, tpk);
        for (k = 0; k < 8u; k++)
            printf(",%.1f", lv[k]);
        for (k = 0; k < 3u; k++)
            printf(",%.0f,%.3f,%.1f", c[k], ih[k], pw[k]);
        printf(",%.3f\n", am);
        fflush(stdout);
    }
    return 0;
}
