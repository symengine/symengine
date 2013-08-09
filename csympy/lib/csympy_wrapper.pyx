from cython.operator cimport dereference as deref
cimport csympy
from csympy cimport rcp, RCP

cdef class Basic(object):
    pass

cdef class Symbol(Basic):
    cdef RCP[csympy.Symbol] thisptr

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Symbol(name))

    def __str__(self):
        return deref(self.thisptr).__str__()

cdef class Integer(Basic):
    cdef RCP[csympy.Integer] thisptr

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Integer(name))

    def __str__(self):
        return deref(self.thisptr).__str__()
