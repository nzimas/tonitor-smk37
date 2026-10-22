#!/usr/bin/env python3
"""DX7 research: the audition's sounds measured beside every FM family's (host render on generation 6): brightness,
inharmonicity, attack, how much is left after 3 s, how the level moves, how far the pitch travels -- and for each new
family its nearest earlier one by those measures (each scaled to unit spread). python3 -I research/dx7/compare.py DIR"""
import sys, os, wave, glob, collections
import numpy as np

def load(p):
    w = wave.open(p)
    x = np.frombuffer(w.readframes(w.getnframes()), dtype=np.int16).astype(float) / 32768
    return x, w.getframerate()
def feats(p, hold=6.0):
    x, sr = load(p)
    h = int(hold * sr)
    x = x[:h]
    win = 2048
    n = len(x) // win
    rms = np.array([np.sqrt(np.mean(x[i * win:(i + 1) * win] ** 2)) + 1e-9 for i in range(n)])
    db = 20 * np.log10(rms)
    pk = db.max()
    t90 = np.argmax(db >= pk - 1) * win / sr
    left = db[min(n - 1, int(3.0 * sr / win))] - pk
    mv = np.mean(np.abs(np.diff(db[int(1.0 * sr / win):]))) if n > 30 else 0
    cents, inh, f0s = [], [], []
    for t in (0.1, 0.6, 2.0, 4.5):
        a = int(t * sr)
        seg = x[a:a + 8192] * np.hanning(8192)
        if len(seg) < 8192 or np.sqrt(np.mean(seg ** 2)) < 1e-4:
            continue
        S = np.abs(np.fft.rfft(seg)) ** 2
        f = np.fft.rfftfreq(8192, 1 / sr)
        m = (f > 40) & (f < 10000)
        cents.append((S[m] * f[m]).sum() / S[m].sum())
        # the strongest partial's place: how the pitch moves
        f0s.append(f[m][np.argmax(S[m])])
        # inharmonicity: energy off the multiples of the strongest low partial's half
        base = f[m][np.argmax(S[m] * (f[m] < 1200))] / 2 or 1
        q = np.abs(((f[m] / base) + 0.5) % 1 - 0.5) * base
        inh.append(1 - S[m][q < 12].sum() / S[m].sum())
    travel = 12 * np.abs(np.log2(max(f0s) / min(f0s))) if len(f0s) > 1 and min(f0s) > 0 else 0
    return dict(bright=np.log10(np.median(cents) if cents else 1), inharm=np.median(inh) if inh else 0,
                attack=np.log10(t90 * 1000 + 10), left=max(left, -60), move=min(mv, 4), travel=min(travel, 36),
                fall=np.log10(cents[0] / cents[-1]) if len(cents) > 1 else 0)

D = sys.argv[1]
rows = collections.defaultdict(list)
for p in sorted(glob.glob(os.path.join(D, 'FM-*.wav'))):
    fam = os.path.basename(p).split('-')[1]
    rows[fam].append(feats(p, 8.0))
K = list(next(iter(rows.values()))[0])
M = {f: np.array([[r[k] for k in K] for r in v]) for f, v in rows.items()}
allx = np.vstack(list(M.values()))
mu, sd = allx.mean(0), allx.std(0) + 1e-9
C = {f: ((m - mu) / sd).mean(0) for f, m in M.items()}
NEW = ['SHAPER', 'LADDER', 'DIVE', 'REVERSE', 'CREEP', 'CLANG']
OLD = [f for f in M if f not in NEW and f not in ('GLISSE', 'RUBBER')]
print('family     ' + ' '.join('%8s' % k for k in K))
for f in sorted(M, key=lambda f: (f in NEW, f)):
    print('%-10s ' % f + ' '.join('%8.2f' % v for v in M[f].mean(0)) + ('   (new)' if f in NEW else ''))
print()
for f in NEW:
    d = sorted((np.linalg.norm(C[f] - C[g]), g) for g in OLD)
    dn = sorted(np.linalg.norm(C[a] - C[b]) for a in OLD for b in OLD if a < b)
    print(f'{f}: nearest earlier families {d[0][1]} ({d[0][0]:.2f}), {d[1][1]} ({d[1][0]:.2f}), {d[2][1]} ({d[2][0]:.2f});'
          f' (two earlier families lie {np.median(dn):.2f} apart, the closest pair {dn[0]:.2f})')
