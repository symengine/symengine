from timeit import default_timer as clock
import sympy
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
A = sympy.Matrix(A)
print "Jacobian"
t1 = clock()
dfdu = A.jacobian(u)
t2 = clock()
print "Done"
print "Time: ", t2-t1
