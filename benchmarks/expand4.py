import sys
sys.path.append("..")
from timeit import default_timer as clock
from csympy import var
var("x")
e = 1
for i in range(1, 351):
    e *= (i+x)**3
#print e
t1 = clock()
f = e.expand()
t2 = clock()
print "Total time:", t2-t1, "s"
