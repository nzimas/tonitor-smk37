/* SPDX-License-Identifier: GPL-3.0-only
 * DX7 research: the audition. Built on the host harness against a tree with generation 6 wired in (research/dx7/
 * audition.sh makes it), it renders eight sounds of each new FM family exactly as the firmware would draw them
 * (fm_generate on GEN_NOW, fm_measure, a layer's gain and limiter) -- a note held 6 s, released for 4 s -- and,
 * beside them, a few of the collection's own voices each family learned from, through the same core.
 *   audition OUTDIR [voices155.bin learned-ids...]  */
#define main host_main
#include "../../tests/host/host.c"
#undef main

static const char *note_name(int n)
{
    static char b[8];
    static const char *const N[12] = {"C", "Cs", "D", "Ds", "E", "F", "Fs", "G", "Gs", "A", "As", "B"};
    snprintf(b, sizeof b, "%s%d", N[n % 12], n / 12 - 1);
    return b;
}

int main(int argc, char **argv)
{
    static const uint32_t PATH[8] = {PATH_OPEN, PATH_OPEN, PATH_AMBIENT, PATH_AMBIENT, PATH_EXP, PATH_EXP, PATH_OPEN, PATH_AMBIENT};
    static const int NOTE[8] = {60, 48, 55, 43, 64, 72, 36, 67};
    static const char *const PN[PATH_N] = {"open", "ambient", "exp"};
    char path[600];
    int32_t f, i;
    wav_ch = 1;
    printf("%-8s %2s %-7s %-4s  %6s %6s %5s  %s\n", "family", "#", "path", "note", "peak", "rmsmax", "clip", "patch");
    for (f = FF_N5; f < FF_N; f++)
        for (i = 0; i < 8; i++) {
            static fm_patch_t pt;
            uint32_t seed = 9000u + (uint32_t)f * 101u + (uint32_t)i * 7919u, k;
            stats_t st;
            host_path = PATH[i];
            st = render_sound(ENG_FM, f, seed, NOTE[i], 6.0, 4.0, 1);
            snprintf(path, sizeof path, "%s/%s-%d-%s-%s.wav", argv[1], FF_NAME[f], i + 1, PN[PATH[i]], note_name(NOTE[i]));
            wav_write(path);
            fm_generate(&pt, f, seed, PATH[i], GEN_NOW);      /* (the same patch again, to print it) */
            printf("%-8s %2d %-7s %-4s  %6.1f %6.1f %5u  alg%d fb%d", FF_NAME[f], i + 1, PN[PATH[i]], note_name(NOTE[i]),
                   dbp(st.peak), db(st.rms_max), st.clip, pt.p[FP_ALG] + 1, pt.p[FP_FB]);
            for (k = 1; k <= 6; k++) {
                const uint8_t *op = fm_op(pt.p, (int)k);
                int32_t x = fm_op_ratio100(op);
                printf(" %c%u:%s%d.%02d@%d", fm_is_carrier(pt.p, (int)k) ? 'C' : 'm', k, op[FP_MODE] ? "f" : "",
                       x / 100, x % 100, op[FP_OL]);
            }
            printf(" PEG %d %d %d %d / %d %d %d %d\n", pt.p[FP_PR1], pt.p[FP_PR1 + 1], pt.p[FP_PR1 + 2], pt.p[FP_PR1 + 3],
                   pt.p[FP_PL1], pt.p[FP_PL1 + 1], pt.p[FP_PL1 + 2], pt.p[FP_PL1 + 3]);
        }
    if (argc > 3) {                                      /* the collection's own: FAMILY:index ... */
        FILE *fv = fopen(argv[2], "rb");
        for (i = 3; i < argc && fv; i++) {
            static fm_patch_t pt;
            fm_voice_t fv0;
            int32_t out[CTL], lim_env = 0, lim_g = Q15;
            uint32_t b, j, v = (uint32_t)atoi(strchr(argv[i], ':') + 1);
            memset(&pt, 0, sizeof pt);
            if (fseek(fv, (long)v * 155, SEEK_SET) || fread(pt.p, 1, 155, fv) != 155)
                continue;
            fm6_sanitize(pt.p);
            pt.family = FF_WARP;
            fm_measure(&pt);
            memset(&fv0, 0, sizeof fv0);
            fm_note_on(&fv0, &pt, 60, 1);
            for (b = 0; b < (uint32_t)(10.0 * 44100 / CTL); b++) {
                if (b == (uint32_t)(6.0 * 44100 / CTL))
                    fm_note_off(&fv0, &pt);
                if (!fm_render(&fv0, &pt, out, 0))
                    memset(out, 0, sizeof out);
                for (j = 0; j < CTL; j++)
                    out[j] = mulq12(out[j], LAYER_GAIN);
                lim_block(&lim_env, &lim_g, out, CTL);
                for (j = 0; j < CTL; j++)
                    wav_put(out[j]);
            }
            *strchr(argv[i], ':') = 0;
            snprintf(path, sizeof path, "%s/sources/%s-voice%u-C4.wav", argv[1], argv[i], v);
            wav_write(path);
        }
    }
    return 0;
}
