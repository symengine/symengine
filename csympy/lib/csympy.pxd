from libcpp cimport bool
from libcpp.string cimport string
from libcpp.map cimport map
from libcpp.vector cimport vector

cdef extern from 'gmpxx.h':
    cdef cppclass mpz_class:
        mpz_class()
        mpz_class(int i)
        mpz_class(mpz_class)
        mpz_class(const string &s, int base) except +
    cdef cppclass mpq_class:
        mpq_class()

cdef extern from "csympy_rcp.h" namespace "CSymPy":
    cdef enum ENull:
        null

    cdef cppclass RCP[T]:
        T& operator*() nogil except +
        # Not yet supported in Cython:
#        RCP[T]& operator=(RCP[T] &r_ptr) nogil except +
        void reset() nogil except +

    cdef cppclass Ptr[T]:
        T& operator*() nogil except +

    RCP[const Symbol] rcp_static_cast_Symbol "CSymPy::rcp_static_cast<const CSymPy::Symbol>"(RCP[const Basic] &b) nogil
    RCP[const Add] rcp_static_cast_Add "CSymPy::rcp_static_cast<const CSymPy::Add>"(RCP[const Basic] &b) nogil
    RCP[const Mul] rcp_static_cast_Mul "CSymPy::rcp_static_cast<const CSymPy::Mul>"(RCP[const Basic] &b) nogil
    RCP[const Pow] rcp_static_cast_Pow "CSymPy::rcp_static_cast<const CSymPy::Pow>"(RCP[const Basic] &b) nogil
    RCP[const Sin] rcp_static_cast_Sin "CSymPy::rcp_static_cast<const CSymPy::Sin>"(RCP[const Basic] &b) nogil
    RCP[const Cos] rcp_static_cast_Cos "CSymPy::rcp_static_cast<const CSymPy::Cos>"(RCP[const Basic] &b) nogil
    RCP[const FunctionSymbol] rcp_static_cast_FunctionSymbol "CSymPy::rcp_static_cast<const CSymPy::FunctionSymbol>"(RCP[const Basic] &b) nogil
    RCP[const Derivative] rcp_static_cast_Derivative "CSymPy::rcp_static_cast<const CSymPy::Derivative>"(RCP[const Basic] &b) nogil
    Ptr[RCP[Basic]] outArg(RCP[const Basic] &arg) nogil

    void print_stack_on_segfault() nogil


cdef extern from "basic.h" namespace "CSymPy":
    ctypedef map[RCP[Basic], RCP[Basic]] map_basic_basic
    ctypedef vector[RCP[Basic]] vec_basic "CSymPy::vec_basic"
    cdef cppclass Basic:
        string __str__() nogil except +
        unsigned int hash() nogil except +
        RCP[const Basic] diff(RCP[const Symbol] &x) nogil except +
        RCP[const Basic] subs(map_basic_basic &x) nogil except +
        vector[RCP[Basic]] get_args() nogil

    bool eq(RCP[const Basic] &a, RCP[const Basic] &b) nogil except +
    bool neq(RCP[const Basic] &a, RCP[const Basic] &b) nogil except +

    bool is_a_Add "CSymPy::is_a<CSymPy::Add>"(const Basic &b) nogil
    bool is_a_Mul "CSymPy::is_a<CSymPy::Mul>"(const Basic &b) nogil
    bool is_a_Pow "CSymPy::is_a<CSymPy::Pow>"(const Basic &b) nogil
    bool is_a_Integer "CSymPy::is_a<CSymPy::Integer>"(const Basic &b) nogil
    bool is_a_Rational "CSymPy::is_a<CSymPy::Rational>"(const Basic &b) nogil
    bool is_a_Complex "CSymPy::is_a<CSymPy::Complex>"(const Basic &b) nogil
    bool is_a_Symbol "CSymPy::is_a<CSymPy::Symbol>"(const Basic &b) nogil
    bool is_a_Sin "CSymPy::is_a<CSymPy::Sin>"(const Basic &b) nogil
    bool is_a_Cos "CSymPy::is_a<CSymPy::Cos>"(const Basic &b) nogil
    bool is_a_FunctionSymbol "CSymPy::is_a<CSymPy::FunctionSymbol>"(const Basic &b) nogil
    bool is_a_Derivative "CSymPy::is_a<CSymPy::Derivative>"(const Basic &b) nogil

    RCP[const Basic] expand(RCP[const Basic] &o) nogil except +


cdef extern from "symbol.h" namespace "CSymPy":
    cdef cppclass Symbol(Basic):
        Symbol(string name) nogil
        string get_name() nogil

cdef extern from "number.h" namespace "CSymPy":
    cdef cppclass Number(Basic):
        pass

cdef extern from "integer.h" namespace "CSymPy":
    cdef cppclass Integer(Number):
        Integer(int i) nogil
        Integer(mpz_class i) nogil

cdef extern from "rational.h" namespace "CSymPy":
    cdef cppclass Rational(Number):
        pass

cdef extern from "complex.h" namespace "CSymPy":
    cdef cppclass Complex(Number):
        pass
    RCP[const Basic] I


cdef extern from "add.h" namespace "CSymPy":
    cdef RCP[const Basic] add(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] sub(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+

    cdef cppclass Add(Basic):
        void as_two_terms(const Ptr[RCP[Basic]] &a, const Ptr[RCP[Basic]] &b)

cdef extern from "mul.h" namespace "CSymPy":
    cdef RCP[const Basic] mul(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] div(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] neg(RCP[const Basic] &a) nogil except+

    cdef cppclass Mul(Basic):
        void as_two_terms(const Ptr[RCP[Basic]] &a, const Ptr[RCP[Basic]] &b)

cdef extern from "pow.h" namespace "CSymPy":
    cdef RCP[const Basic] pow(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] sqrt(RCP[const Basic] &x) nogil except+

    cdef cppclass Pow(Basic):
        RCP[const Basic] get_base() nogil
        RCP[const Basic] get_exp() nogil


cdef extern from "basic.h" namespace "CSymPy":
    # We need to specialize these for our classes:
    RCP[const Basic] rcp(Symbol *p) nogil
    RCP[const Basic] rcp(Integer *p) nogil

cdef extern from "functions.h" namespace "CSymPy":
    cdef RCP[const Basic] sin(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] cos(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] function_symbol(string name, RCP[const Basic] &arg) nogil except+

    cdef cppclass Function(Basic):
        pass

    cdef cppclass Sin(Function):
        RCP[const Basic] get_arg() nogil

    cdef cppclass Cos(Function):
        RCP[const Basic] get_arg() nogil

    cdef cppclass FunctionSymbol(Function):
        string get_name() nogil
        RCP[const Basic] get_arg() nogil

    cdef cppclass Derivative(Basic):
        RCP[const Basic] get_arg() nogil
        vector[RCP[Basic]] get_symbols() nogil

cdef extern from "dict.h" namespace "CSymPy":
    cdef cppclass VecBasic:
        void push_back(const RCP[const Basic] &e) nogil
        vector[RCP[Basic]] get_vec() nogil

cdef extern from "matrix.h" namespace "CSymPy":
    cdef cppclass MatrixBase:
        const unsigned nrows() nogil
        const unsigned ncols() nogil
        RCP[const Basic] get(unsigned i, unsigned j) nogil
        RCP[const Basic] set(unsigned i, unsigned j, const RCP[const Basic] &e) nogil
        string __str__() nogil except+
        bool eq(const MatrixBase &) nogil

    cdef cppclass DenseMatrix(MatrixBase):
        DenseMatrix()
        DenseMatrix(unsigned i, unsigned j) nogil
        DenseMatrix(unsigned i, unsigned j, const vector[RCP[Basic]] &v) nogil

    bool is_a_DenseMatrix "CSymPy::is_a<CSymPy::DenseMatrix>"(const MatrixBase &b) nogil
    DenseMatrix* static_cast_DenseMatrix "static_cast<CSymPy::DenseMatrix*>"(const MatrixBase *a)
