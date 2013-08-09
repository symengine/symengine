from cython.operator cimport dereference as deref
cimport csympy
from csympy cimport rcp, RCP

class SympifyError(Exception):
    pass

cdef c2py(RCP[csympy.Basic] o):
    cdef Basic r
    if (csympy.is_a_Add(deref(o))):
        r = Add.__new__(Add)
    elif (csympy.is_a_Mul(deref(o))):
        r = Mul.__new__(Mul)
    elif (csympy.is_a_Pow(deref(o))):
        r = Pow.__new__(Pow)
    else:
        raise Exception("Unsupported CSymPy class.")
    r.thisptr = o
    return r

def sympify(a, raise_error=True):
    if isinstance(a, Basic):
        return a
    elif isinstance(a, (int, long)):
        return Integer(a)
    else:
        if raise_error:
            raise SympifyError("Cannot convert '%r' to a csympy type." % a)

cdef class Basic(object):
    cdef RCP[csympy.Basic] thisptr

    def __add__(Basic self not None, Basic other not None):
        return c2py(csympy.add(self.thisptr, other.thisptr))

    def __mul__(a, b):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.mul(A.thisptr, B.thisptr))

    # What is the purpose of "c" here?
    def __pow__(a, b, c):
        cdef Basic A = sympify(a, False)
        cdef Basic B = sympify(b, False)
        if A is None or B is None: return NotImplemented
        return c2py(csympy.pow(A.thisptr, B.thisptr))

    def __richcmp__(Basic self not None, Basic other not None, int op):
        if (op == 2):
            return csympy.eq(self.thisptr, other.thisptr)
        elif (op == 3):
            return csympy.neq(self.thisptr, other.thisptr)
        else:
            raise Exception("Operation not implemented.")


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

cdef class Mul(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def __str__(self):
        return deref(self.thisptr).__str__()

cdef class Pow(Basic):

    def __dealloc__(self):
        self.thisptr.reset()

    def __str__(self):
        return deref(self.thisptr).__str__()
