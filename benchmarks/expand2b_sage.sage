from timeit import default_timer as clock
from sage.all import var, SR
R.<x, y, z, w> = QQ[]
# Let this expand first, we don't time it:
e = (x+y+z+w)**15
# Time the actual multiplication of two long polynomials:
t1 = clock()
f = e*(e+w)
t2 = clock()
print "Total time:", t2-t1, "s"
g = SR(f)
print "Number of terms:", len(g.operands())
