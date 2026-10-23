#!/usr/bin/env python3
"""DX7 research, second pass: the voices outside the conventional instruments (inharmonic, pitch-enveloped, slow to
swell, moving), clustered on sound and structure together; each cluster's skeletons and ranges listed.
python3 -I research/dx7/subset.py build/dx7 > research/dx7/subset.txt (after analyse.py)"""
import sys, os, json, collections
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dx7lib import GR, ratio, kmeans  # noqa: E402

D = sys.argv[1]
J = json.load(open(os.path.join(D, 'all.json')))
R, S = J['rows'], J['S']
V = np.frombuffer(open(os.path.join(D, 'voices155.bin'), 'rb').read(), dtype=np.uint8).reshape(-1, 155).astype(int)
COLS = ['gain', 'bright', 'peak', 't90', 'tpk', 'l002', 'l015', 'l05', 'l1', 'l2', 'l38', 'r03', 'r10',
        'c0', 'ih0', 'p0', 'c1', 'ih1', 'p1', 'c2', 'ih2', 'p2', 'am']
ci = {c: j for j, c in enumerate(COLS)}
def s(i, c):
    return S[str(i)][ci[c]]

keep = []
why = collections.Counter()
for r in R:
    i = r['i']
    if s(i, 'peak') < -60 or s(i, 'c0') == 0:
        continue                                        # silent, or gone in 80 ms
    inh = max(s(i, 'ih0'), s(i, 'ih1'))
    w = []
    if inh > 0.3: w.append('inharm')
    if r['pegmove'] > 8: w.append('peg')
    if r['lpm'] > 25: w.append('pitchlfo')
    if s(i, 't90') > 1.0: w.append('slow')
    if s(i, 'am') > 1.5: w.append('moving')
    if w:
        keep.append(i)
        why.update(w)
print(f'# {len(keep)} unconventional voices of {len(R)}: {dict(why)}')

def f(i):
    r = R[i]
    lg = lambda x: np.log10(max(x, 1))
    pl = r['pl']
    return [lg(s(i, 'c1')) * 2, lg(s(i, 'c0')) - lg(max(s(i, 'c2'), 1)), max(s(i, 'ih0'), s(i, 'ih1')) * 3,
            lg(s(i, 't90') * 1000 + 1), np.clip(s(i, 'l38'), -60, 0) / 15, np.clip(s(i, 'l05'), -60, 0) / 15,
            min(s(i, 'am'), 4), min(r['pegmove'], 50) / 20, (pl[3] - pl[2]) / 25 if r['pegmove'] else 0,
            min(r['lpm'], 99) / 30, 1.0 if r['lfw'] == 5 else 0, min(r['nfix'], 3) / 2, r['ncar'] / 3]
F = np.array([f(i) for i in keep])
Z = (F - F.mean(0)) / (F.std(0) + 1e-9)
K = int(os.environ.get('K', 18))
L, _ = kmeans(Z, K, seed=3)
np.save(os.path.join(D, 'sub_idx.npy'), np.array(keep))
np.save(os.path.join(D, 'sub_labels.npy'), L)

def q(x, p=(10, 50, 90)):
    return '/'.join('%.0f' % v for v in np.percentile(x, p))
def skel(i):
    p = V[i]
    a = p[134] & 31
    car, tgt, fbop = GR[a]
    out = []
    for k in range(6):
        op = p[k * 21:(k + 1) * 21]
        rr, hz = ratio(op)
        tag = ('%.0fHz' % hz) if hz else ('%.2f' % rr)
        out.append(('C' if car[k] else 'm') + str(6 - k) + ':' + tag + '@' + str(op[16]))
    return f'alg{a + 1} fb{p[135]} ' + ' '.join(reversed(out))
for j in sorted(range(K), key=lambda j: -(L == j).sum()):
    m = np.where(L == j)[0]
    ids = [keep[x] for x in m]
    g = lambda c: np.array([s(i, c) for i in ids])
    P = lambda k: np.array([R[i][k] for i in ids], dtype=float)
    inh = np.maximum(g('ih0'), g('ih1'))
    print(f'\n== {j}  n={len(ids)}')
    print(f'  centroid Hz 80ms {q(g("c0"))} | 0.6s {q(g("c1"))} | 3s {q(g("c2"))}; inharm {q(inh * 100)}%; t90 ms {q(g("t90") * 1000)};'
          f' dB@0.5s {q(g("l05"))} @3.8s {q(g("l38"))}; release(1s) {q(g("r10") - g("l38"))}; am {np.median(g("am")):.2f}')
    algs = collections.Counter(P('alg').astype(int))
    print(f'  algs {algs.most_common(8)}; carriers {collections.Counter(P("ncar").astype(int)).most_common(4)}; fb {q(P("fb"))};'
          f' fixed ops>0 {(P("nfix") > 0).mean() * 100:.0f}%; mod-ratio non-integer {(P("inhm") > 0).mean() * 100:.0f}%;'
          f' carrier ratios uneven {(P("cinh") > 0).mean() * 100:.0f}%')
    print(f'  carriers R1 {q(P("catt"))} L3 {q(P("csus"))} R4 {q(P("crel"))}; mods R2 {q(P("mdec"))} OL {q(P("modol"))}')
    print(f'  PEG moving {(P("pegmove") > 8).mean() * 100:.0f}% depth {q(P("pegmove"))}; LFO pitch>25 {(P("lpm") > 25).mean() * 100:.0f}%'
          f' amp>10 {(P("lam") > 10).mean() * 100:.0f}% S&H {(P("lfw") == 5).mean() * 100:.0f}% speed {q(P("lfs"))}')
    pegs = [(R[i]['pr'], R[i]['pl']) for i in ids if R[i]['pegmove'] > 8][:5]
    if pegs:
        print('  PEG e.g. ' + '; '.join(f'R{a} L{b}' for a, b in pegs))
    for i in ids[:: max(1, len(ids) // 7)][:7]:
        print('   ', i, skel(i))
