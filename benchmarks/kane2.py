import sys
sys.path.append("../symengine/python")
from timeit import default_timer as clock
import diff
from symengine import var, sympify, function_symbol, Symbol, DenseMatrix
import sympy
print "Setup"
t = diff.TimeJacobian()
t.setup()

print "Converting to SymEngine..."
m = sympify(t.matrix)
states = [sympify(x) for x in t.subs2]
states = DenseMatrix(len(states), 1, states)

print "SymPy Jacobian:"
t1 = clock()
t.time_jacobian()
t2 = clock()
tsympy = t2-t1
print "Total time:", t2-t1, "s"

print "SymEngine Jacobian:"
t1 = clock()
J = m.jacobian(states)
t2 = clock()
tsymengine = t2-t1
print "Total time:", t2-t1, "s"
print
print "Speedup: %.2fx" % (tsympy/tsymengine)

def check():
    for i in range(t.J.rows):
        for j in range(t.J.cols):
            print i, j, (t.J[i, j] - J[i, j]).expand()
#check()
