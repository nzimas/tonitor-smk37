#!/usr/bin/env python3
"""DX7 research: the unique voices' parameters (from voices155.bin) beside their measured sound (dx7scan's CSV),
clustered (k-means in numpy) and described. python3 -I research/dx7/analyse.py build/dx7 > research/dx7/clusters.txt"""
import sys, os, json
import numpy as np

D = sys.argv[1]
raw = open(os.path.join(D, 'voices155.bin'), 'rb').read()
N = len(raw) // 155
V = np.frombuffer(raw, dtype=np.uint8).reshape(N, 155).astype(int)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dx7lib import ALG, GR, ratio, kmeans  # noqa: E402

rows = []
for i in range(N):
    p = V[i]
    a = p[134] & 31
    car, tgt, fbop = GR[a]
    ops = [p[k * 21:(k + 1) * 21] for k in range(6)]
    r = [ratio(o) for o in ops]
    cr = [r[k][0] for k in range(6) if car[k] and r[k][0] is not None and ops[k][16] > 50]
    nfix = sum(1 for k in range(6) if ops[k][17] and ops[k][16] > 30)
    # modulation: the loudest modulator's level, the number of modulators that matter
    mods = [k for k in range(6) if not car[k] and ops[k][16] > 40]
    modol = max([ops[k][16] for k in mods], default=0)
    # inharmonic ratios between a modulator and its target (both ratio mode, a level that matters)
    def nonint(x):
        y = x * 2
        return abs(y - round(y)) > 0.04
    inh = 0
    for k in mods:
        t = tgt[k]
        if t is not None and r[k][0] and r[t][0]:
            q = r[k][0] / r[t][0]
            if nonint(q) and nonint(1 / q if q else 1):
                inh += 1
    cin = sum(1 for x in cr if nonint(x / min(cr))) if cr else 0
    cars = [k for k in range(6) if car[k] and ops[k][16] > 50] or [k for k in range(6) if car[k]]
    catt = np.mean([ops[k][0] for k in cars])       # R1
    csus = np.mean([ops[k][6] for k in cars])       # L3
    crel = np.mean([ops[k][3] for k in cars])       # R4
    mdec = np.mean([ops[k][1] for k in mods]) if mods else 0
    peg = max(abs(p[130 + j] - 50) for j in range(4))
    pegmove = peg if (len(set(p[130:134])) > 1) else 0
    lpm = p[139] * p[143] / 7
    lam = p[140] * max(ops[k][14] for k in range(6)) / 3
    rows.append(dict(i=i, alg=a + 1, fb=p[135], ncar=sum(car), cr=cr, nfix=nfix, modol=modol, nmods=len(mods), inhm=inh,
                     cinh=cin, catt=catt, csus=csus, crel=crel, mdec=mdec, peg=peg, pegmove=pegmove, pr=p[126:130].tolist(),
                     pl=p[130:134].tolist(), lfs=p[137], lfw=p[142], lpm=lpm, lam=lam, trnsp=p[144]))

# the sound
S = {}
for f in sorted(os.listdir(D)):
    if f.startswith('scan') and f.endswith('.csv'):
        for line in open(os.path.join(D, f)):
            x = line.strip().split(',')
            if len(x) == 24:
                S[int(x[0])] = [float(y) for y in x[1:]]
print(f'# {N} voices, {len(S)} measured', file=sys.stderr)
COLS = ['gain', 'bright', 'peak', 't90', 'tpk', 'l002', 'l015', 'l05', 'l1', 'l2', 'l38', 'r03', 'r10',
        'c0', 'ih0', 'p0', 'c1', 'ih1', 'p1', 'c2', 'ih2', 'p2', 'am']
idx = sorted(S)
M = {c: np.array([S[i][j] for i in idx]) for j, c in enumerate(COLS)}
R = [rows[i] for i in idx]
P = {k: np.array([r[k] for r in R], dtype=float) for k in ['alg', 'fb', 'ncar', 'nfix', 'modol', 'nmods', 'inhm', 'cinh', 'catt',
                                                          'csus', 'crel', 'mdec', 'peg', 'pegmove', 'lfs', 'lfw', 'lpm', 'lam']}
np.save(os.path.join(D, 'idx.npy'), np.array(idx))
json.dump({'rows': R, 'S': {str(i): S[i] for i in idx}}, open(os.path.join(D, 'all.json'), 'w'), default=float)

# the sound's descriptors for clustering (each scaled to unit spread)
lg = lambda x: np.log10(np.maximum(x, 1))
inh = np.maximum(M['ih1'], M['ih0'])
F = np.stack([
    lg(M['c1']) * 2,                 # brightness, held
    lg(M['c0']) - lg(M['c2']),       # brightness falling (the modulators dying first)
    inh * 3,                         # inharmonic / noisy
    lg(M['t90'] * 1000 + 1),         # attack
    np.clip(M['l38'], -60, 0) / 15,  # sustained or decayed by 3.8 s
    np.clip(M['l05'], -60, 0) / 15,  # .. by 0.5 s
    np.clip(M['r10'] - M['l38'], -60, 0) / 20,   # the release
    np.minimum(M['am'], 3) * 1.5,    # movement of level
    np.minimum(P['pegmove'], 50) / 25, # pitch envelope
    np.minimum(P['lpm'], 99) / 40,   # vibrato / pitch LFO
], 1)
mu, sd = F.mean(0), F.std(0) + 1e-9
Z = (F - mu) / sd

K = int(os.environ.get('K', 36))
L, C = kmeans(Z, K)
np.save(os.path.join(D, 'labels.npy'), L)

def q(x):
    return '%.0f/%.0f/%.0f' % tuple(np.percentile(x, [25, 50, 75]))
order = sorted(range(K), key=lambda j: -(L == j).sum())
for j in order:
    m = L == j
    n = m.sum()
    algs = np.bincount(P['alg'][m].astype(int), minlength=33)
    top = ' '.join(f'{a}:{algs[a]}' for a in np.argsort(-algs)[:5] if algs[a])
    print(f'== cluster {j}  n={n}')
    print(f'  sound: centroid Hz {q(M["c1"][m])}  fall c0/c2 {q(M["c0"][m])}->{q(M["c2"][m])}  inharm {np.median(inh[m]):.2f}'
          f' ({(inh[m] > 0.3).mean() * 100:.0f}% >0.3)  t90 ms {q(M["t90"][m] * 1000)}  lvl@0.5s {np.median(M["l05"][m]):.0f} dB'
          f' @3.8s {np.median(M["l38"][m]):.0f}  rel {np.median(M["r10"][m] - M["l38"][m]):.0f}  am {np.median(M["am"][m]):.2f}')
    print(f'  params: alg {top} | carriers {np.median(P["ncar"][m]):.0f} fb {np.median(P["fb"][m]):.0f} fixed>0 {(P["nfix"][m] > 0).mean() * 100:.0f}%'
          f' inharm-mod {(P["inhm"][m] > 0).mean() * 100:.0f}% inharm-car {(P["cinh"][m] > 0).mean() * 100:.0f}% modOL {np.median(P["modol"][m]):.0f}'
          f' | car R1 {q(P["catt"][m])} L3 {q(P["csus"][m])} R4 {q(P["crel"][m])} | PEG {(P["pegmove"][m] > 5).mean() * 100:.0f}%'
          f' LFO-pitch {(P["lpm"][m] > 10).mean() * 100:.0f}% LFO-amp {(P["lam"][m] > 10).mean() * 100:.0f}%')
    ex = [idx[i] for i in np.where(m)[0][:8]]
    print(f'  examples: {ex}')
