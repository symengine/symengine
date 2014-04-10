print "import..."
from timeit import default_timer as clock
import sympy
from sage.all import SR, var, Integer
print "    done."
print "Reading"
f = open("A.txt")
n = int(f.readline())
u = []
for i in range(n):
    u.append(sympy.sympify(f.readline()))
n = int(f.readline())
A = []
for i in range(n):
    A.append(sympy.sympify(f.readline()))
print "Done"
d = {}
for i in range(len(u)):
    d[u[i]] = sympy.Symbol("q%d" % i)
for i in range(20):
    d[sympy.Function("u%d" % i)(sympy.Symbol("t"))] = sympy.Symbol("u%d" % i)
d[sympy.Function("F")(sympy.Symbol("t"))] = sympy.Symbol("F")

A = [x.subs(d) for x in A]
u = [x.subs(d) for x in u]
print "SymPy -> Sage"
A = [SR(x) for x in A]
u = [SR(x) for x in u]
print "Jacobian"
t1 = clock()
dfdu = []
tmp = var("tmp")
for i in range(len(A)):
    for j in range(len(u)):
        dfdu.append(A[i].diff(u[j]))
t2 = clock()
print "Done"
print "Time: ", t2-t1
