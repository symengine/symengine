from libcpp cimport bool
from libcpp.string cimport string

cdef extern from "basic.h" namespace "Teuchos":
    cdef enum ENull:
        null

    cdef cppclass RCP[T]:
        T& operator*() nogil except +
        # Not yet supported in Cython:
#        RCP[T]& operator=(RCP[T] &r_ptr) nogil except +
        void reset() nogil except +

    RCP[Symbol] rcp_static_cast_Symbol "Teuchos::rcp_static_cast<CSymPy::Symbol>"(const RCP[Basic] &b) nogil


cdef extern from "basic.h" namespace "CSymPy":
    cdef cppclass Basic:
        string __str__() nogil except +
        RCP[Basic] diff(const RCP[Symbol] &x) nogil except +

    bool eq(RCP[Basic] &a, RCP[Basic] &b) nogil except +
    bool neq(RCP[Basic] &a, RCP[Basic] &b) nogil except +

    bool is_a_Add "CSymPy::is_a<CSymPy::Add>"(const Basic &b) nogil
    bool is_a_Mul "CSymPy::is_a<CSymPy::Mul>"(const Basic &b) nogil
    bool is_a_Pow "CSymPy::is_a<CSymPy::Pow>"(const Basic &b) nogil
    bool is_a_Integer "CSymPy::is_a<CSymPy::Integer>"(const Basic &b) nogil
    bool is_a_Rational "CSymPy::is_a<CSymPy::Rational>"(const Basic &b) nogil
    bool is_a_Symbol "CSymPy::is_a<CSymPy::Symbol>"(const Basic &b) nogil
    bool is_a_Sin "CSymPy::is_a<CSymPy::Sin>"(const Basic &b) nogil
    bool is_a_Cos "CSymPy::is_a<CSymPy::Cos>"(const Basic &b) nogil

    RCP[Basic] expand(const RCP[Basic] &o) nogil except +


cdef extern from "symbol.h" namespace "CSymPy":
    cdef cppclass Symbol(Basic):
        Symbol(string name) nogil
        string get_name() nogil


cdef extern from "integer.h" namespace "CSymPy":
    cdef cppclass Integer(Basic):
        Integer(int i) nogil

cdef extern from "rational.h" namespace "CSymPy":
    cdef cppclass Rational(Basic):
        pass


cdef extern from "add.h" namespace "CSymPy":
    cdef RCP[Basic] add(RCP[Basic] &a, RCP[Basic] &b) nogil except+
    cdef RCP[Basic] sub(RCP[Basic] &a, RCP[Basic] &b) nogil except+

    cdef cppclass Add(Basic):
        pass

cdef extern from "mul.h" namespace "CSymPy":
    cdef RCP[Basic] mul(RCP[Basic] &a, RCP[Basic] &b) nogil except+
    cdef RCP[Basic] div(RCP[Basic] &a, RCP[Basic] &b) nogil except+
    cdef RCP[Basic] neg(RCP[Basic] &a) nogil except+

    cdef cppclass Mul(Basic):
        pass

cdef extern from "pow.h" namespace "CSymPy":
    cdef RCP[Basic] pow(RCP[Basic] &a, RCP[Basic] &b) nogil except+
    cdef RCP[Basic] sqrt(RCP[Basic] &x) nogil except+

    cdef cppclass Pow(Basic):
        pass


cdef extern from "basic.h" namespace "Teuchos":
    # We need to specialize these for our classes:
    RCP[Basic] rcp(Symbol *p) nogil
    RCP[Basic] rcp(Integer *p) nogil

cdef extern from "functions.h" namespace "CSymPy":
    cdef RCP[Basic] sin(RCP[Basic] &arg) nogil except+
    cdef RCP[Basic] cos(RCP[Basic] &arg) nogil except+

    cdef cppclass Function(Basic):
        pass

    cdef cppclass Sin(Function):
        pass

    cdef cppclass Cos(Function):
        pass
