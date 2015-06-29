cimport symengine
from symengine cimport rcp, RCP, set, map_basic_basic, vec_basic, set_basic

cdef class Basic(object):
    cdef RCP[const symengine.Basic] thisptr

cdef class Number(Basic):
    pass

cdef class Integer(Number):
    pass

cdef class Rational(Number):
    pass

cdef class RealDouble(Number):
    pass

cdef class ComplexDouble(Number):
    pass

cdef class RealMPFR(Number):
    pass

cdef class ComplexMPC(Number):
    pass