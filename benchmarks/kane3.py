import sys
sys.path.append("../symengine/python")
from timeit import default_timer as clock
from symengine import var, sympify, function_symbol, Symbol, symbols
import sympy
from sympy.physics.mechanics import msubs

try:
    import sage
    with_sage=True
except ImportError:
    with_sage=False

consts = symbols("forkcg1, mwf, mwr, mframe, WRrad, t, htangle, g, framelength, forkcg3, WFrad, forkoffset, mfork, forklength, framecg1, framecg3")
syms = symbols("s0 s1 s2 s3 s4 s5")

s = open("expr2.txt").read()
print "Converting to SymPy..."
e = sympy.sympify(s)
print "Converting to SymEngine..."
ce = sympify(e)
print "    Done."

if with_sage:
    print "Converting to Sage..."
    sconsts = [i._sage_() for i in consts]
    ssyms = [i._sage_() for i in syms]
    se = ce._sage_()
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

if with_sage:
    print "Sage diff:"
    t1 = clock()
    sg = se.diff(ssyms[5])
    t2 = clock()
    tsage = t2-t1
    print "Total time:", t2-t1, "s"
    print
    print "Speedup compared to Sage : %.2fx" % (tsage/tsymengine)

print "Speedup compared to SymPy: %.2fx" % (tsympy/tsymengine)

print
print "Subs"
cg = cg.subs(dict(zip(consts, range(1, len(consts)+1))))
cg = cg.subs(dict(zip(syms, range(1, len(syms)+1))))
print "SymEngine Eval derivative"
print cg.n(real=True)
print cg.n(200)
# This number was checked against sympy:
assert abs(cg.n(real=True) - (-31789.936485)) < 1e-6

print "Converting SymPy derivative to SymEngine..."
cg = sympify(g)

print "Subs"
cg = cg.subs(dict(zip(consts, range(1, len(consts)+1))))
cg = cg.subs(dict(zip(syms, range(1, len(syms)+1))))
print "SymPy/SymEngine Eval derivative"
print cg.n(real=True)
print cg.n(200)
assert abs(cg.n(real=True) - (-31789.936485)) < 1e-6

if with_sage:
    print "Subs"
    sg = sg.subs(dict(zip(sconsts, range(1, len(sconsts)+1))))
    sg = sg.subs(dict(zip(ssyms, range(1, len(ssyms)+1))))
    print "Sage Eval derivative"
    print sg.n()
    print sg.n(200)
    assert abs(sg.n() - (-31789.936485)) < 1e-6
