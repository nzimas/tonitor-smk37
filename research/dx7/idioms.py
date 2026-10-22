#!/usr/bin/env python3
"""DX7 research: how often the idioms the recipes come from appear, and their skeletons.
python3 -I research/dx7/idioms.py build/dx7 > research/dx7/idioms.txt"""
import sys, os, json, collections
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dx7lib import GR, ratio  # noqa: E402
D = sys.argv[1]
J = json.load(open(os.path.join(D, 'all.json')))
R, S = J['rows'], J['S']
V = np.frombuffer(open(os.path.join(D, 'voices155.bin'), 'rb').read(), dtype=np.uint8).reshape(-1, 155).astype(int)
COLS = ['gain', 'bright', 'peak', 't90', 'tpk', 'l002', 'l015', 'l05', 'l1', 'l2', 'l38', 'r03', 'r10',
        'c0', 'ih0', 'p0', 'c1', 'ih1', 'p1', 'c2', 'ih2', 'p2', 'am']
ci = {c: j for j, c in enumerate(COLS)}
s = lambda i, c: S[str(i)][ci[c]]
ops = lambda i: [V[i][k * 21:(k + 1) * 21] for k in range(6)]
alive = [r['i'] for r in R if s(r['i'], 'peak') > -60 and s(r['i'], 'c0') > 0]
print(f'{len(alive)} sounding voices')
fbh = collections.Counter(V[alive, 135]); print('feedback histogram', sorted(fbh.items()))

def show(title, ids, n=12):
    print(f'\n== {title}: {len(ids)}')
    for i in ids[:: max(1, len(ids) // n)][:n]:
        p = V[i]; a = p[134] & 31; car, tgt, fb = GR[a]
        o = []
        for k in range(6):
            op = p[k * 21:(k + 1) * 21]; rr, hz = ratio(op)
            o.append(('C' if car[k] else 'm') + str(6 - k) + ':' + (('%.0fHz' % hz) if hz else '%.2f' % rr) + '@' + str(op[16])
                     + '>' + (str(6 - tgt[k]) if tgt[k] is not None else 'out') + ' E' + ','.join(map(str, op[0:8])))
        print(f'  {i} alg{a + 1} fb{p[135]} PEG R{list(p[126:130])} L{list(p[130:134])} LFO w{p[142]} s{p[137]} d{p[138]} pmd{p[139]}x{p[143]} amd{p[140]}'
              f' | t90 {s(i, "t90"):.2f} l38 {s(i, "l38"):.0f} rel {s(i, "r10") - s(i, "l38"):.0f} ih {max(s(i, "ih0"), s(i, "ih1")):.2f} c1 {s(i, "c1"):.0f}')
        for x in reversed(o):
            print('      ' + x)

# 1. carriers nailed to a fixed sub-audio frequency (< 20 Hz) and modulated by key-following operators: wave shaping
sh = []
for i in alive:
    p = V[i]; car, tgt, fb = GR[p[134] & 31]; O = ops(i)
    for k in range(6):
        if car[k] and O[k][17] and O[k][16] > 70 and ratio(O[k])[1] < 20:
            if any(tgt[m] == k and not O[m][17] and O[m][16] > 60 for m in range(6)):
                sh.append(i); break
show('fixed sub-audio carrier, ratio modulators (shaper)', sh)
# 2. additive ladders: >= 4 carriers, their ratios spread unevenly / near-unison
ld = []
for i in alive:
    p = V[i]; car, tgt, fb = GR[p[134] & 31]; O = ops(i)
    rs = sorted(ratio(O[k])[0] for k in range(6) if car[k] and not O[k][17] and O[k][16] > 70)
    if len(rs) >= 4:
        st = np.diff(rs) / rs[0]
        if (np.abs(st) > 0.005).sum() >= 2 and any(abs(r * 2 - round(r * 2)) > 0.03 for r in rs):
            ld.append(i)
show('additive ladders (4+ carriers, uneven ratios)', ld)
# 3. full-range pitch envelopes: a level at <= 15 or >= 85 that is not the resting one
fr = [i for i in alive if (min(V[i][130:134]) <= 15 or max(V[i][130:134]) >= 85) and len(set(V[i][130:134])) > 1]
show('full-range pitch envelope', fr)
# 4. the level swelling to the end, then cut (reverse)
rv = [i for i in alive if s(i, 't90') > 2.0 and s(i, 'r10') - s(i, 'l38') < -30]
show('reverse: swells to the release, cut', rv)
# 5. creeping pitch: a pitch envelope rate <= 12 towards a level away from 50
cr = [i for i in alive if any(V[i][126 + j] <= 12 and abs(V[i][130 + j] - 50) >= 4 for j in range(3))]
show('creeping pitch (slow pitch envelope)', cr)
# 6. Chowning pairs: a modulator on a non-integer ratio, high level, its index dying (R2 fast, L2/L3 low), carrier decaying
cp = collections.Counter(); cpi = []
for i in alive:
    p = V[i]; car, tgt, fb = GR[p[134] & 31]; O = ops(i)
    if max(s(i, 'ih0'), s(i, 'ih1')) < 0.3:
        continue
    for m in range(6):
        t = tgt[m]
        if t is None or not car[t] or O[m][17] or O[t][17] or O[m][16] < 70:
            continue
        q = ratio(O[m])[0] / ratio(O[t])[0]
        if abs(q * 2 - round(q * 2)) > 0.04 and O[t][6] < 40:
            cp[round(q, 2)] += 1; cpi.append(i); break
print('\nChowning pairs: modulator/carrier ratios', cp.most_common(30))
show('Chowning pairs (inharmonic index, decaying)', sorted(set(cpi)))
