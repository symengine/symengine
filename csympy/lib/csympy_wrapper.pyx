from cython.operator cimport dereference as deref
cimport csympy
from csympy cimport rcp, RCP

cdef class Basic(object):

    cdef RCP[csympy.Basic] getptr(self):
        pass

    def __add__(Basic self not None, Basic other not None):
        cdef RCP[csympy.Basic] a = self.getptr()
        cdef RCP[csympy.Basic] b = other.getptr()
        cdef RCP[csympy.Basic] c = csympy.add(a, b)
        s = deref(c).__str__()
        return s

cdef class Symbol(Basic):
    cdef RCP[csympy.Symbol] thisptr

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Symbol(name))

    def __dealloc__(self):
        self.thisptr.reset()

    cdef RCP[csympy.Basic] getptr(self):
        return <RCP[csympy.Basic]>self.thisptr

    def __str__(self):
        return deref(self.thisptr).__str__()

cdef class Integer(Basic):
    cdef RCP[csympy.Integer] thisptr

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Integer(name))

    def __dealloc__(self):
        self.thisptr.reset()

    def __str__(self):
        return deref(self.thisptr).__str__()
