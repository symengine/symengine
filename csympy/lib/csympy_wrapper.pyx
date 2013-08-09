cimport csympy

cdef class Symbol(object):
    cdef csympy.Symbol *thisptr

    def __cinit__(self, name):
        self.thisptr = new csympy.Symbol(name);

    def __dealloc__(self):
        del self.thisptr

    def __str__(self):
        return self.thisptr.__str__()
