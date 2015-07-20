from libcpp cimport bool
from libcpp.string cimport string
from libcpp.map cimport map
from libcpp.vector cimport vector

include "config.pxi"

cdef extern from 'gmpxx.h':
    ctypedef unsigned long mp_limb_t
    ctypedef struct __mpz_struct:
        pass
    ctypedef struct __mpq_struct:
        pass
    ctypedef __mpz_struct mpz_t[1]
    ctypedef __mpq_struct mpq_t[1]

    cdef cppclass mpz_class:
        mpz_class()
        mpz_class(int i)
        mpz_class(mpz_class)
        mpz_class(mpz_t)
        mpz_class(const string &s, int base) except +
        mpz_t get_mpz_t()
    cdef cppclass mpq_class:
        mpq_class()
        mpq_class(mpq_t)
        mpq_t get_mpq_t()

cdef extern from "<set>" namespace "std":
# Cython's libcpp.set does not support two template arguments to set.
# Methods to declare and iterate a set with a custom compare are given here
    cdef cppclass set[T, U]:
        cppclass iterator:
            T& operator*()
            iterator operator++() nogil
            iterator operator--() nogil
            bint operator==(iterator) nogil
            bint operator!=(iterator) nogil
        iterator begin() nogil
        iterator end() nogil

cdef extern from "<symengine/symengine_rcp.h>" namespace "SymEngine":
    cdef enum ENull:
        null

    cdef cppclass RCP[T]:
        T& operator*() nogil except +
        # Not yet supported in Cython:
#        RCP[T]& operator=(RCP[T] &r_ptr) nogil except +
        void reset() nogil except +

    cdef cppclass Ptr[T]:
        T& operator*() nogil except +

    RCP[const Symbol] rcp_static_cast_Symbol "SymEngine::rcp_static_cast<const SymEngine::Symbol>"(RCP[const Basic] &b) nogil
    RCP[const Integer] rcp_static_cast_Integer "SymEngine::rcp_static_cast<const SymEngine::Integer>"(RCP[const Basic] &b) nogil
    RCP[const Rational] rcp_static_cast_Rational "SymEngine::rcp_static_cast<const SymEngine::Rational>"(RCP[const Basic] &b) nogil
    RCP[const Complex] rcp_static_cast_Complex "SymEngine::rcp_static_cast<const SymEngine::Complex>"(RCP[const Basic] &b) nogil
    RCP[const Number] rcp_static_cast_Number "SymEngine::rcp_static_cast<const SymEngine::Number>"(RCP[const Basic] &b) nogil
    RCP[const Add] rcp_static_cast_Add "SymEngine::rcp_static_cast<const SymEngine::Add>"(RCP[const Basic] &b) nogil
    RCP[const Mul] rcp_static_cast_Mul "SymEngine::rcp_static_cast<const SymEngine::Mul>"(RCP[const Basic] &b) nogil
    RCP[const Pow] rcp_static_cast_Pow "SymEngine::rcp_static_cast<const SymEngine::Pow>"(RCP[const Basic] &b) nogil
    RCP[const TrigFunction] rcp_static_cast_TrigFunction "SymEngine::rcp_static_cast<const SymEngine::TrigFunction>"(RCP[const Basic] &b) nogil
    RCP[const HyperbolicFunction] rcp_static_cast_HyperbolicFunction "SymEngine::rcp_static_cast<const SymEngine::HyperbolicFunction>"(RCP[const Basic] &b) nogil
    RCP[const FunctionSymbol] rcp_static_cast_FunctionSymbol "SymEngine::rcp_static_cast<const SymEngine::FunctionSymbol>"(RCP[const Basic] &b) nogil
    RCP[const FunctionWrapper] rcp_static_cast_FunctionWrapper "SymEngine::rcp_static_cast<const SymEngine::FunctionWrapper>"(RCP[const Basic] &b) nogil
    RCP[const Abs] rcp_static_cast_Abs "SymEngine::rcp_static_cast<const SymEngine::Abs>"(RCP[const Basic] &b) nogil
    RCP[const Derivative] rcp_static_cast_Derivative "SymEngine::rcp_static_cast<const SymEngine::Derivative>"(RCP[const Basic] &b) nogil
    RCP[const Subs] rcp_static_cast_Subs "SymEngine::rcp_static_cast<const SymEngine::Subs>"(RCP[const Basic] &b) nogil
    RCP[const RealDouble] rcp_static_cast_RealDouble "SymEngine::rcp_static_cast<const SymEngine::RealDouble>"(RCP[const Basic] &b) nogil
    RCP[const ComplexDouble] rcp_static_cast_ComplexDouble "SymEngine::rcp_static_cast<const SymEngine::ComplexDouble>"(RCP[const Basic] &b) nogil
    RCP[const RealMPFR] rcp_static_cast_RealMPFR "SymEngine::rcp_static_cast<const SymEngine::RealMPFR>"(RCP[const Basic] &b) nogil
    RCP[const ComplexMPC] rcp_static_cast_ComplexMPC "SymEngine::rcp_static_cast<const SymEngine::ComplexMPC>"(RCP[const Basic] &b) nogil
    RCP[const Log] rcp_static_cast_Log "SymEngine::rcp_static_cast<const SymEngine::Log>"(RCP[const Basic] &b) nogil
    Ptr[RCP[Basic]] outArg(RCP[const Basic] &arg) nogil
    Ptr[RCP[Integer]] outArg_Integer "SymEngine::outArg<SymEngine::RCP<const SymEngine::Integer>>"(RCP[const Integer] &arg) nogil

    void print_stack_on_segfault() nogil


cdef extern from "<symengine/basic.h>" namespace "SymEngine":
    ctypedef Basic const_Basic "const SymEngine::Basic"
    ctypedef map[RCP[Basic], RCP[Basic]] map_basic_basic
    ctypedef vector[RCP[Basic]] vec_basic "SymEngine::vec_basic"
    ctypedef vector[RCP[Integer]] vec_integer "SymEngine::vec_integer"
    ctypedef map[RCP[Integer], unsigned] map_integer_uint "SymEngine::map_integer_uint"
    cdef struct RCPIntegerKeyLess
    cdef struct RCPBasicKeyLess
    ctypedef set[RCP[const_Basic], RCPBasicKeyLess] set_basic "SymEngine::set_basic"
    cdef cppclass Basic:
        string __str__() nogil except +
        unsigned int hash() nogil except +
        RCP[const Basic] diff(RCP[const Symbol] &x) nogil except +
        RCP[const Basic] subs(map_basic_basic &x) nogil except +
        vec_basic get_args() nogil

    bool eq(const Basic &a, const Basic &b) nogil except +
    bool neq(const Basic &a, const Basic &b) nogil except +

    bool is_a_Add "SymEngine::is_a<SymEngine::Add>"(const Basic &b) nogil
    bool is_a_Mul "SymEngine::is_a<SymEngine::Mul>"(const Basic &b) nogil
    bool is_a_Pow "SymEngine::is_a<SymEngine::Pow>"(const Basic &b) nogil
    bool is_a_Integer "SymEngine::is_a<SymEngine::Integer>"(const Basic &b) nogil
    bool is_a_Rational "SymEngine::is_a<SymEngine::Rational>"(const Basic &b) nogil
    bool is_a_Complex "SymEngine::is_a<SymEngine::Complex>"(const Basic &b) nogil
    bool is_a_Symbol "SymEngine::is_a<SymEngine::Symbol>"(const Basic &b) nogil
    bool is_a_Constant "SymEngine::is_a<SymEngine::Constant>"(const Basic &b) nogil
    bool is_a_Sin "SymEngine::is_a<SymEngine::Sin>"(const Basic &b) nogil
    bool is_a_Cos "SymEngine::is_a<SymEngine::Cos>"(const Basic &b) nogil
    bool is_a_Tan "SymEngine::is_a<SymEngine::Tan>"(const Basic &b) nogil
    bool is_a_Cot "SymEngine::is_a<SymEngine::Cot>"(const Basic &b) nogil
    bool is_a_Csc "SymEngine::is_a<SymEngine::Csc>"(const Basic &b) nogil
    bool is_a_Sec "SymEngine::is_a<SymEngine::Sec>"(const Basic &b) nogil
    bool is_a_ASin "SymEngine::is_a<SymEngine::ASin>"(const Basic &b) nogil
    bool is_a_ACos "SymEngine::is_a<SymEngine::ACos>"(const Basic &b) nogil
    bool is_a_ATan "SymEngine::is_a<SymEngine::ATan>"(const Basic &b) nogil
    bool is_a_ACot "SymEngine::is_a<SymEngine::ACot>"(const Basic &b) nogil
    bool is_a_ACsc "SymEngine::is_a<SymEngine::ACsc>"(const Basic &b) nogil
    bool is_a_ASec "SymEngine::is_a<SymEngine::ASec>"(const Basic &b) nogil
    bool is_a_Sinh "SymEngine::is_a<SymEngine::Sinh>"(const Basic &b) nogil
    bool is_a_Cosh "SymEngine::is_a<SymEngine::Cosh>"(const Basic &b) nogil
    bool is_a_Tanh "SymEngine::is_a<SymEngine::Tanh>"(const Basic &b) nogil
    bool is_a_Coth "SymEngine::is_a<SymEngine::Coth>"(const Basic &b) nogil
    bool is_a_ASinh "SymEngine::is_a<SymEngine::ASinh>"(const Basic &b) nogil
    bool is_a_ACosh "SymEngine::is_a<SymEngine::ACosh>"(const Basic &b) nogil
    bool is_a_ATanh "SymEngine::is_a<SymEngine::ATanh>"(const Basic &b) nogil
    bool is_a_ACoth "SymEngine::is_a<SymEngine::ACoth>"(const Basic &b) nogil
    bool is_a_FunctionSymbol "SymEngine::is_a<SymEngine::FunctionSymbol>"(const Basic &b) nogil
    bool is_a_Abs "SymEngine::is_a<SymEngine::Abs>"(const Basic &b) nogil
    bool is_a_Derivative "SymEngine::is_a<SymEngine::Derivative>"(const Basic &b) nogil
    bool is_a_Subs "SymEngine::is_a<SymEngine::Subs>"(const Basic &b) nogil
    bool is_a_FunctionWrapper "SymEngine::is_a<SymEngine::FunctionWrapper>"(const Basic &b) nogil
    bool is_a_RealDouble "SymEngine::is_a<SymEngine::RealDouble>"(const Basic &b) nogil
    bool is_a_ComplexDouble "SymEngine::is_a<SymEngine::ComplexDouble>"(const Basic &b) nogil
    bool is_a_RealMPFR "SymEngine::is_a<SymEngine::RealMPFR>"(const Basic &b) nogil
    bool is_a_ComplexMPC "SymEngine::is_a<SymEngine::ComplexMPC>"(const Basic &b) nogil
    bool is_a_Log "SymEngine::is_a<SymEngine::Log>"(const Basic &b) nogil

    RCP[const Basic] expand(RCP[const Basic] &o) nogil except +


cdef extern from "<symengine/symbol.h>" namespace "SymEngine":
    cdef cppclass Symbol(Basic):
        Symbol(string name) nogil
        string get_name() nogil

cdef extern from "<symengine/number.h>" namespace "SymEngine":
    cdef cppclass Number(Basic):
        pass

cdef extern from "<symengine/integer.h>" namespace "SymEngine":
    cdef cppclass Integer(Number):
        Integer(int i) nogil
        Integer(mpz_class i) nogil
        int compare(const Basic &o) nogil
        mpz_class as_mpz() nogil
    cdef RCP[const Integer] integer(int i) nogil
    cdef RCP[const Integer] integer(mpz_class i) nogil

cdef extern from "<symengine/rational.h>" namespace "SymEngine":
    cdef cppclass Rational(Number):
        mpq_class as_mpq() nogil
    cdef RCP[const Number] from_mpq "SymEngine::Rational::from_mpq"(mpq_class) nogil
    cdef void get_num_den(const RCP[Rational] &rat, const Ptr[RCP[Integer]] &num,
                     const Ptr[RCP[Integer]] &den) nogil

cdef extern from "<symengine/complex.h>" namespace "SymEngine":
    cdef cppclass Complex(Number):
        RCP[const Number] real_part() nogil
        RCP[const Number] imaginary_part() nogil

cdef extern from "<symengine/real_double.h>" namespace "SymEngine":
    cdef cppclass RealDouble(Number):
        RealDouble(double x) nogil
        double as_double() nogil
    RCP[const RealDouble] real_double(double d) nogil

cdef extern from "<symengine/complex_double.h>" namespace "SymEngine":
    cdef cppclass ComplexDouble(Number):
        ComplexDouble(double complex x) nogil
        RCP[const Number] real_part() nogil
        RCP[const Number] imaginary_part() nogil
        double complex as_complex_double() nogil
    RCP[const ComplexDouble] complex_double(double complex d) nogil

cdef extern from "<symengine/constants.h>" namespace "SymEngine":
    cdef cppclass Constant(Basic):
        Constant(string name) nogil
        string get_name() nogil
    RCP[const Basic] I
    RCP[const Basic] E
    RCP[const Basic] pi


cdef extern from "<symengine/add.h>" namespace "SymEngine":
    cdef RCP[const Basic] add(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] sub(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+

    cdef cppclass Add(Basic):
        void as_two_terms(const Ptr[RCP[Basic]] &a, const Ptr[RCP[Basic]] &b)

cdef extern from "<symengine/mul.h>" namespace "SymEngine":
    cdef RCP[const Basic] mul(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] div(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] neg(RCP[const Basic] &a) nogil except+

    cdef cppclass Mul(Basic):
        void as_two_terms(const Ptr[RCP[Basic]] &a, const Ptr[RCP[Basic]] &b)

cdef extern from "<symengine/pow.h>" namespace "SymEngine":
    cdef RCP[const Basic] pow(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] sqrt(RCP[const Basic] &x) nogil except+
    cdef RCP[const Basic] exp(RCP[const Basic] &x) nogil except+
    cdef RCP[const Basic] log(RCP[const Basic] &x) nogil except+
    cdef RCP[const Basic] log(RCP[const Basic] &x, RCP[const Basic] &y) nogil except+

    cdef cppclass Pow(Basic):
        RCP[const Basic] get_base() nogil
        RCP[const Basic] get_exp() nogil

    cdef cppclass Log(Basic):
        RCP[const Basic] get_arg() nogil


cdef extern from "<symengine/basic.h>" namespace "SymEngine":
    # We need to specialize these for our classes:
    RCP[const Basic] make_rcp_Symbol "SymEngine::make_rcp<const SymEngine::Symbol>"(string name) nogil
    RCP[const Basic] make_rcp_Constant "SymEngine::make_rcp<const SymEngine::Constant>"(string name) nogil
    RCP[const Basic] make_rcp_Integer "SymEngine::make_rcp<const SymEngine::Integer>"(int i) nogil
    RCP[const Basic] make_rcp_Integer "SymEngine::make_rcp<const SymEngine::Integer>"(mpz_class i) nogil
    RCP[const Basic] make_rcp_Subs "SymEngine::make_rcp<const SymEngine::Subs>"(const RCP[const Basic] &arg, const map_basic_basic &x) nogil
    RCP[const Basic] make_rcp_Derivative "SymEngine::make_rcp<const SymEngine::Derivative>"(const RCP[const Basic] &arg, const vec_basic &x) nogil
    RCP[const Basic] make_rcp_FunctionWrapper "SymEngine::make_rcp<const SymEngine::FunctionWrapper>"(void* obj, string name, string hash_, const vec_basic &arg, \
            void (*dec_ref)(void *), int (*comp)(void *, void *)) nogil
    RCP[const Basic] make_rcp_RealDouble "SymEngine::make_rcp<const SymEngine::RealDouble>"(double x) nogil
    RCP[const Basic] make_rcp_ComplexDouble "SymEngine::make_rcp<const SymEngine::ComplexDouble>"(double complex x) nogil

cdef extern from "<symengine/functions.h>" namespace "SymEngine":
    cdef RCP[const Basic] sin(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] cos(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] tan(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] cot(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] csc(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] sec(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] asin(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] acos(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] atan(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] acot(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] acsc(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] asec(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] sinh(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] cosh(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] tanh(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] coth(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] asinh(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] acosh(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] atanh(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] acoth(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] function_symbol(string name, const vec_basic &arg) nogil except+
    cdef RCP[const Basic] abs(RCP[const Basic] &arg) nogil except+

    cdef cppclass Function(Basic):
        pass

    cdef cppclass TrigFunction(Function):
        RCP[const Basic] get_arg() nogil

    cdef cppclass Sin(TrigFunction):
        pass

    cdef cppclass Cos(TrigFunction):
        pass

    cdef cppclass Tan(TrigFunction):
        pass

    cdef cppclass Cot(TrigFunction):
        pass

    cdef cppclass Csc(TrigFunction):
        pass

    cdef cppclass Sec(TrigFunction):
        pass

    cdef cppclass ASin(TrigFunction):
        pass

    cdef cppclass ACos(TrigFunction):
        pass

    cdef cppclass ATan(TrigFunction):
        pass

    cdef cppclass ACot(TrigFunction):
        pass

    cdef cppclass ACsc(TrigFunction):
        pass

    cdef cppclass ASec(TrigFunction):
        pass

    cdef cppclass HyperbolicFunction(Function):
        RCP[const Basic] get_arg() nogil

    cdef cppclass Sinh(HyperbolicFunction):
        pass

    cdef cppclass Cosh(HyperbolicFunction):
        pass

    cdef cppclass Tanh(HyperbolicFunction):
        pass

    cdef cppclass Coth(HyperbolicFunction):
        pass

    cdef cppclass ASinh(HyperbolicFunction):
        pass

    cdef cppclass ACosh(HyperbolicFunction):
        pass

    cdef cppclass ATanh(HyperbolicFunction):
        pass

    cdef cppclass ACoth(HyperbolicFunction):
        pass

    cdef cppclass FunctionSymbol(Function):
        string get_name() nogil

    cdef cppclass FunctionWrapper(FunctionSymbol):
        FunctionWrapper(void* obj, string name, string hash_, const vec_basic &arg, \
            void (*dec_ref)(void *), int (*comp)(void *, void *))
        void* get_object()

    cdef cppclass Derivative(Basic):
        Derivative(const RCP[const Basic] &arg, const vec_basic &x) nogil
        RCP[const Basic] get_arg() nogil
        vec_basic get_symbols() nogil

    cdef cppclass Subs(Basic):
        Subs(const RCP[const Basic] &arg, const map_basic_basic &x) nogil
        RCP[const Basic] get_arg() nogil
        vec_basic get_variables() nogil
        vec_basic get_point() nogil

    cdef cppclass Abs(Function):
        RCP[const Basic] get_arg() nogil

IF HAVE_SYMENGINE_MPFR:
    cdef extern from "mpfr.h":
        ctypedef struct __mpfr_struct:
            pass
        ctypedef __mpfr_struct mpfr_t[1]
        ctypedef __mpfr_struct* mpfr_ptr
        ctypedef const __mpfr_struct* mpfr_srcptr
        ctypedef long mpfr_prec_t
        ctypedef enum mpfr_rnd_t:
            MPFR_RNDN
            MPFR_RNDZ
            MPFR_RNDU
            MPFR_RNDD
            MPFR_RNDA
            MPFR_RNDF
            MPFR_RNDNA

    cdef extern from "<symengine/real_mpfr.h>" namespace "SymEngine":
        cdef cppclass mpfr_class:
            mpfr_class() nogil
            mpfr_class(mpfr_prec_t prec) nogil
            mpfr_class(string s, mpfr_prec_t prec, unsigned base) nogil
            mpfr_class(mpfr_t m) nogil
            mpfr_ptr get_mpfr_t() nogil

        cdef cppclass RealMPFR(Number):
            RealMPFR(mpfr_class) nogil
            mpfr_class as_mpfr() nogil
            mpfr_prec_t get_prec() nogil

        RCP[const RealMPFR] real_mpfr(mpfr_class t) nogil
ELSE:
    cdef extern from "<symengine/real_mpfr.h>" namespace "SymEngine":
        cdef cppclass RealMPFR(Number):
            pass

IF HAVE_SYMENGINE_MPC:
    cdef extern from "mpc.h":
        ctypedef struct __mpc_struct:
            pass
        ctypedef __mpc_struct mpc_t[1]
        ctypedef __mpc_struct* mpc_ptr
        ctypedef const __mpc_struct* mpc_srcptr

    cdef extern from "<symengine/complex_mpc.h>" namespace "SymEngine":
        cdef cppclass mpc_class:
            mpc_class() nogil
            mpc_class(mpfr_prec_t prec) nogil
            mpc_class(mpc_t m) nogil
            mpc_ptr get_mpc_t() nogil
            mpc_class(string s, mpfr_prec_t prec, unsigned base) nogil

        cdef cppclass ComplexMPC(Number):
            ComplexMPC(mpc_class) nogil
            mpc_class as_mpc() nogil
            mpfr_prec_t get_prec() nogil
            RCP[const Number] real_part() nogil
            RCP[const Number] imaginary_part() nogil

        RCP[const ComplexMPC] complex_mpc(mpc_class t) nogil
ELSE:
    cdef extern from "<symengine/complex_mpc.h>" namespace "SymEngine":
        cdef cppclass ComplexMPC(Number):
            pass

cdef extern from "<symengine/matrix.h>" namespace "SymEngine":
    cdef cppclass MatrixBase:
        const unsigned nrows() nogil
        const unsigned ncols() nogil
        RCP[const Basic] get(unsigned i, unsigned j) nogil
        RCP[const Basic] set(unsigned i, unsigned j, const RCP[const Basic] &e) nogil
        string __str__() nogil except+
        bool eq(const MatrixBase &) nogil
        RCP[const Basic] det() nogil
        void inv(MatrixBase &)
        void add_matrix(const MatrixBase &other, MatrixBase &result) nogil
        void mul_matrix(const MatrixBase &other, MatrixBase &result) nogil
        void add_scalar(const RCP[const Basic] &k, MatrixBase &result) nogil
        void mul_scalar(const RCP[const Basic] &k, MatrixBase &result) nogil
        void transpose(MatrixBase &result) nogil
        void submatrix(unsigned row_start,
                        unsigned row_end,
                        unsigned col_start,
                        unsigned col_end,
                        MatrixBase &result) nogil
        void LU(MatrixBase &L, MatrixBase &U) nogil
        void LDL(MatrixBase &L, MatrixBase &D) nogil
        void LU_solve(const MatrixBase &b, MatrixBase &x) nogil
        void FFLU(MatrixBase &LU) nogil
        void FFLDU(MatrixBase&L, MatrixBase &D, MatrixBase &U) nogil

    cdef cppclass DenseMatrix(MatrixBase):
        DenseMatrix()
        DenseMatrix(unsigned i, unsigned j) nogil
        DenseMatrix(unsigned i, unsigned j, const vec_basic &v) nogil

    bool is_a_DenseMatrix "SymEngine::is_a<SymEngine::DenseMatrix>"(const MatrixBase &b) nogil
    DenseMatrix* static_cast_DenseMatrix "static_cast<SymEngine::DenseMatrix*>"(const MatrixBase *a)
    void inverse_FFLU "SymEngine::inverse_fraction_free_LU"(const DenseMatrix &A,
        DenseMatrix &B) nogil
    void inverse_GJ "SymEngine::inverse_gauss_jordan"(const DenseMatrix &A,
        DenseMatrix &B) nogil
    void FFLU_solve "SymEngine::fraction_free_LU_solve"(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x) nogil
    void FFGJ_solve "SymEngine::fraction_free_gauss_jordan_solve"(const DenseMatrix &A,
        const DenseMatrix &b, DenseMatrix &x) nogil
    void LDL_solve "SymEngine::LDL_solve"(const DenseMatrix &A, const DenseMatrix &b,
        DenseMatrix &x) nogil
    void jacobian "SymEngine::jacobian"(const DenseMatrix &A,
            const DenseMatrix &x, DenseMatrix &result) nogil

cdef extern from "<symengine/ntheory.h>" namespace "SymEngine":
    int probab_prime_p(const Integer &a, int reps)
    RCP[const Integer] nextprime (const Integer &a) nogil
    RCP[const Integer] gcd(const Integer &a, const Integer &b) nogil
    RCP[const Integer] lcm(const Integer &a, const Integer &b) nogil
    void gcd_ext(const Ptr[RCP[Integer]] &g, const Ptr[RCP[Integer]] &s,
            const Ptr[RCP[Integer]] &t, const Integer &a, const Integer &b) nogil
    RCP[const Integer] mod(const Integer &n, const Integer &d) nogil except +
    RCP[const Integer] quotient "SymEngine::quotient_f"(const Integer &n, const Integer &d) nogil except +
    void quotient_mod (const Ptr[RCP[Integer]] &q, const Ptr[RCP[Integer]] &mod,
            const Integer &n, const Integer &d) nogil except +
    int mod_inverse(const Ptr[RCP[Integer]] &b, const Integer &a,
            const Integer &m) nogil
    bool crt(const Ptr[RCP[Integer]] &R, const vec_integer &rem,
           const vec_integer &mod) nogil
    RCP[const Integer] fibonacci(unsigned long n) nogil
    void fibonacci2(const Ptr[RCP[Integer]] &g, const Ptr[RCP[Integer]] &s,
            unsigned long n) nogil
    RCP[const Integer] lucas(unsigned long n) nogil
    void lucas2(const Ptr[RCP[Integer]] &g, const Ptr[RCP[Integer]] &s,
            unsigned long n) nogil
    RCP[const Integer] binomial(const Integer &n,unsigned long k) nogil
    RCP[const Integer] factorial(unsigned long n) nogil
    bool divides(const Integer &a, const Integer &b) nogil
    int factor(const Ptr[RCP[Integer]] &f, const Integer &n, double B1) nogil
    int factor_lehman_method(const Ptr[RCP[Integer]] &f, const Integer &n) nogil
    int factor_pollard_pm1_method(const Ptr[RCP[Integer]] &f, const Integer &n,
            unsigned B, unsigned retries) nogil
    int factor_pollard_rho_method(const Ptr[RCP[Integer]] &f, const Integer &n,
            unsigned retries) nogil
    void prime_factors(vec_integer &primes, const Integer &n) nogil except +
    void prime_factor_multiplicities(map_integer_uint &primes, const Integer &n) nogil except +
    RCP[const Number] bernoulli(unsigned long n) nogil except +
    bool primitive_root(const Ptr[RCP[Integer]] &g, const Integer &n) nogil
    void primitive_root_list(vec_integer &roots, const Integer &n) nogil
    RCP[const Integer] totient(const RCP[const Integer] &n) nogil
    RCP[const Integer] carmichael(const RCP[const Integer] &n) nogil
    bool multiplicative_order(const Ptr[RCP[Integer]] &o, const RCP[const Integer] &a,
            const RCP[const Integer] &n) nogil
    int legendre(const Integer &a, const Integer &n) nogil
    int jacobi(const Integer &a, const Integer &n) nogil
    int kronecker(const Integer &a, const Integer &n) nogil
    void nthroot_mod_list(vec_integer &roots, const RCP[const Integer] &n,
            const RCP[const Integer] &a, const RCP[const Integer] &m) nogil
    bool nthroot_mod(const Ptr[RCP[Integer]] &root, const RCP[const Integer] &n,
            const RCP[const Integer] &a, const RCP[const Integer] &m) nogil
    bool powermod(const Ptr[RCP[Integer]] &powm, const RCP[const Integer] &a,
            const RCP[const Number] &b, const RCP[const Integer] &m) nogil
    void powermod_list(vec_integer &powm, const RCP[const Integer] &a,
            const RCP[const Number] &b, const RCP[const Integer] &m) nogil

    void sieve_generate_primes "SymEngine::Sieve::generate_primes"(vector[unsigned] &primes, unsigned limit) nogil

    cdef cppclass sieve_iterator "SymEngine::Sieve::iterator":
        sieve_iterator()
        sieve_iterator(unsigned limit) nogil
        unsigned next_prime() nogil

cdef extern from "<symengine/visitor.h>" namespace "SymEngine":
    bool has_symbol(const Basic &b, const RCP[const Symbol] &x) nogil except +
    set_basic free_symbols(const Basic &b) nogil except +

cdef extern from "<utility>" namespace "std":
    cdef mpz_class std_move_mpz "std::move" (mpz_class) nogil
    IF HAVE_SYMENGINE_MPFR:
        cdef mpfr_class std_move_mpfr "std::move" (mpfr_class) nogil
    IF HAVE_SYMENGINE_MPC:
        cdef mpc_class std_move_mpc "std::move" (mpc_class) nogil

cdef extern from "<symengine/eval_double.h>" namespace "SymEngine":
    double eval_double(const Basic &b) nogil except +
    double complex eval_complex_double(const Basic &b) nogil except +

IF HAVE_SYMENGINE_MPFR:
    cdef extern from "<symengine/eval_mpfr.h>" namespace "SymEngine":
        void eval_mpfr(mpfr_t result, const Basic &b, mpfr_rnd_t rnd) nogil except +

IF HAVE_SYMENGINE_MPC:
    cdef extern from "<symengine/eval_mpc.h>" namespace "SymEngine":
        void eval_mpc(mpc_t result, const Basic &b, mpfr_rnd_t rnd) nogil except +
