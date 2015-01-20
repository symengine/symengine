#!/usr/bin/env python

"""
Benchmarks listed at http://wiki.sagemath.org/symbench can be run here
To run all benchmarks in csympy, python symbench.py
To run all benchmarks including sympy benchmarks, python symbench.py sympy
To run a set of benchmarks give the benchmark name as a command line argument

"""

from timeit import default_timer as clock
from random import random
import sys
import csympy

def R1(m = csympy):
    def f(z):
        return m.sqrt(m.Integer(1)/3)*z**2 + m.I/3
    if (m == csympy):
        t1 = clock()
        g = f(f(f(f(f(f(f(f(f(f(m.I/2)))))))))).expand()
        t2 = clock()
    else :
        t1 = clock()
        g = f(f(f(f(f(f(f(f(f(f(m.I/2)))))))))).as_real_imag()[0]
        t2 = clock()
    return t2 - t1

def R2(m = csympy):
    def hermite(n,y):
          if n == 1: return 2*y
          if n == 0: return 1
          return (2*y*hermite(n-1,y) - 2*(n-1)*hermite(n-2,y)).expand()
    t1 = clock()
    hermite(15, m.var('y'))
    t2 = clock()
    return t2 - t1

def R3(m = csympy):
    m.var('x y z')
    f = x+y+z
    t1 = clock()
    a = [bool(f==f) for _ in range(10)]
    t2 = clock()
    return t2 - t1

def R5(m = csympy):
    def blowup(L,n):
        for i in range(n):
            L.append( (L[i] + L[i+1]) * L[i+2] )
    def uniq(x):
        v = list(set(x))
        return v
    m.var('x y z')
    L = [x,y,z]
    blowup(L,8)
    t1 = clock()
    L = uniq(L)
    t2 = clock()
    return t2 - t1

benchmarks = []
if "R1" in sys.argv:
    benchmarks.append(R1)
if "R2" in sys.argv:
    benchmarks.append(R2)
if "R3" in sys.argv:
    benchmarks.append(R3)
if "R5" in sys.argv:
    benchmarks.append(R4)
if not benchmarks:
    benchmarks = [R1, R2, R3, R5]

run_sympy = False
if "sympy" in sys.argv:
    run_sympy = True
    import sympy
    print("\t\t CSymPy \t\t SymPy")
    for benchmark in benchmarks:
        print("Time for %s \t %s s\t %s s" % (benchmark.__name__, benchmark(csympy), benchmark(sympy)))
else :
    print("\t\t CSymPy")
    for benchmark in benchmarks:
        print("Time for %s \t %s s" % (benchmark.__name__, benchmark(csympy)))
