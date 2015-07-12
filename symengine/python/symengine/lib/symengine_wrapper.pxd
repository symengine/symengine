cimport symengine
from symengine cimport RCP

cdef class Basic(object):
    cdef RCP[const symengine.Basic] thisptr

cdef class Number(Basic):
    pass

cdef class Integer(Number):
    pass

cdef class Rational(Number):
    pass

cdef class Complex(Number):
    pass

cdef class RealDouble(Number):
    pass

cdef class ComplexDouble(Number):
    pass

cdef class RealMPFR(Number):
    pass

cdef class ComplexMPC(Number):
    pass

cdef class Add(Basic):
    pass

cdef class Mul(Basic):
    pass

cdef class Pow(Basic):
    pass

cdef class Function(Basic):
    pass

cdef class TrigFunction(Basic):
    pass

cdef class Sin(TrigFunction):
    pass

cdef class Cos(TrigFunction):
    pass

cdef class Tan(TrigFunction):
    pass

cdef class Cot(TrigFunction):
    pass

cdef class Csc(TrigFunction):
    pass

cdef class Sec(TrigFunction):
    pass

cdef class ASin(TrigFunction):
    pass

cdef class ACos(TrigFunction):
    pass

cdef class ATan(TrigFunction):
    pass

cdef class ACot(TrigFunction):
    pass

cdef class ACsc(TrigFunction):
    pass

cdef class ASec(TrigFunction):
    pass

cdef class FunctionSymbol(Function):
    pass

cdef class FunctionWrapper(FunctionSymbol):
    pass

cdef class Abs(Function):
    pass

cdef class Derivative(Basic):
    pass

cdef class Subs(Basic):
    pass

cdef class MatrixBase(object):
    cdef symengine.MatrixBase* thisptr

cdef class DenseMatrix(MatrixBase):
    pass

cdef class Log(Basic):
    pass