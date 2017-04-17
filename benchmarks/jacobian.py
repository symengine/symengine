from timeit import default_timer as clock
import sympy
from csympy import sympify, Symbol
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
A = [sympify(x) for x in A]
u = [sympify(x) for x in u]
print "Jacobian"
t1 = clock()
dfdu = []
tmp = Symbol("tmp")
for i in range(len(A)):
    for j in range(len(u)):
        dfdu.append(A[i].subs(u[j], tmp).diff(tmp).subs(tmp, u[j]))
t2 = clock()
print "Done"
print "Time: ", t2-t1
