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

cdef extern from "<set>" namespace "std":
    cdef cppclass set[T, U]:
        cppclass iterator:
            T& operator*()
            iterator operator++() nogil
            iterator operator--() nogil
            bint operator==(iterator) nogil
            bint operator!=(iterator) nogil
        iterator begin() nogil
        iterator end() nogil

cdef extern from "symengine_rcp.h" namespace "SymEngine":
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
    RCP[const Sin] rcp_static_cast_Sin "SymEngine::rcp_static_cast<const SymEngine::Sin>"(RCP[const Basic] &b) nogil
    RCP[const Cos] rcp_static_cast_Cos "SymEngine::rcp_static_cast<const SymEngine::Cos>"(RCP[const Basic] &b) nogil
    RCP[const FunctionSymbol] rcp_static_cast_FunctionSymbol "SymEngine::rcp_static_cast<const SymEngine::FunctionSymbol>"(RCP[const Basic] &b) nogil
    RCP[const FunctionWrapper] rcp_static_cast_FunctionWrapper "SymEngine::rcp_static_cast<const SymEngine::FunctionWrapper>"(RCP[const Basic] &b) nogil
    RCP[const Abs] rcp_static_cast_Abs "SymEngine::rcp_static_cast<const SymEngine::Abs>"(RCP[const Basic] &b) nogil
    RCP[const Derivative] rcp_static_cast_Derivative "SymEngine::rcp_static_cast<const SymEngine::Derivative>"(RCP[const Basic] &b) nogil
    RCP[const Subs] rcp_static_cast_Subs "SymEngine::rcp_static_cast<const SymEngine::Subs>"(RCP[const Basic] &b) nogil
    Ptr[RCP[Basic]] outArg(RCP[const Basic] &arg) nogil
    Ptr[RCP[Integer]] outArg_Integer "SymEngine::outArg<SymEngine::RCP<const SymEngine::Integer>>"(RCP[const Integer] &arg) nogil

    void print_stack_on_segfault() nogil


cdef extern from "basic.h" namespace "SymEngine":
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

    bool eq(RCP[const Basic] &a, RCP[const Basic] &b) nogil except +
    bool neq(RCP[const Basic] &a, RCP[const Basic] &b) nogil except +

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
    bool is_a_FunctionSymbol "SymEngine::is_a<SymEngine::FunctionSymbol>"(const Basic &b) nogil
    bool is_a_Abs "SymEngine::is_a<SymEngine::Abs>"(const Basic &b) nogil
    bool is_a_Derivative "SymEngine::is_a<SymEngine::Derivative>"(const Basic &b) nogil
    bool is_a_Subs "SymEngine::is_a<SymEngine::Subs>"(const Basic &b) nogil
    bool is_a_FunctionWrapper "SymEngine::is_a<SymEngine::FunctionWrapper>"(const Basic &b) nogil

    RCP[const Basic] expand(RCP[const Basic] &o) nogil except +


cdef extern from "symbol.h" namespace "SymEngine":
    cdef cppclass Symbol(Basic):
        Symbol(string name) nogil
        string get_name() nogil

cdef extern from "number.h" namespace "SymEngine":
    cdef cppclass Number(Basic):
        pass

cdef extern from "integer.h" namespace "SymEngine":
    cdef cppclass Integer(Number):
        Integer(int i) nogil
        Integer(mpz_class i) nogil
        int compare(const Basic &o) nogil
        mpz_class as_mpz() nogil

cdef extern from "rational.h" namespace "SymEngine":
    cdef cppclass Rational(Number):
        mpq_class as_mpq() nogil
    cdef void get_num_den(const RCP[Rational] &rat, const Ptr[RCP[Integer]] &num,
                     const Ptr[RCP[Integer]] &den) nogil

cdef extern from "complex.h" namespace "SymEngine":
    cdef cppclass Complex(Number):
        RCP[const Number] real_part() nogil
        RCP[const Number] imaginary_part() nogil

cdef extern from "constants.h" namespace "SymEngine":
    cdef cppclass Constant(Basic):
        Constant(string name) nogil
        string get_name() nogil
    RCP[const Basic] I
    RCP[const Basic] E
    RCP[const Basic] pi


cdef extern from "add.h" namespace "SymEngine":
    cdef RCP[const Basic] add(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] sub(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+

    cdef cppclass Add(Basic):
        void as_two_terms(const Ptr[RCP[Basic]] &a, const Ptr[RCP[Basic]] &b)

cdef extern from "mul.h" namespace "SymEngine":
    cdef RCP[const Basic] mul(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] div(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] neg(RCP[const Basic] &a) nogil except+

    cdef cppclass Mul(Basic):
        void as_two_terms(const Ptr[RCP[Basic]] &a, const Ptr[RCP[Basic]] &b)

cdef extern from "pow.h" namespace "SymEngine":
    cdef RCP[const Basic] pow(RCP[const Basic] &a, RCP[const Basic] &b) nogil except+
    cdef RCP[const Basic] sqrt(RCP[const Basic] &x) nogil except+
    cdef RCP[const Basic] exp(RCP[const Basic] &x) nogil except+

    cdef cppclass Pow(Basic):
        RCP[const Basic] get_base() nogil
        RCP[const Basic] get_exp() nogil


cdef extern from "basic.h" namespace "SymEngine":
    # We need to specialize these for our classes:
    RCP[const Basic] rcp(Symbol *p) nogil
    RCP[const Basic] rcp(Constant *p) nogil
    RCP[const Basic] rcp(Integer *p) nogil
    RCP[const Basic] rcp(Subs *p) nogil
    RCP[const Basic] rcp(Derivative *p) nogil
    RCP[const Basic] rcp(FunctionWrapper *p) nogil

cdef extern from "functions.h" namespace "SymEngine":
    cdef RCP[const Basic] sin(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] cos(RCP[const Basic] &arg) nogil except+
    cdef RCP[const Basic] function_symbol(string name, const vec_basic &arg) nogil except+
    cdef RCP[const Basic] abs(RCP[const Basic] &arg) nogil except+

    cdef cppclass Function(Basic):
        pass

    cdef cppclass Sin(Function):
        RCP[const Basic] get_arg() nogil

    cdef cppclass Cos(Function):
        RCP[const Basic] get_arg() nogil

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

cdef extern from "matrix.h" namespace "SymEngine":
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

cdef extern from "ntheory.h" namespace "SymEngine":
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

cdef extern from "eval_double.h" namespace "SymEngine":
    double eval_double(const Basic &b) nogil except +

cdef extern from "visitor.h" namespace "SymEngine":
    bool has_symbol(const Basic &b, const RCP[const Symbol] &x) nogil except +
    set_basic free_symbols(const Basic &b) nogil except +
