"""Drop unreferenced vertices, degenerate faces and duplicate faces from an OFF
triangle mesh, reindexing the rest. Used to make the dragon input for the README
gallery: data/dragonrecon.off has 2,103,979 vertices of which 1,726,460 belong to
no face; afront would otherwise spend minutes trimming a guidance field built
from them.

usage: clean_off.py in.off out.off
"""
import sys, time
import numpy as np

src, dst = sys.argv[1], sys.argv[2]
t = time.time()
f = open(src)
hdr = f.readline()
while hdr.strip() == '' or hdr.startswith('#') or hdr.strip() == 'OFF':
    hdr = f.readline()
nv, nf = [int(x) for x in hdr.split()[:2]]
V = [f.readline() for _ in range(nv)]
F = np.array([f.readline().split()[1:4] for _ in range(nf)], dtype=np.int64)

F = F[(F[:, 0] != F[:, 1]) & (F[:, 1] != F[:, 2]) & (F[:, 0] != F[:, 2])]
key = np.sort(F, 1)
_, keep = np.unique(key, axis=0, return_index=True)
F = F[np.sort(keep)]

used = np.zeros(nv, bool)
used[F.ravel()] = True
newidx = np.full(nv, -1, np.int64)
newidx[used] = np.arange(used.sum())

with open(dst, 'w') as o:
    o.write('OFF\n%d %d 0\n' % (used.sum(), len(F)))
    for i in np.flatnonzero(used):
        o.write(V[i])
    for a, b, c in newidx[F]:
        o.write('3 %d %d %d\n' % (a, b, c))
print('%s: %d vertices, %d faces -> %s: %d vertices, %d faces (%.0f s)'
      % (src, nv, nf, dst, used.sum(), len(F), time.time() - t))
