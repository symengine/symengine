from cython.operator cimport dereference as deref
cimport csympy
from csympy cimport rcp, RCP

cdef c2py(RCP[csympy.Basic] o):
    # TODO: We need to determine the correct type:
    cdef Add r = Add.__new__(Add)
    r.thisptr = o
    return r

cdef class Basic(object):
    cdef RCP[csympy.Basic] thisptr

    def __add__(Basic self not None, Basic other not None):
        return c2py(csympy.add(self.thisptr, other.thisptr))

cdef class Symbol(Basic):

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Symbol(name))

    def __dealloc__(self):
        self.thisptr.reset()

    def __str__(self):
        return deref(self.thisptr).__str__()

cdef class Integer(Basic):

    def __cinit__(self, name):
        self.thisptr = rcp(new csympy.Integer(name))

    def __dealloc__(self):
        self.thisptr.reset()

    def __str__(self):
        return deref(self.thisptr).__str__()

cdef class Add(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def __str__(self):
        return deref(self.thisptr).__str__()
