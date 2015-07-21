import sys
sys.path.append("../symengine/python")
from timeit import default_timer as clock
from symengine import var, sympify, function_symbol, Symbol, symbols
import sympy
from sympy.physics.mechanics import msubs
consts = symbols("forkcg1, mwf, mwr, mframe, WRrad, t, htangle, g, framelength, forkcg3, WFrad, forkoffset, mfork, forklength, framecg1, framecg3")
syms = symbols("s0 s1 s2 s3 s4 s5")

s = open("expr2.txt").read()
print "Converting to SymPy..."
e = sympy.sympify(s)
print "Converting to SymEngine..."
ce = sympify(e)
print "    Done."

print "Subs"
cf = ce.subs(dict(zip(consts, range(1, len(consts)+1))))
cf = cf.subs(dict(zip(syms, range(1, len(syms)+1))))
print "Eval initial expression"
print cf.n(real=True)
print cf.n(200)
# This number was checked against sympy:
assert abs(cf.n(real=True) - (-14960.1119361)) < 1e-6

print "SymPy diff:"
t1 = clock()
g = e.diff(syms[5])
t2 = clock()
tsympy = t2-t1
print "Total time:", t2-t1, "s"
print "SymEngine diff:"
t1 = clock()
cg = ce.diff(syms[5])
t2 = clock()
tsymengine = t2-t1
print "Total time:", t2-t1, "s"
print
print "Speedup: %.2fx" % (tsympy/tsymengine)

print
print "Subs"
cg = cg.subs(dict(zip(consts, range(1, len(consts)+1))))
cg = cg.subs(dict(zip(syms, range(1, len(syms)+1))))
print "Eval derivative"
print cg.n(real=True)
print cg.n(200)
# This number was checked against sympy:
assert abs(cg.n(real=True) - (-31789.936485)) < 1e-6

print "Converting SymPy derivative to SymEngine..."
cg = sympify(g)

print "Subs"
cg = cg.subs(dict(zip(consts, range(1, len(consts)+1))))
cg = cg.subs(dict(zip(syms, range(1, len(syms)+1))))
print "Eval derivative"
print cg.n(real=True)
print cg.n(200)
assert abs(cg.n(real=True) - (-31789.936485)) < 1e-6
