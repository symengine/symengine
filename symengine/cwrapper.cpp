#include <cstdlib>
#include <cstring>

#include <symengine/symbol.h>
#include <symengine/cwrapper.h>
#include <symengine/printer.h>
#include <symengine/matrix.h>
#include <symengine/eval.h>
#include <symengine/parser.h>
#define xstr(s) str(s)
#define str(s) #s

using SymEngine::DenseMatrix;
using SymEngine::CSRMatrix;
using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::zero;
using SymEngine::Symbol;
using SymEngine::function_symbol;
using SymEngine::Rational;
using SymEngine::Integer;
using SymEngine::integer_class;
using SymEngine::rational_class;
using SymEngine::Number;
using SymEngine::Complex;
using SymEngine::ComplexDouble;
using SymEngine::RealDouble;
#ifdef HAVE_SYMENGINE_MPFR
using SymEngine::RealMPFR;
using SymEngine::mpfr_class;
#endif // HAVE_SYMENGINE_MPFR
#ifdef HAVE_SYMENGINE_MPC
using SymEngine::ComplexMPC;
#endif // HAVE_SYMENGINE_MPC
using SymEngine::rcp_static_cast;
using SymEngine::is_a;
using SymEngine::RCPBasicKeyLess;
using SymEngine::set_basic;
using SymEngine::get_mpz_t;
using SymEngine::get_mpq_t;
using SymEngine::mp_get_ui;
using SymEngine::mp_get_si;
using SymEngine::eye;
using SymEngine::diag;
using SymEngine::ones;
using SymEngine::zeros;
using SymEngine::parse;
using SymEngine::SymEngineException;

namespace SymEngine
{

template <typename T>
inline bool is_aligned(T *p, size_t n = alignof(T))
{
    return 0 == reinterpret_cast<uintptr_t>(p) % n;
}
}

extern "C" {

#define CWRAPPER_BEGIN try {

#define CWRAPPER_END                                                           \
    return SYMENGINE_NO_EXCEPTION;                                             \
    }                                                                          \
    catch (SymEngineException & e)                                             \
    {                                                                          \
        return e.error_code();                                                 \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        return SYMENGINE_RUNTIME_ERROR;                                        \
    }

struct CRCPBasic {
    SymEngine::RCP<const SymEngine::Basic> m;
};

static_assert(sizeof(CRCPBasic) == sizeof(CRCPBasic_C),
              "Size of 'basic' is not correct");
static_assert(std::alignment_of<CRCPBasic>::value
                  == std::alignment_of<CRCPBasic_C>::value,
              "Alignment of 'basic' is not correct");

void basic_new_stack(basic s)
{
    new (s) CRCPBasic();
}

void basic_free_stack(basic s)
{
    s->m.~RCP();
}

basic_struct *basic_new_heap()
{
    return new CRCPBasic();
}

void basic_free_heap(basic_struct *s)
{
    delete s;
}

void basic_const_set(basic s, const char *c)
{
    s->m = SymEngine::constant(std::string(c));
}

void basic_const_zero(basic s)
{
    s->m = SymEngine::zero;
}

void basic_const_one(basic s)
{
    s->m = SymEngine::one;
}

void basic_const_minus_one(basic s)
{
    s->m = SymEngine::minus_one;
}

void basic_const_I(basic s)
{
    s->m = SymEngine::I;
}

void basic_const_pi(basic s)
{
    s->m = SymEngine::pi;
}

void basic_const_E(basic s)
{
    s->m = SymEngine::E;
}

void basic_const_EulerGamma(basic s)
{
    s->m = SymEngine::EulerGamma;
}

TypeID basic_get_class_id(const char *c)
{
    static std::map<std::string, TypeID> names = {
#define SYMENGINE_INCLUDE_ALL
#define SYMENGINE_ENUM(type, Class)                                            \
    {                                                                          \
        xstr(Class), SYMENGINE_##type                                          \
    }                                                                          \
    ,
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
#undef SYMENGINE_INCLUDE_ALL
        {"", SYMENGINE_TypeID_Count}};

    return names[std::string(c)];
}

char *basic_get_class_from_id(TypeID id)
{
    static std::map<TypeID, std::string> names = {
#define SYMENGINE_INCLUDE_ALL
#define SYMENGINE_ENUM(type, Class)                                            \
    {                                                                          \
        SYMENGINE_##type, xstr(Class)                                          \
    }                                                                          \
    ,
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
#undef SYMENGINE_INCLUDE_ALL
        {SYMENGINE_TypeID_Count, ""}};

    std::string name = names[id];
    auto cc = new char[name.length() + 1];
    std::strcpy(cc, name.c_str());
    return cc;
}

TypeID basic_get_type(const basic s)
{
    return static_cast<TypeID>(s->m->get_type_code());
}

CWRAPPER_OUTPUT_TYPE symbol_set(basic s, const char *c)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::symbol(std::string(c));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE integer_set_si(basic s, long i)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::integer(integer_class(i));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE integer_set_ui(basic s, unsigned long i)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::integer(integer_class(i));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE integer_set_mpz(basic s, const mpz_t i)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::integer(integer_class(i));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE integer_set_str(basic s, const char *c)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::integer(integer_class(c));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE real_double_set_d(basic s, double d)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::real_double(d);
    CWRAPPER_END
}

double real_double_get_d(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealDouble>(*(s->m)));
    return (rcp_static_cast<const RealDouble>(s->m))->as_double();
}

#ifdef HAVE_SYMENGINE_MPFR

CWRAPPER_OUTPUT_TYPE real_mpfr_set_d(basic s, double d, int prec)
{
    CWRAPPER_BEGIN
    mpfr_class mc = mpfr_class(prec);
    mpfr_set_d(mc.get_mpfr_t(), d, MPFR_RNDN);
    s->m = SymEngine::real_mpfr(std::move(mc));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE real_mpfr_set_str(basic s, const char *c, int prec)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::real_mpfr(mpfr_class(c, prec, 10));
    CWRAPPER_END
}

double real_mpfr_get_d(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(s->m)));
    return mpfr_get_d(
        ((rcp_static_cast<const RealMPFR>(s->m))->as_mpfr()).get_mpfr_t(),
        MPFR_RNDN);
}

CWRAPPER_OUTPUT_TYPE real_mpfr_set(basic s, mpfr_srcptr m)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::real_mpfr(mpfr_class(m));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE real_mpfr_get(mpfr_ptr m, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(s->m)));
    mpfr_set(m,
             ((rcp_static_cast<const RealMPFR>(s->m))->as_mpfr()).get_mpfr_t(),
             MPFR_RNDN);
    CWRAPPER_END
}

mpfr_prec_t real_mpfr_get_prec(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(s->m)));
    return ((rcp_static_cast<const RealMPFR>(s->m))->as_mpfr()).get_prec();
}

int real_mpfr_is_zero(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(s->m)));
    return (int)((rcp_static_cast<const RealMPFR>(s->m))->is_zero());
}

#endif // HAVE_SYMENGINE_MPFR
#ifdef HAVE_SYMENGINE_MPC
int complex_mpc_is_zero(const basic s)
{
    SYMENGINE_ASSERT(is_a<ComplexMPC>(*(s->m)));
    return (int)((rcp_static_cast<const ComplexMPC>(s->m))->is_zero());
}

CWRAPPER_OUTPUT_TYPE complex_mpc_real_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<ComplexMPC>(*(com->m)));
    s->m = (rcp_static_cast<const ComplexMPC>(com->m))->real_part();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_mpc_imaginary_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<ComplexMPC>(*(com->m)));
    s->m = (rcp_static_cast<const ComplexMPC>(com->m))->imaginary_part();
    CWRAPPER_END
}

#endif // HAVE_SYMENGINE_MPC
signed long integer_get_si(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    return mp_get_si(
        (rcp_static_cast<const Integer>(s->m))->as_integer_class());
}

unsigned long integer_get_ui(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    return mp_get_ui(
        (rcp_static_cast<const Integer>(s->m))->as_integer_class());
}

CWRAPPER_OUTPUT_TYPE integer_get_mpz(mpz_t a, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    mpz_set(a, get_mpz_t(
                   (rcp_static_cast<const Integer>(s->m))->as_integer_class()));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set_si(basic s, long a, long b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::Rational::from_mpq(rational_class(a, b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::Rational::from_mpq(rational_class(a, b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set(basic s, const basic a, const basic b)
{
    if (not is_a_Integer(a) or not is_a_Integer(b)) {
        return SYMENGINE_RUNTIME_ERROR;
    }
    s->m = SymEngine::Rational::from_two_ints(
        *(rcp_static_cast<const Integer>(a->m)),
        *(rcp_static_cast<const Integer>(b->m)));
    return SYMENGINE_NO_EXCEPTION;
}

CWRAPPER_OUTPUT_TYPE rational_get_mpq(mpq_t a, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Rational>(*(s->m)));
    mpq_set(a, get_mpq_t((rcp_static_cast<const Rational>(s->m))
                             ->as_rational_class()));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set_mpq(basic s, const mpq_t i)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::Rational::from_mpq(rational_class(i));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_set(basic s, const basic re, const basic im)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::Complex::from_two_nums(
        *(rcp_static_cast<const Number>(re->m)),
        *(rcp_static_cast<const Number>(im->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_set_rat(basic s, const basic re, const basic im)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::Complex::from_two_rats(
        *(rcp_static_cast<const Rational>(re->m)),
        *(rcp_static_cast<const Rational>(im->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_set_mpq(basic s, const mpq_t re, const mpq_t im)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::Complex::from_mpq(rational_class(re), rational_class(im));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_real_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Complex>(*(com->m)));
    s->m = (rcp_static_cast<const Complex>(com->m))->real_part();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_imaginary_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Complex>(*(com->m)));
    s->m = (rcp_static_cast<const Complex>(com->m))->imaginary_part();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_double_real_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<ComplexDouble>(*(com->m)));
    s->m = (rcp_static_cast<const ComplexDouble>(com->m))->real_part();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_double_imaginary_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<ComplexDouble>(*(com->m)));
    s->m = (rcp_static_cast<const ComplexDouble>(com->m))->imaginary_part();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_diff(basic s, const basic expr, basic const symbol)
{
    if (not is_a_Symbol(symbol))
        return SYMENGINE_RUNTIME_ERROR;
    s->m = expr->m->diff(rcp_static_cast<const Symbol>(symbol->m));
    return SYMENGINE_NO_EXCEPTION;
}

CWRAPPER_OUTPUT_TYPE basic_assign(basic a, const basic b)
{
    CWRAPPER_BEGIN
    a->m = b->m;
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_parse(basic b, const char *str)
{
    CWRAPPER_BEGIN
    b->m = SymEngine::parse(str);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_add(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::add(a->m, b->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_sub(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::sub(a->m, b->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_mul(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::mul(a->m, b->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_pow(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::pow(a->m, b->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_div(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::div(a->m, b->m);
    CWRAPPER_END
}

int basic_eq(const basic a, const basic b)
{
    return SymEngine::eq(*(a->m), *(b->m)) ? 1 : 0;
}

int basic_neq(const basic a, const basic b)
{
    return SymEngine::neq(*(a->m), *(b->m)) ? 1 : 0;
}

int basic_number_sign(const basic s)
{

    SYMENGINE_ASSERT(is_a_Number(*(s->m)));

    if ((rcp_static_cast<const Number>(s->m))->is_positive()) {
        return 1;
    } else if ((rcp_static_cast<const Number>(s->m))->is_zero()) {
        return 0;
    } else {
        return -1;
    }
}

#define IMPLEMENT_ONE_ARG_FUNC(func)                                           \
    CWRAPPER_OUTPUT_TYPE basic_##func(basic s, const basic a)                  \
    {                                                                          \
        CWRAPPER_BEGIN                                                         \
        s->m = SymEngine::func(a->m);                                          \
        CWRAPPER_END                                                           \
    }

IMPLEMENT_ONE_ARG_FUNC(expand);
IMPLEMENT_ONE_ARG_FUNC(neg);
IMPLEMENT_ONE_ARG_FUNC(abs);
IMPLEMENT_ONE_ARG_FUNC(sin);
IMPLEMENT_ONE_ARG_FUNC(cos);
IMPLEMENT_ONE_ARG_FUNC(tan);
IMPLEMENT_ONE_ARG_FUNC(csc);
IMPLEMENT_ONE_ARG_FUNC(sec);
IMPLEMENT_ONE_ARG_FUNC(cot);
IMPLEMENT_ONE_ARG_FUNC(asin);
IMPLEMENT_ONE_ARG_FUNC(acos);
IMPLEMENT_ONE_ARG_FUNC(asec);
IMPLEMENT_ONE_ARG_FUNC(acsc);
IMPLEMENT_ONE_ARG_FUNC(atan);
IMPLEMENT_ONE_ARG_FUNC(acot);
IMPLEMENT_ONE_ARG_FUNC(sinh);
IMPLEMENT_ONE_ARG_FUNC(cosh);
IMPLEMENT_ONE_ARG_FUNC(tanh);
IMPLEMENT_ONE_ARG_FUNC(csch);
IMPLEMENT_ONE_ARG_FUNC(sech);
IMPLEMENT_ONE_ARG_FUNC(coth);
IMPLEMENT_ONE_ARG_FUNC(asinh);
IMPLEMENT_ONE_ARG_FUNC(acosh);
IMPLEMENT_ONE_ARG_FUNC(asech);
IMPLEMENT_ONE_ARG_FUNC(acsch);
IMPLEMENT_ONE_ARG_FUNC(atanh);
IMPLEMENT_ONE_ARG_FUNC(acoth);
IMPLEMENT_ONE_ARG_FUNC(lambertw);
IMPLEMENT_ONE_ARG_FUNC(zeta);
IMPLEMENT_ONE_ARG_FUNC(dirichlet_eta);
IMPLEMENT_ONE_ARG_FUNC(gamma);

char *basic_str(const basic s)
{
    std::string str = s->m->__str__();
    auto cc = new char[str.length() + 1];
    std::strcpy(cc, str.c_str());
    return cc;
}

void basic_str_free(char *s)
{
    delete[] s;
}

int is_a_Number(const basic s)
{
    return (int)is_a_Number(*(s->m));
}
int is_a_Integer(const basic c)
{
    return is_a<Integer>(*(c->m));
}
int is_a_Rational(const basic c)
{
    return is_a<Rational>(*(c->m));
}
int is_a_Symbol(const basic c)
{
    return is_a<Symbol>(*(c->m));
}
int is_a_Complex(const basic c)
{
    return is_a<Complex>(*(c->m));
}
int is_a_RealDouble(const basic c)
{
    return is_a<RealDouble>(*(c->m));
}
int is_a_ComplexDouble(const basic c)
{
    return is_a<ComplexDouble>(*(c->m));
}
int is_a_RealMPFR(const basic c)
{
#ifdef HAVE_SYMENGINE_MPFR
    return is_a<RealMPFR>(*(c->m));
#else
    return false;
#endif // HAVE_SYMENGINE_MPFR
}
int is_a_ComplexMPC(const basic c)
{
#ifdef HAVE_SYMENGINE_MPC
    return is_a<ComplexMPC>(*(c->m));
#else
    return false;
#endif // HAVE_SYMENGINE_MPC
}

// C wrapper for std::vector<int>

struct CVectorInt {
    std::vector<int> m;
};

CVectorInt *vectorint_new()
{
    return new CVectorInt;
}

int vectorint_placement_new_check(void *data, size_t size)
{
    CVectorInt *self = (CVectorInt *)data;
    if (size < sizeof(CVectorInt))
        return 1;
    if (not SymEngine::is_aligned(self))
        return 2;
    return 0;
}

CVectorInt *vectorint_placement_new(void *data)
{
#if defined(WITH_SYMENGINE_ASSERT)
    // if (size < sizeof(CVectorInt)) return 1; // Requires the 'size' argument
    CVectorInt *self = (CVectorInt *)data;
    SYMENGINE_ASSERT(SymEngine::is_aligned(self));
#endif
    new (data) CVectorInt;
    return (CVectorInt *)data;
}

void vectorint_placement_free(CVectorInt *self)
{
    self->m.~vector<int>();
}

void vectorint_free(CVectorInt *self)
{
    delete self;
}

void vectorint_push_back(CVectorInt *self, int value)
{
    self->m.push_back(value);
}

int vectorint_get(CVectorInt *self, int n)
{
    return self->m[n];
}

// C wrapper for vec_basic

struct CVecBasic {
    SymEngine::vec_basic m;
};

CVecBasic *vecbasic_new()
{
    return new CVecBasic;
}

void vecbasic_free(CVecBasic *self)
{
    delete self;
}

void vecbasic_push_back(CVecBasic *self, const basic value)
{
    self->m.push_back(value->m);
}

void vecbasic_get(CVecBasic *self, int n, basic result)
{
    result->m = self->m[n];
}

size_t vecbasic_size(CVecBasic *self)
{
    return self->m.size();
}

// C wrapper for Matrix

struct CDenseMatrix {
    SymEngine::DenseMatrix m;
};

struct CSparseMatrix {
    SymEngine::CSRMatrix m;
};

CDenseMatrix *dense_matrix_new()
{
    return new CDenseMatrix();
}

CDenseMatrix *dense_matrix_new_vec(unsigned rows, unsigned cols, CVecBasic *l)
{
    return new CDenseMatrix({{rows, cols, l->m}});
}

CDenseMatrix *dense_matrix_new_rows_cols(unsigned rows, unsigned cols)
{
    return new CDenseMatrix({{rows, cols}});
}

CSparseMatrix *sparse_matrix_new()
{
    return new CSparseMatrix;
}

void dense_matrix_free(CDenseMatrix *self)
{
    delete self;
}

void sparse_matrix_free(CSparseMatrix *self)
{
    delete self;
}

void sparse_matrix_init(CSparseMatrix *s)
{
    s->m = SymEngine::CSRMatrix();
}

void sparse_matrix_rows_cols(CSparseMatrix *s, unsigned long int rows,
                             unsigned long int cols)
{
    s->m = SymEngine::CSRMatrix(rows, cols);
}

CWRAPPER_OUTPUT_TYPE dense_matrix_set(CDenseMatrix *s, const CDenseMatrix *d)
{
    CWRAPPER_BEGIN
    s->m = d->m;
    CWRAPPER_END
}

char *dense_matrix_str(const CDenseMatrix *s)
{
    std::string str = s->m.__str__();
    auto cc = new char[str.length() + 1];
    std::strcpy(cc, str.c_str());
    return cc;
}

char *sparse_matrix_str(const CSparseMatrix *s)
{
    std::string str = s->m.__str__();
    auto cc = new char[str.length() + 1];
    std::strcpy(cc, str.c_str());
    return cc;
}

CWRAPPER_OUTPUT_TYPE dense_matrix_rows_cols(CDenseMatrix *mat, unsigned r,
                                            unsigned c)
{
    CWRAPPER_BEGIN
    mat->m.resize(r, c);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_get_basic(basic s, const CDenseMatrix *mat,
                                            unsigned long int r,
                                            unsigned long int c)
{
    CWRAPPER_BEGIN
    s->m = mat->m.get(r, c);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_set_basic(CDenseMatrix *mat,
                                            unsigned long int r,
                                            unsigned long int c, basic s)
{
    CWRAPPER_BEGIN
    mat->m.set(r, c, s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE sparse_matrix_get_basic(basic s, const CSparseMatrix *mat,
                                             unsigned long int r,
                                             unsigned long int c)
{
    CWRAPPER_BEGIN
    s->m = mat->m.get(r, c);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE sparse_matrix_set_basic(CSparseMatrix *mat,
                                             unsigned long int r,
                                             unsigned long int c, basic s)
{
    CWRAPPER_BEGIN
    mat->m.set(r, c, s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_det(basic s, const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    s->m = mat->m.det();
    CWRAPPER_END
}
CWRAPPER_OUTPUT_TYPE dense_matrix_inv(CDenseMatrix *s, const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, mat->m.nrows(), mat->m.ncols());
    mat->m.inv(s->m);
    CWRAPPER_END
}
CWRAPPER_OUTPUT_TYPE dense_matrix_transpose(CDenseMatrix *s,
                                            const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, mat->m.ncols(), mat->m.nrows());
    mat->m.transpose(s->m);
    CWRAPPER_END
}
CWRAPPER_OUTPUT_TYPE
dense_matrix_submatrix(CDenseMatrix *s, const CDenseMatrix *mat,
                       unsigned long int r1, unsigned long int c1,
                       unsigned long int r2, unsigned long int c2,
                       unsigned long int r, unsigned long int c)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, r2 - r1 + 1, c2 - c1 + 1);
    mat->m.submatrix(s->m, r1, c1, r2, c2, r, c);
    CWRAPPER_END
}

unsigned long int dense_matrix_rows(const CDenseMatrix *s)
{
    return s->m.nrows();
}

unsigned long int dense_matrix_cols(const CDenseMatrix *s)
{
    return s->m.ncols();
}

CWRAPPER_OUTPUT_TYPE dense_matrix_add_matrix(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const CDenseMatrix *matB)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, matA->m.nrows(), matA->m.ncols());
    matA->m.add_matrix(matB->m, s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_mul_matrix(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const CDenseMatrix *matB)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, matA->m.nrows(), matB->m.ncols());
    matA->m.mul_matrix(matB->m, s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_add_scalar(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const basic b)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, matA->m.nrows(), matA->m.ncols());
    matA->m.add_scalar(b->m, s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_mul_scalar(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const basic b)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, matA->m.nrows(), matA->m.ncols());
    matA->m.mul_scalar(b->m, s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_LU(CDenseMatrix *l, CDenseMatrix *u,
                                     const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(l, mat->m.nrows(), mat->m.ncols());
    dense_matrix_rows_cols(u, mat->m.nrows(), mat->m.ncols());
    mat->m.LU(l->m, u->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_LDL(CDenseMatrix *l, CDenseMatrix *d,
                                      const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(l, mat->m.nrows(), mat->m.ncols());
    dense_matrix_rows_cols(d, mat->m.nrows(), mat->m.ncols());
    mat->m.LDL(l->m, d->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_FFLU(CDenseMatrix *lu,
                                       const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(lu, mat->m.nrows(), mat->m.ncols());
    mat->m.FFLU(lu->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_FFLDU(CDenseMatrix *l, CDenseMatrix *d,
                                        CDenseMatrix *u,
                                        const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(l, mat->m.nrows(), mat->m.ncols());
    dense_matrix_rows_cols(d, mat->m.nrows(), mat->m.ncols());
    dense_matrix_rows_cols(u, mat->m.nrows(), mat->m.ncols());
    mat->m.FFLDU(l->m, d->m, u->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_LU_solve(CDenseMatrix *x,
                                           const CDenseMatrix *A,
                                           const CDenseMatrix *b)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(x, A->m.ncols(), 1);
    A->m.LU_solve(b->m, x->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_ones(CDenseMatrix *s, unsigned long int r,
                                       unsigned long int c)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, r, c);
    ones(s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_zeros(CDenseMatrix *s, unsigned long int r,
                                        unsigned long int c)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, r, c);
    zeros(s->m);
    CWRAPPER_END
}
CWRAPPER_OUTPUT_TYPE dense_matrix_diag(CDenseMatrix *s, CVecBasic *d,
                                       long int k)
{
    CWRAPPER_BEGIN
    int vec_size = vecbasic_size(d);
    dense_matrix_rows_cols(s, vec_size + (k >= 0 ? k : -k),
                           vec_size + (k >= 0 ? k : -k));
    diag(s->m, d->m, k);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_eye(CDenseMatrix *s, unsigned long int N,
                                      unsigned long int M, int k)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, N, M);
    eye(s->m, k);
    CWRAPPER_END
}

int is_a_DenseMatrix(const CDenseMatrix *c)
{
    return is_a<DenseMatrix>(c->m);
}

int is_a_SparseMatrix(const CSparseMatrix *c)
{
    return is_a<CSRMatrix>(c->m);
}

int dense_matrix_eq(CDenseMatrix *lhs, CDenseMatrix *rhs)
{
    return (lhs->m) == (rhs->m);
}

int sparse_matrix_eq(CSparseMatrix *lhs, CSparseMatrix *rhs)
{
    return (lhs->m) == (rhs->m);
}

// C Wrapper for set_basic

struct CSetBasic {
    SymEngine::set_basic m;
};

CSetBasic *setbasic_new()
{
    return new CSetBasic;
}

void setbasic_free(CSetBasic *self)
{
    delete self;
}

int setbasic_insert(CSetBasic *self, const basic value)
{
    return (self->m.insert(value->m)).second ? 1 : 0;
}

void setbasic_get(CSetBasic *self, int n, basic result)
{
    result->m = *std::next((self->m).begin(), n);
}

int setbasic_find(CSetBasic *self, basic value)
{
    return self->m.find(value->m) != (self->m).end() ? 1 : 0;
}

size_t setbasic_size(CSetBasic *self)
{
    return self->m.size();
}

// C Wrapper for map_basic_basic

struct CMapBasicBasic {
    SymEngine::map_basic_basic m;
};

CMapBasicBasic *mapbasicbasic_new()
{
    return new CMapBasicBasic;
}

void mapbasicbasic_free(CMapBasicBasic *self)
{
    delete self;
}

void mapbasicbasic_insert(CMapBasicBasic *self, const basic key,
                          const basic mapped)
{
    (self->m)[key->m] = mapped->m;
}

int mapbasicbasic_get(CMapBasicBasic *self, const basic key, basic mapped)
{
    auto it = self->m.find(key->m);
    if (it != self->m.end()) {
        mapped->m = it->second;
        return 1;
    }
    return 0;
}

size_t mapbasicbasic_size(CMapBasicBasic *self)
{
    return self->m.size();
}

// ----------------------

CWRAPPER_OUTPUT_TYPE basic_get_args(const basic self, CVecBasic *args)
{
    CWRAPPER_BEGIN
    args->m = self->m->get_args();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_free_symbols(const basic self, CSetBasic *symbols)
{
    CWRAPPER_BEGIN
    symbols->m = SymEngine::free_symbols(*(self->m));
    CWRAPPER_END
}

size_t basic_hash(const basic self)
{
    return self->m->hash();
}

CWRAPPER_OUTPUT_TYPE basic_subs(basic s, const basic e,
                                const CMapBasicBasic *mapbb)
{
    CWRAPPER_BEGIN
    s->m = e->m->subs(mapbb->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_subs2(basic s, const basic e, const basic a,
                                 const basic b)
{
    CWRAPPER_BEGIN
    s->m = e->m->subs({{a->m, b->m}});
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE function_symbol_set(basic s, const char *c,
                                         const CVecBasic *arg)
{
    CWRAPPER_BEGIN
    s->m = function_symbol(c, arg->m);
    CWRAPPER_END
}

// ----------------------

char *ascii_art_str()
{
    std::string str = SymEngine::ascii_art();
    auto cc = new char[str.length() + 1];
    std::strcpy(cc, str.c_str());
    return cc;
}

// Cwrapper for ntheory

CWRAPPER_OUTPUT_TYPE ntheory_gcd(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(b->m)));
    s->m = SymEngine::gcd(static_cast<const Integer &>(*(a->m)),
                          static_cast<const Integer &>(*(b->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_lcm(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(b->m)));
    s->m = SymEngine::lcm(static_cast<const Integer &>(*(a->m)),
                          static_cast<const Integer &>(*(b->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_nextprime(basic s, const basic a)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    s->m = SymEngine::nextprime(static_cast<const Integer &>(*(a->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_mod(basic s, const basic n, const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(n->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(d->m)));
    s->m = SymEngine::mod(static_cast<const Integer &>(*(n->m)),
                          static_cast<const Integer &>(*(d->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_quotient(basic s, const basic n, const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(n->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(d->m)));
    s->m = SymEngine::quotient(static_cast<const Integer &>(*(n->m)),
                               static_cast<const Integer &>(*(d->m)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_fibonacci(basic s, unsigned long a)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::fibonacci(a);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_lucas(basic s, unsigned long a)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::lucas(a);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_binomial(basic s, const basic a, unsigned long b)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    s->m = SymEngine::binomial(static_cast<const Integer &>(*(a->m)), b);
    CWRAPPER_END
}

//! Wrapper for evalf
CWRAPPER_OUTPUT_TYPE basic_evalf(basic s, const basic b, unsigned long bits,
                                 int real)
{

    CWRAPPER_BEGIN
    s->m = SymEngine::evalf(*(b->m), bits, (bool)real);
    CWRAPPER_END
}

//! Print stacktrace on segfault
void symengine_print_stack_on_segfault()
{
    SymEngine::print_stack_on_segfault();
}
}
