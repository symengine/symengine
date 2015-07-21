import sys
sys.path.append("../symengine/python")
from timeit import default_timer as clock
from symengine import var, sympify, function_symbol, Symbol, symbols
import sympy
syms = symbols("forkcg1, mwf, mframe, WRrad, t, htangle, g, framelength, forkcg3, WFrad, forkoffset, mfork, forklength, framecg1, framecg3")

s = open("expr.txt").read()
print "Converting to SymPy..."
e = sympy.sympify(s)
print "Subs"
t1 = clock()
t = sympy.Symbol("t")
states = [s(t) for s in sympy.symbols('q0, q1, q2, q3, q4, q5, '
                                      'u0, u1, u2, u3, u4, u5',
                                      cls=sympy.Function)]
subs2 = sympy.symbols('s:{}'.format(len(states)))
f = e.subs(dict(zip(states, subs2)))

print "Saving to expr2.txt"
s = str(f)
open("expr2.txt", "w").write(s)
