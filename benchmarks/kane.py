import sys
sys.path.append("..")
from timeit import default_timer as clock
from csympy import var, sympify, function_symbol, Symbol
import sympy
s = open("expr.txt").read()
print "Converting to SymPy..."
e = sympy.sympify(s)
print "Converting to CSymPy..."
ce = sympify(e)
print "    Done."
print "SymPy subs:"
t1 = clock()
f = e.subs(sympy.Function("q5")(sympy.Symbol("t")), sympy.Symbol("sq5"))
t2 = clock()
print "Total time:", t2-t1, "s"
print "CSymPy subs:"
t1 = clock()
cf = ce.subs(function_symbol("q5", Symbol("t")), Symbol("sq5"))
t2 = clock()
print "Total time:", t2-t1, "s"

print "SymPy diff:"
t1 = clock()
g = f.diff(sympy.Symbol("sq5"))
t2 = clock()
print "Total time:", t2-t1, "s"
print "CSymPy diff:"
t1 = clock()
cg = cf.diff(Symbol("sq5"))
t2 = clock()
print "Total time:", t2-t1, "s"
