#!/usr/bin/env python3
"""DX7 research: each candidate family's voices picked from the collection by the idiom it is named after, and
what the recipe learns from them -- a table of skeletons (algorithm, feedback, each operator's frequency and how
loud it is: off / under / full), the envelopes' and levels' ranges for carriers and modulators (10th to 90th
percentile), the LFO's habits, and for the pitch families a table of pitch-envelope shapes. Writes the C tables
between the markers in research/dx7/fm_dx7.c and a summary for the report.

    python3 -I research/dx7/learn.py build/dx7 research/dx7/fm_dx7.c > research/dx7/learned.txt"""
import sys, os, json, collections, re
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dx7lib import GR, ratio, kmeans  # noqa: E402

D, CFILE = sys.argv[1], sys.argv[2]
J = json.load(open(os.path.join(D, 'all.json')))
R, S = J['rows'], J['S']
V = np.frombuffer(open(os.path.join(D, 'voices155.bin'), 'rb').read(), dtype=np.uint8).reshape(-1, 155).astype(int)
COLS = ['gain', 'bright', 'peak', 't90', 'tpk', 'l002', 'l015', 'l05', 'l1', 'l2', 'l38', 'r03', 'r10',
        'c0', 'ih0', 'p0', 'c1', 'ih1', 'p1', 'c2', 'ih2', 'p2', 'am']
ci = {c: j for j, c in enumerate(COLS)}
s = lambda i, c: S[str(i)][ci[c]]
OPS = lambda i: [V[i][k * 21:(k + 1) * 21] for k in range(6)]        # k: 0 = the sixth operator
alive = [r['i'] for r in R if s(r['i'], 'peak') > -60 and s(r['i'], 'c0') > 0]
nonint = lambda x: abs(x * 2 - round(x * 2)) > 0.04

# ---------------------------------------------------------------- the idioms
def shaper(i):        # a carrier nailed to 1 .. 19 Hz, turned by a modulator that follows the key
    car, tgt, fb = GR[V[i][134] & 31]; O = OPS(i)
    return any(car[k] and O[k][17] and O[k][16] > 70 and ratio(O[k])[1] < 20 and
               any(tgt[m] == k and not O[m][17] and O[m][16] > 60 for m in range(6)) for k in range(6))
def ladder(i):        # four or more carriers on uneven ratios
    car, tgt, fb = GR[V[i][134] & 31]; O = OPS(i)
    rs = sorted(ratio(O[k])[0] for k in range(6) if car[k] and not O[k][17] and O[k][16] > 70)
    return len(rs) >= 4 and (np.abs(np.diff(rs) / rs[0]) > 0.005).sum() >= 2 and any(nonint(x) for x in rs)
def dive(i):          # the pitch gliding more than an octave and a half, audibly (rate 20 .. 96) and soon (no slow
    pr, pl = V[i][126:130], V[i][130:134]   # stage before it); the held pitch within an octave of the note
    if not 18 <= pl[2] <= 82:               # (levels: 50 the note, 18 and 82 an octave off, 0 and 99 four)
        return False
    prev = pl[3]
    for j in range(3):
        d = abs(pl[j] - prev)
        if d >= 30 and (prev <= 12 or prev >= 88 or pl[j] <= 12 or pl[j] >= 88):
            return 20 <= pr[j] <= 96
        if d >= 3 and pr[j] < 20:
            return False
        prev = pl[j]
    return False
def reverse(i):       # swelling until the key is let go, then cut
    return s(i, 't90') > 2.0 and s(i, 'r10') - s(i, 'l38') < -30
def creep(i):         # a slow stage (rate <= 12) moving the pitch, every level within about an octave of the note
    pr, pl = V[i][126:130], V[i][130:134]
    prev = [pl[3], pl[0], pl[1]]
    return all(14 <= x <= 86 for x in pl) and any(pr[j] <= 12 and abs(pl[j] - prev[j]) >= 3 for j in range(3))
def clang(i):         # a modulator on a ratio off the harmonics of its carrier, the carrier dying
    if max(s(i, 'ih0'), s(i, 'ih1')) < 0.3:
        return False
    car, tgt, fb = GR[V[i][134] & 31]; O = OPS(i)
    for m in range(6):
        t = tgt[m]
        if t is not None and car[t] and not O[m][17] and not O[t][17] and O[m][16] >= 70:
            q = ratio(O[m])[0] / ratio(O[t])[0]       # (well off the harmonics: a slight offset only beats)
            if q >= 0.3 and abs(q * 2 - round(q * 2)) >= 0.2 and O[t][6] < 40:
                return True
    return False
FAM = [('SHAPER', shaper), ('LADDER', ladder), ('DIVE', dive), ('REVERSE', reverse), ('CREEP', creep), ('CLANG', clang)]

# ---------------------------------------------------------------- what is learned
def code(op):         # the operator's frequency as the tables keep it: ratio x 100, or -(1 + decade x 100 + fine)
    if op[17]:
        return -(1 + (op[18] & 3) * 100 + op[19] % 100)
    c = op[18] if op[18] else 0.5
    return int(round(c * (100 + op[19])))
def lvb(ol):
    return 0 if ol < 40 else 1 if ol < 80 else 2
def skeletons(ids, n=24):
    cnt, first, ext = collections.Counter(), {}, collections.defaultdict(collections.Counter)
    for i in ids:
        O = OPS(i)
        key = (V[i][134] & 31,) + tuple(code(O[6 - k]) for k in range(1, 7))
        cnt[key] += 1
        first.setdefault(key, i)
        ext[key][(V[i][135],) + tuple(lvb(O[6 - k][16]) for k in range(1, 7))] += 1
    keys = sorted(cnt, key=lambda k: (-cnt[k], first[k]))
    out, used = [], set()
    for k in keys:                     # (no two from one bank of 32: the collection holds many edits of one voice)
        b = first[k] // 32
        if b in used and len(keys) > 3 * n:
            continue
        used.add(b)
        e = ext[k].most_common(1)[0][0]
        out.append((k[0], e[0]) + k[1:] + e[1:])
        if len(out) == n:
            break
    return out, cnt
def envelopes(ids, want_car, k=12):
    rows, rd = [], []
    for i in ids:
        car, tgt, fb = GR[V[i][134] & 31]; O = OPS(i)
        for j in range(6):
            if car[j] == want_car and O[j][16] >= 40:
                rows.append(O[j][0:8])
                rd.append([O[j][13], O[j][20]])
    X = np.array(rows, dtype=float) / 99
    L, Cn = kmeans(X, min(k, len(X)), seed=7)
    order = np.argsort(-np.bincount(L, minlength=len(Cn)))
    env = [[int(round(x * 99)) for x in Cn[j]] for j in order if (L == j).any()]
    rd = np.array(rd)
    q = lambda c, p: int(round(np.percentile(rd[:, c], p)))
    return env, (q(0, 10), q(0, 90), q(1, 10), q(1, 90)), len(rows)
def lfo(ids):
    P = V[ids]
    w = np.bincount(P[:, 142], minlength=6)[:6]
    pit = P[:, 139] * P[:, 143] > 40
    amp = P[:, 140] > 10
    q = lambda x, p: int(round(np.percentile(x, p))) if len(x) else 0
    return dict(w=[int(round(x * 99 / w.sum())) for x in w], lfs=(q(P[:, 137], 10), q(P[:, 137], 90)),
                ppit=int(round(pit.mean() * 100)), pmd=(q(P[pit, 139], 10), q(P[pit, 139], 90)), pms=(q(P[pit, 143], 10), q(P[pit, 143], 90)),
                pamp=int(round(amp.mean() * 100)), amd=(q(P[amp, 140], 10), q(P[amp, 140], 90)),
                fb=[int(x) for x in np.bincount(P[:, 135], minlength=8)[:8] * 99 // len(ids)])
def pegs(ids, n=24):
    cnt, first = collections.Counter(), {}
    for i in ids:
        k = tuple(V[i][126:134])
        cnt[k] += 1
        first.setdefault(k, i)
    return sorted(cnt, key=lambda k: (-cnt[k], first[k]))[:n], cnt

C, summary = [], {}
for name, f in FAM:
    ids = [i for i in alive if f(i)]
    sk, cnt = skeletons(ids)
    ce, crd, cn = envelopes(ids, True)
    me, mrd, mn = envelopes(ids, False)
    L = lfo(ids)
    lc = name.lower()
    summary[name] = dict(n=len(ids), skeletons=len(cnt), kept=len(sk), car=ce, mod=me, scale=(crd, mrd), lfo=L,
                         ids=ids[:: max(1, len(ids) // 6)][:6])
    C.append(f'/* {name}: {len(ids)} voices of the collection; {len(cnt)} skeletons, the {len(sk)} kept */')
    C.append(f'static const dx7_skel_t DX_{name}[{len(sk)}] = {{')
    for k in sk:
        lv = sum(b << (2 * j) for j, b in enumerate(k[8:14]))
        C.append('    {%d, %d, 0x%03x, {%s}},' % (k[0], k[1], lv, ', '.join(str(x) for x in k[2:8])))
    C.append('};')
    for nm, E in (('C', ce), ('M', me)):
        C.append(f'static const uint8_t D{nm}_{name}[{len(E)}][8] = {{   /* {"carrier" if nm == "C" else "modulator"} envelopes: R1..R4, L1..L4 (centroids, the most common first) */')
        for e in E:
            C.append('    {%s},' % ', '.join(map(str, e)))
        C.append('};')
    C.append(f'static const dx7_fam_t DF_{name} = {{')
    C.append('    %s, %s,   /* RS low, high; DET low, high: carriers, then modulators */' % (', '.join(map(str, crd)), ', '.join(map(str, mrd))))
    C.append('    {%s}, {%s}, %d, %d, %d, %d, %d, %d, %d, %d, %d,' % (', '.join(map(str, L['fb'])), ', '.join(map(str, L['w'])),
             L['lfs'][0], L['lfs'][1], L['ppit'], L['pmd'][0], L['pmd'][1], L['pms'][0], L['pms'][1], L['pamp'], L['amd'][0]))
    C.append('    %d, DX_%s, DC_%s, DM_%s, %d, %d, %d};' % (L['amd'][1], name, name, name, len(sk), len(ce), len(me)))
    if name in ('DIVE', 'CREEP'):
        pg, pc = pegs(ids)
        summary[name]['pegs'] = len(pc)
        C.append(f'static const uint8_t DP_{name}[{len(pg)}][8] = {{   /* pitch envelopes: R1..R4, L1..L4 ({len(pc)} seen) */')
        for k in pg:
            C.append('    {%s},' % ', '.join(map(str, k)))
        C.append('};')
    if name == 'LADDER':       # how its carriers' envelopes are staggered: entering one by one, dying one by one
        ent = dec = 0
        for i in ids:
            car, tgt, fb = GR[V[i][134] & 31]; O = OPS(i)
            r1 = [O[k][0] for k in range(6) if car[k] and O[k][16] > 70]
            r2 = [O[k][1] for k in range(6) if car[k] and O[k][16] > 70]
            ent += max(r1) - min(r1) >= 15
            dec += max(r2) - min(r2) >= 15
        summary[name]['stagger'] = (ent * 100 // len(ids), dec * 100 // len(ids))
        C.append(f'#define DX_LADDER_ENTER {ent * 100 // len(ids)}   /* % of them whose carriers enter one by one */')
        C.append(f'#define DX_LADDER_DIE {dec * 100 // len(ids)}     /* .. die one by one */')
    C.append('')

src = open(CFILE).read()
a, b = src.index('/* BEGIN LEARNED'), src.index('/* END LEARNED */')
a = src.index('\n', a) + 1
open(CFILE, 'w').write(src[:a] + '\n'.join(C) + src[b:])
json.dump(summary, open(os.path.join(D, 'learned.json'), 'w'), indent=1, default=int)
for k, v in summary.items():
    print(k, {x: y for x, y in v.items() if x not in ('car', 'mod')})
    print('   scale', v['scale'], '\n   car env', v['car'][:4], '\n   mod env', v['mod'][:4])
