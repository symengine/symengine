#include <cstdlib>
#include <cstddef>
#include <cstring>

#include <symengine/symbol.h>
#include <symengine/cwrapper.h>
#include <symengine/printers.h>
#include <symengine/matrix.h>
#include <symengine/eval.h>
#include <symengine/parser.h>
#include <symengine/lambda_double.h>
#include <symengine/solve.h>
#ifdef HAVE_SYMENGINE_LLVM
#include <symengine/llvm_double.h>
using SymEngine::LLVMDoubleVisitor;
using SymEngine::LLVMFloatVisitor;
#ifdef HAVE_SYMENGINE_LLVM_LONG_DOUBLE
using SymEngine::LLVMLongDoubleVisitor;
#endif
#endif

#define xstr(s) ystr(s)
#define ystr(s) #s

using SymEngine::Add;
using SymEngine::Basic;
using SymEngine::Complex;
using SymEngine::ComplexBase;
using SymEngine::ComplexDouble;
using SymEngine::CSRMatrix;
using SymEngine::DenseMatrix;
using SymEngine::down_cast;
using SymEngine::function_symbol;
using SymEngine::FunctionSymbol;
using SymEngine::has_symbol;
using SymEngine::Integer;
using SymEngine::integer_class;
using SymEngine::LambdaRealDoubleVisitor;
using SymEngine::Mul;
using SymEngine::Number;
using SymEngine::Rational;
using SymEngine::rational_class;
using SymEngine::RCP;
using SymEngine::RealDouble;
using SymEngine::Symbol;
using SymEngine::zero;
#ifdef HAVE_SYMENGINE_MPFR
using SymEngine::mpfr_class;
using SymEngine::RealMPFR;
#endif // HAVE_SYMENGINE_MPFR
#ifdef HAVE_SYMENGINE_MPC
using SymEngine::ComplexMPC;
#endif // HAVE_SYMENGINE_MPC
using SymEngine::FiniteSet;
using SymEngine::is_a;
using SymEngine::rcp_static_cast;
using SymEngine::RCPBasicKeyLess;
using SymEngine::Set;
using SymEngine::set_basic;
using SymEngine::vec_basic;
using SymEngine::vec_pair;
using SymEngine::vec_sym;
#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
using SymEngine::get_mpq_t;
using SymEngine::get_mpz_t;
#endif
using SymEngine::ccode;
using SymEngine::CodePrinterPrecision;
using SymEngine::cudacode;
using SymEngine::diag;
using SymEngine::eye;
using SymEngine::jscode;
using SymEngine::julia_str;
using SymEngine::latex;
using SymEngine::mathml;
using SymEngine::metalcode;
using SymEngine::mp_get_si;
using SymEngine::mp_get_ui;
using SymEngine::numeric_cast;
using SymEngine::ones;
using SymEngine::parse;
using SymEngine::SymEngineException;
using SymEngine::zeros;

namespace SymEngine
{

template <typename T>
inline bool is_aligned(T *p, size_t n = alignof(T))
{
    return 0 == reinterpret_cast<uintptr_t>(p) % n;
}

static std::string _str(const Basic &a)
{
    return a.__str__();
}
} // namespace SymEngine

extern "C" {
struct BasicCodePrinterSettings {
    SymEngine::CodePrinterPrecision precision;
};
}

namespace
{

SymEngine::CodePrinterPrecision
to_code_printer_precision(const BasicCodePrinterSettings *settings)
{
    if (settings == nullptr) {
        return SymEngine::CodePrinterPrecision::Double;
    }
    return settings->precision;
}

} // namespace

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

struct CSetBasic {
    SymEngine::set_basic m;
};

static_assert(sizeof(CRCPBasic) == sizeof(CRCPBasic_C),
              "Size of 'basic' is not correct");
static_assert(std::alignment_of<CRCPBasic>::value
                  == std::alignment_of<CRCPBasic_C>::value,
              "Alignment of 'basic' is not correct");

extern "C++" {
static inline SymEngine::RCP<const SymEngine::Basic> &basic_rcp(basic_struct *s)
{
    return reinterpret_cast<CRCPBasic *>(s)->m;
}

static inline const SymEngine::RCP<const SymEngine::Basic> &
basic_rcp(const basic_struct *s)
{
    return reinterpret_cast<const CRCPBasic *>(s)->m;
}
}

void basic_new_stack(basic s)
{
    new (s) CRCPBasic();
}

void basic_free_stack(basic s)
{
    basic_rcp(s).~RCP();
}

basic_struct *basic_new_heap()
{
    return reinterpret_cast<basic_struct *>(new CRCPBasic());
}

void basic_free_heap(basic_struct *s)
{
    delete reinterpret_cast<CRCPBasic *>(s);
}

const char *symengine_version()
{
    return SYMENGINE_VERSION;
}

void basic_const_set(basic s, const char *c)
{
    basic_rcp(s) = SymEngine::constant(std::string(c));
}

void basic_const_zero(basic s)
{
    basic_rcp(s) = SymEngine::zero;
}

void basic_const_one(basic s)
{
    basic_rcp(s) = SymEngine::one;
}

void basic_const_minus_one(basic s)
{
    basic_rcp(s) = SymEngine::minus_one;
}

void basic_const_I(basic s)
{
    basic_rcp(s) = SymEngine::I;
}

void basic_const_pi(basic s)
{
    basic_rcp(s) = SymEngine::pi;
}

void basic_const_E(basic s)
{
    basic_rcp(s) = SymEngine::E;
}

void basic_const_EulerGamma(basic s)
{
    basic_rcp(s) = SymEngine::EulerGamma;
}

void basic_const_Catalan(basic s)
{
    basic_rcp(s) = SymEngine::Catalan;
}

void basic_const_GoldenRatio(basic s)
{
    basic_rcp(s) = SymEngine::GoldenRatio;
}

void basic_const_infinity(basic s)
{
    basic_rcp(s) = SymEngine::Inf;
}

void basic_const_neginfinity(basic s)
{
    basic_rcp(s) = SymEngine::NegInf;
}

void basic_const_complex_infinity(basic s)
{
    basic_rcp(s) = SymEngine::ComplexInf;
}

void basic_const_nan(basic s)
{
    basic_rcp(s) = SymEngine::Nan;
}

TypeID basic_get_class_id(const char *c)
{
    static std::map<std::string, TypeID> names = {
#define SYMENGINE_INCLUDE_ALL
#define SYMENGINE_ENUM(type, Class) {xstr(Class), type},
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
#define SYMENGINE_ENUM(type, Class) {type, xstr(Class)},
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
    return static_cast<TypeID>(basic_rcp(s)->get_type_code());
}

CWRAPPER_OUTPUT_TYPE symbol_set(basic s, const char *c)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::symbol(std::string(c));
    CWRAPPER_END
}

int number_is_zero(const basic s)
{
    SYMENGINE_ASSERT(is_a_Number(*(basic_rcp(s))));
    return (int)((down_cast<const Number &>(*(basic_rcp(s)))).is_zero());
}

int number_is_negative(const basic s)
{
    SYMENGINE_ASSERT(is_a_Number(*(basic_rcp(s))));
    return (int)((down_cast<const Number &>(*(basic_rcp(s)))).is_negative());
}

int number_is_positive(const basic s)
{
    SYMENGINE_ASSERT(is_a_Number(*(basic_rcp(s))));
    return (int)((down_cast<const Number &>(*(basic_rcp(s)))).is_positive());
}

int number_is_complex(const basic s)
{
    SYMENGINE_ASSERT(is_a_Number(*(basic_rcp(s))));
    return (int)((down_cast<const Number &>(*(basic_rcp(s)))).is_complex());
}

int basic_has_symbol(const basic e, const basic s)
{
    return (int)(has_symbol(*(basic_rcp(e)), *(basic_rcp(s))));
}

CWRAPPER_OUTPUT_TYPE integer_set_si(basic s, long i)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::integer(integer_class(i));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE integer_set_ui(basic s, unsigned long i)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::integer(integer_class(i));
    CWRAPPER_END
}

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
CWRAPPER_OUTPUT_TYPE integer_set_mpz(basic s, const mpz_t i)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::integer(integer_class(i));
    CWRAPPER_END
}
#endif

CWRAPPER_OUTPUT_TYPE integer_set_str(basic s, const char *c)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::integer(integer_class(c));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE real_double_set_d(basic s, double d)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::real_double(d);
    CWRAPPER_END
}

double real_double_get_d(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealDouble>(*(basic_rcp(s))));
    return (down_cast<const RealDouble &>(*(basic_rcp(s)))).as_double();
}

#ifdef HAVE_SYMENGINE_MPFR

CWRAPPER_OUTPUT_TYPE real_mpfr_set_d(basic s, double d, int prec)
{
    CWRAPPER_BEGIN
    mpfr_class mc = mpfr_class(prec);
    mpfr_set_d(mc.get_mpfr_t(), d, MPFR_RNDN);
    basic_rcp(s) = SymEngine::real_mpfr(std::move(mc));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE real_mpfr_set_str(basic s, const char *c, int prec)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::real_mpfr(mpfr_class(c, prec, 10));
    CWRAPPER_END
}

double real_mpfr_get_d(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(basic_rcp(s))));
    return mpfr_get_d(
        ((down_cast<const RealMPFR &>(*(basic_rcp(s)))).as_mpfr()).get_mpfr_t(),
        MPFR_RNDN);
}

CWRAPPER_OUTPUT_TYPE real_mpfr_set(basic s, mpfr_srcptr m)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::real_mpfr(mpfr_class(m));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE real_mpfr_get(mpfr_ptr m, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(basic_rcp(s))));
    mpfr_set(
        m,
        ((down_cast<const RealMPFR &>(*(basic_rcp(s)))).as_mpfr()).get_mpfr_t(),
        MPFR_RNDN);
    CWRAPPER_END
}

mpfr_prec_t real_mpfr_get_prec(const basic s)
{
    SYMENGINE_ASSERT(is_a<RealMPFR>(*(basic_rcp(s))));
    return ((down_cast<const RealMPFR &>(*(basic_rcp(s)))).as_mpfr())
        .get_prec();
}

#endif // HAVE_SYMENGINE_MPFR

CWRAPPER_OUTPUT_TYPE complex_base_real_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(SymEngine::is_a_Complex(*(basic_rcp(com))));
    basic_rcp(s)
        = (down_cast<const ComplexBase &>(*(basic_rcp(com)))).real_part();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_base_imaginary_part(basic s, const basic com)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(SymEngine::is_a_Complex(*(basic_rcp(com))));
    basic_rcp(s)
        = (down_cast<const ComplexBase &>(*(basic_rcp(com)))).imaginary_part();
    CWRAPPER_END
}

signed long integer_get_si(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(s))));
    return mp_get_si(
        (down_cast<const Integer &>(*(basic_rcp(s)))).as_integer_class());
}

unsigned long integer_get_ui(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(s))));
    return mp_get_ui(
        (down_cast<const Integer &>(*(basic_rcp(s)))).as_integer_class());
}

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
CWRAPPER_OUTPUT_TYPE integer_get_mpz(mpz_t a, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(s))));
    mpz_set(
        a,
        get_mpz_t(
            (down_cast<const Integer &>(*(basic_rcp(s)))).as_integer_class()));
    CWRAPPER_END
}
#endif

CWRAPPER_OUTPUT_TYPE rational_set_si(basic s, long a, long b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::Rational::from_mpq(rational_class(a, b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::Rational::from_mpq(rational_class(a, b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set(basic s, const basic a, const basic b)
{
    if (not is_a_Integer(a) or not is_a_Integer(b)) {
        return SYMENGINE_RUNTIME_ERROR;
    }
    basic_rcp(s) = SymEngine::Rational::from_two_ints(
        *(rcp_static_cast<const Integer>(basic_rcp(a))),
        *(rcp_static_cast<const Integer>(basic_rcp(b))));
    return SYMENGINE_NO_EXCEPTION;
}

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
CWRAPPER_OUTPUT_TYPE rational_get_mpq(mpq_t a, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Rational>(*(basic_rcp(s))));
    mpq_set(a, get_mpq_t((down_cast<const Rational &>(*(basic_rcp(s))))
                             .as_rational_class()));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE rational_set_mpq(basic s, const mpq_t i)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::Rational::from_mpq(rational_class(i));
    CWRAPPER_END
}
#endif

CWRAPPER_OUTPUT_TYPE complex_set(basic s, const basic re, const basic im)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::Complex::from_two_nums(
        *(rcp_static_cast<const Number>(basic_rcp(re))),
        *(rcp_static_cast<const Number>(basic_rcp(im))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE complex_set_rat(basic s, const basic re, const basic im)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::Complex::from_two_rats(
        *(rcp_static_cast<const Rational>(basic_rcp(re))),
        *(rcp_static_cast<const Rational>(basic_rcp(im))));
    CWRAPPER_END
}

#if SYMENGINE_INTEGER_CLASS != SYMENGINE_BOOSTMP
CWRAPPER_OUTPUT_TYPE complex_set_mpq(basic s, const mpq_t re, const mpq_t im)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = SymEngine::Complex::from_mpq(rational_class(re), rational_class(im));
    CWRAPPER_END
}
#endif

dcomplex complex_double_get(const basic s)
{
    SYMENGINE_ASSERT(is_a<ComplexDouble>(*(basic_rcp(s))));
    dcomplex d;
    d.real = (down_cast<const ComplexDouble &>(*(basic_rcp(s)))
                  .as_complex_double())
                 .real();
    d.imag = (down_cast<const ComplexDouble &>(*(basic_rcp(s)))
                  .as_complex_double())
                 .imag();
    return d;
}

char *basic_dumps(const basic s, unsigned long *size)
{
    std::string str = basic_rcp(s)->dumps();
    *size = str.length();
    auto cc = new char[*size];
    str.copy(cc, *size);
    return cc;
}

CWRAPPER_OUTPUT_TYPE basic_loads(basic s, const char *c, unsigned long size)
{
    CWRAPPER_BEGIN
    std::string data(c, size);
    basic_rcp(s) = Basic::loads(data);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_diff(basic s, const basic expr, basic const symbol)
{
    if (not is_a_Symbol(symbol))
        return SYMENGINE_RUNTIME_ERROR;
    CWRAPPER_BEGIN
    basic_rcp(s) = basic_rcp(expr)->diff(
        rcp_static_cast<const Symbol>(basic_rcp(symbol)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_series(basic s, const basic expr, basic const sym,
                                  unsigned int prec)
{
    CWRAPPER_BEGIN
    s->m = SymEngine::series(expr->m, rcp_static_cast<const Symbol>(sym->m),
                             prec);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_assign(basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(a) = basic_rcp(b);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_parse(basic b, const char *str)
{
    CWRAPPER_BEGIN
    basic_rcp(b) = SymEngine::parse(str);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_parse2(basic b, const char *str, int convert_xor)
{
    CWRAPPER_BEGIN
    if (convert_xor > 0) {
        basic_rcp(b) = SymEngine::parse(str);
    } else {
        basic_rcp(b) = SymEngine::parse(str, false);
    }
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_add(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::add(basic_rcp(a), basic_rcp(b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_sub(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::sub(basic_rcp(a), basic_rcp(b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_mul(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::mul(basic_rcp(a), basic_rcp(b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_pow(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::pow(basic_rcp(a), basic_rcp(b));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_div(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::div(basic_rcp(a), basic_rcp(b));
    CWRAPPER_END
}

int basic_eq(const basic a, const basic b)
{
    return SymEngine::eq(*(basic_rcp(a)), *(basic_rcp(b))) ? 1 : 0;
}

int basic_neq(const basic a, const basic b)
{
    return SymEngine::neq(*(basic_rcp(a)), *(basic_rcp(b))) ? 1 : 0;
}

#define IMPLEMENT_ONE_ARG_FUNC(func)                                           \
    CWRAPPER_OUTPUT_TYPE basic_##func(basic s, const basic a)                  \
    {                                                                          \
        CWRAPPER_BEGIN                                                         \
        basic_rcp(s) = SymEngine::func(basic_rcp(a));                          \
        CWRAPPER_END                                                           \
    }

IMPLEMENT_ONE_ARG_FUNC(expand)
IMPLEMENT_ONE_ARG_FUNC(neg)
IMPLEMENT_ONE_ARG_FUNC(abs)
IMPLEMENT_ONE_ARG_FUNC(erf)
IMPLEMENT_ONE_ARG_FUNC(erfc)
IMPLEMENT_ONE_ARG_FUNC(sin)
IMPLEMENT_ONE_ARG_FUNC(cos)
IMPLEMENT_ONE_ARG_FUNC(tan)
IMPLEMENT_ONE_ARG_FUNC(csc)
IMPLEMENT_ONE_ARG_FUNC(sec)
IMPLEMENT_ONE_ARG_FUNC(cot)
IMPLEMENT_ONE_ARG_FUNC(asin)
IMPLEMENT_ONE_ARG_FUNC(acos)
IMPLEMENT_ONE_ARG_FUNC(asec)
IMPLEMENT_ONE_ARG_FUNC(acsc)
IMPLEMENT_ONE_ARG_FUNC(atan)
IMPLEMENT_ONE_ARG_FUNC(acot)
IMPLEMENT_ONE_ARG_FUNC(sinh)
IMPLEMENT_ONE_ARG_FUNC(cosh)
IMPLEMENT_ONE_ARG_FUNC(tanh)
IMPLEMENT_ONE_ARG_FUNC(csch)
IMPLEMENT_ONE_ARG_FUNC(sech)
IMPLEMENT_ONE_ARG_FUNC(coth)
IMPLEMENT_ONE_ARG_FUNC(asinh)
IMPLEMENT_ONE_ARG_FUNC(acosh)
IMPLEMENT_ONE_ARG_FUNC(asech)
IMPLEMENT_ONE_ARG_FUNC(acsch)
IMPLEMENT_ONE_ARG_FUNC(atanh)
IMPLEMENT_ONE_ARG_FUNC(acoth)
IMPLEMENT_ONE_ARG_FUNC(lambertw)
IMPLEMENT_ONE_ARG_FUNC(zeta)
IMPLEMENT_ONE_ARG_FUNC(dirichlet_eta)
IMPLEMENT_ONE_ARG_FUNC(gamma)
IMPLEMENT_ONE_ARG_FUNC(loggamma)
IMPLEMENT_ONE_ARG_FUNC(sqrt)
IMPLEMENT_ONE_ARG_FUNC(cbrt)
IMPLEMENT_ONE_ARG_FUNC(exp)
IMPLEMENT_ONE_ARG_FUNC(log)
IMPLEMENT_ONE_ARG_FUNC(floor)
IMPLEMENT_ONE_ARG_FUNC(ceiling)
IMPLEMENT_ONE_ARG_FUNC(sign)

#define IMPLEMENT_TWO_ARG_FUNC(func)                                           \
    CWRAPPER_OUTPUT_TYPE basic_##func(basic s, const basic a, const basic b)   \
    {                                                                          \
        CWRAPPER_BEGIN                                                         \
        basic_rcp(s) = SymEngine::func(basic_rcp(a), basic_rcp(b));            \
        CWRAPPER_END                                                           \
    }

IMPLEMENT_TWO_ARG_FUNC(atan2)
IMPLEMENT_TWO_ARG_FUNC(kronecker_delta)
IMPLEMENT_TWO_ARG_FUNC(lowergamma)
IMPLEMENT_TWO_ARG_FUNC(uppergamma)
IMPLEMENT_TWO_ARG_FUNC(beta)
IMPLEMENT_TWO_ARG_FUNC(polygamma)

#define IMPLEMENT_STR_CONVERSION(name, func)                                   \
    char *basic_##name(const basic s)                                          \
    {                                                                          \
        std::string str;                                                       \
        try {                                                                  \
            str = func(*basic_rcp(s));                                         \
        } catch (SymEngineException & e) {                                     \
            return nullptr;                                                    \
        } catch (...) {                                                        \
            return nullptr;                                                    \
        }                                                                      \
        auto cc = new char[str.length() + 1];                                  \
        std::strcpy(cc, str.c_str());                                          \
        return cc;                                                             \
    }

#define IMPLEMENT_STR_CONVERSION_SETTINGS(name, func)                          \
    char *basic_##name(const basic s,                                          \
                       const BasicCodePrinterSettings *settings)               \
    {                                                                          \
        std::string str;                                                       \
        try {                                                                  \
            str = settings == nullptr                                          \
                      ? func(*basic_rcp(s))                                    \
                      : func(*basic_rcp(s),                                    \
                             to_code_printer_precision(settings));             \
        } catch (SymEngineException & e) {                                     \
            return nullptr;                                                    \
        } catch (...) {                                                        \
            return nullptr;                                                    \
        }                                                                      \
        auto cc = new char[str.length() + 1];                                  \
        std::strcpy(cc, str.c_str());                                          \
        return cc;                                                             \
    }

IMPLEMENT_STR_CONVERSION(str, _str)
IMPLEMENT_STR_CONVERSION(str_julia, julia_str)
IMPLEMENT_STR_CONVERSION(str_mathml, mathml)
IMPLEMENT_STR_CONVERSION(str_latex, latex)
IMPLEMENT_STR_CONVERSION(str_jscode, jscode)
IMPLEMENT_STR_CONVERSION(str_ccode, ccode)
IMPLEMENT_STR_CONVERSION(str_cudacode, cudacode)
IMPLEMENT_STR_CONVERSION(str_metalcode, metalcode)
IMPLEMENT_STR_CONVERSION_SETTINGS(str_ccode_settings, ccode)
IMPLEMENT_STR_CONVERSION_SETTINGS(str_cudacode_settings, cudacode)
IMPLEMENT_STR_CONVERSION_SETTINGS(str_metalcode_settings, metalcode)

BasicCodePrinterSettings *basic_code_printer_settings_new()
{
    return new BasicCodePrinterSettings;
}

void basic_code_printer_settings_free(BasicCodePrinterSettings *self)
{
    delete self;
}

void basic_code_printer_settings_set_precision(BasicCodePrinterSettings *self,
                                               BasicCodePrinterPrecision prec)
{
    self->precision = static_cast<SymEngine::CodePrinterPrecision>(prec);
}

void basic_str_free(char *s)
{
    delete[] s;
}

void bool_set_true(basic s)
{
    basic_rcp(s) = SymEngine::boolTrue;
}

void bool_set_false(basic s)
{
    basic_rcp(s) = SymEngine::boolFalse;
}

CWRAPPER_OUTPUT_TYPE basic_set_interval(basic s, const basic start,
                                        const basic end, int left_open,
                                        int right_open)
{
    SYMENGINE_ASSERT(is_a_Number(*(basic_rcp(start))));
    SYMENGINE_ASSERT(is_a_Number(*(basic_rcp(end))));

    CWRAPPER_BEGIN
    basic_rcp(s)
        = SymEngine::interval(rcp_static_cast<const Number>(basic_rcp(start)),
                              rcp_static_cast<const Number>(basic_rcp(end)),
                              (bool)left_open, (bool)right_open);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_finiteset(basic s, const CSetBasic *container)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::finiteset(container->m);
    CWRAPPER_END
}

void basic_set_emptyset(basic s)
{
    basic_rcp(s) = SymEngine::emptyset();
}

void basic_set_universalset(basic s)
{
    basic_rcp(s) = SymEngine::emptyset();
}

void basic_set_complexes(basic s)
{
    basic_rcp(s) = SymEngine::complexes();
}

void basic_set_reals(basic s)
{
    basic_rcp(s) = SymEngine::reals();
}

void basic_set_rationals(basic s)
{
    basic_rcp(s) = SymEngine::rationals();
}

void basic_set_integers(basic s)
{
    basic_rcp(s) = SymEngine::integers();
}

CWRAPPER_OUTPUT_TYPE basic_set_union(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = rcp_static_cast<const Set>(basic_rcp(a))
                       ->set_union(rcp_static_cast<const Set>(basic_rcp(b)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_intersection(basic s, const basic a,
                                            const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = rcp_static_cast<const Set>(basic_rcp(a))
              ->set_intersection(rcp_static_cast<const Set>(basic_rcp(b)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_complement(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = rcp_static_cast<const Set>(basic_rcp(a))
              ->set_complement(rcp_static_cast<const Set>(basic_rcp(b)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_contains(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = rcp_static_cast<const Set>(basic_rcp(a))->contains(basic_rcp(b));
    CWRAPPER_END
}

int basic_set_is_subset(const basic a, const basic b)
{
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(b))));
    return rcp_static_cast<const Set>(basic_rcp(a))
        ->is_subset(rcp_static_cast<const Set>(basic_rcp(b)));
}

int basic_set_is_proper_subset(const basic a, const basic b)
{
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(b))));
    return rcp_static_cast<const Set>(basic_rcp(a))
        ->is_proper_subset(rcp_static_cast<const Set>(basic_rcp(b)));
}

int basic_set_is_superset(const basic a, const basic b)
{
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(b))));
    return rcp_static_cast<const Set>(basic_rcp(a))
        ->is_superset(rcp_static_cast<const Set>(basic_rcp(b)));
}

int basic_set_is_proper_superset(const basic a, const basic b)
{
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a_Set(*(basic_rcp(b))));
    return rcp_static_cast<const Set>(basic_rcp(a))
        ->is_proper_superset(rcp_static_cast<const Set>(basic_rcp(b)));
}

CWRAPPER_OUTPUT_TYPE basic_set_inf(basic s, const basic a)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::inf(*rcp_static_cast<const Set>(basic_rcp(a)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_sup(basic s, const basic a)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::sup(*rcp_static_cast<const Set>(basic_rcp(a)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_boundary(basic s, const basic a)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = SymEngine::boundary(*rcp_static_cast<const Set>(basic_rcp(a)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_interior(basic s, const basic a)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = SymEngine::interior(*rcp_static_cast<const Set>(basic_rcp(a)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_set_closure(basic s, const basic a)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = SymEngine::closure(*rcp_static_cast<const Set>(basic_rcp(a)));
    CWRAPPER_END
}

int symengine_have_component(const char *c)
{
#ifdef HAVE_SYMENGINE_MPFR
    if (std::strcmp("mpfr", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_MPC
    if (std::strcmp("mpc", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_FLINT
    if (std::strcmp("flint", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_ARB
    if (std::strcmp("arb", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_ECM
    if (std::strcmp("ecm", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_PRIMESIEVE
    if (std::strcmp("primesieve", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_PIRANHA
    if (std::strcmp("piranha", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_BOOST
    if (std::strcmp("boost", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_PTHREAD
    if (std::strcmp("pthread", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_LLVM
    if (std::strcmp("llvm", c) == 0)
        return 1;
#endif
#ifdef HAVE_SYMENGINE_LLVM_LONG_DOUBLE
    if (std::strcmp("llvm_long_double", c) == 0)
        return 1;
#endif
    return 0;
}

int is_a_Number(const basic s)
{
    return (int)is_a_Number(*(basic_rcp(s)));
}
int is_a_Integer(const basic c)
{
    return is_a<Integer>(*(basic_rcp(c)));
}
int is_a_Rational(const basic c)
{
    return is_a<Rational>(*(basic_rcp(c)));
}
int is_a_Symbol(const basic c)
{
    return is_a<Symbol>(*(basic_rcp(c)));
}
int is_a_Complex(const basic c)
{
    return is_a<Complex>(*(basic_rcp(c)));
}
int is_a_RealDouble(const basic c)
{
    return is_a<RealDouble>(*(basic_rcp(c)));
}
int is_a_ComplexDouble(const basic c)
{
    return is_a<ComplexDouble>(*(basic_rcp(c)));
}
int is_a_RealMPFR(const basic c)
{
#ifdef HAVE_SYMENGINE_MPFR
    return is_a<RealMPFR>(*(basic_rcp(c)));
#else
    return false;
#endif // HAVE_SYMENGINE_MPFR
}
int is_a_ComplexMPC(const basic c)
{
#ifdef HAVE_SYMENGINE_MPC
    return is_a<ComplexMPC>(*(basic_rcp(c)));
#else
    return false;
#endif // HAVE_SYMENGINE_MPC
}
int is_a_Set(const basic c)
{
    return SymEngine::is_a_Set(*(basic_rcp(c)));
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

CWRAPPER_OUTPUT_TYPE vecbasic_push_back(CVecBasic *self, const basic value)
{
    CWRAPPER_BEGIN

    self->m.push_back(basic_rcp(value));

    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE vecbasic_get(CVecBasic *self, size_t n, basic result)
{
    CWRAPPER_BEGIN

    SYMENGINE_ASSERT(n < self->m.size());
    basic_rcp(result) = self->m[n];

    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE vecbasic_set(CVecBasic *self, size_t n, const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(n < self->m.size());
    self->m[n] = basic_rcp(s);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE vecbasic_erase(CVecBasic *self, size_t n)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(n < self->m.size());
    self->m.erase(self->m.begin() + n);
    CWRAPPER_END
}

size_t vecbasic_size(CVecBasic *self)
{
    return self->m.size();
}

CWRAPPER_OUTPUT_TYPE basic_max(basic s, const CVecBasic *d)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::max(d->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_min(basic s, const CVecBasic *d)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::min(d->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_add_vec(basic s, const CVecBasic *d)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::add(d->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_mul_vec(basic s, const CVecBasic *d)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::mul(d->m);
    CWRAPPER_END
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
    s->m = SymEngine::CSRMatrix(numeric_cast<unsigned>(rows),
                                numeric_cast<unsigned>(cols));
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
    basic_rcp(s)
        = mat->m.get(numeric_cast<unsigned>(r), numeric_cast<unsigned>(c));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_set_basic(CDenseMatrix *mat,
                                            unsigned long int r,
                                            unsigned long int c, basic s)
{
    CWRAPPER_BEGIN
    mat->m.set(numeric_cast<unsigned>(r), numeric_cast<unsigned>(c),
               basic_rcp(s));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE sparse_matrix_get_basic(basic s, const CSparseMatrix *mat,
                                             unsigned long int r,
                                             unsigned long int c)
{
    CWRAPPER_BEGIN
    basic_rcp(s)
        = mat->m.get(numeric_cast<unsigned>(r), numeric_cast<unsigned>(c));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE sparse_matrix_set_basic(CSparseMatrix *mat,
                                             unsigned long int r,
                                             unsigned long int c, basic s)
{
    CWRAPPER_BEGIN
    mat->m.set(numeric_cast<unsigned>(r), numeric_cast<unsigned>(c),
               basic_rcp(s));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_det(basic s, const CDenseMatrix *mat)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = mat->m.det();
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
    dense_matrix_rows_cols(s, numeric_cast<unsigned>(r2 - r1 + 1),
                           numeric_cast<unsigned>(c2 - c1 + 1));
    mat->m.submatrix(s->m, numeric_cast<unsigned>(r1),
                     numeric_cast<unsigned>(c1), numeric_cast<unsigned>(r2),
                     numeric_cast<unsigned>(c2), numeric_cast<unsigned>(r),
                     numeric_cast<unsigned>(c));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_row_join(CDenseMatrix *A,
                                           const CDenseMatrix *B)
{
    CWRAPPER_BEGIN
    A->m.row_join(B->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_col_join(CDenseMatrix *A,
                                           const CDenseMatrix *B)
{
    CWRAPPER_BEGIN
    A->m.col_join(B->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_row_del(CDenseMatrix *A, unsigned k)
{
    CWRAPPER_BEGIN
    A->m.row_del(k);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_col_del(CDenseMatrix *A, unsigned k)
{
    CWRAPPER_BEGIN
    A->m.col_del(k);
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
    matA->m.add_scalar(basic_rcp(b), s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_mul_scalar(CDenseMatrix *s,
                                             const CDenseMatrix *matA,
                                             const basic b)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, matA->m.nrows(), matA->m.ncols());
    matA->m.mul_scalar(basic_rcp(b), s->m);
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
    dense_matrix_rows_cols(s, numeric_cast<unsigned>(r),
                           numeric_cast<unsigned>(c));
    ones(s->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_zeros(CDenseMatrix *s, unsigned long int r,
                                        unsigned long int c)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, numeric_cast<unsigned>(r),
                           numeric_cast<unsigned>(c));
    zeros(s->m);
    CWRAPPER_END
}
CWRAPPER_OUTPUT_TYPE dense_matrix_diag(CDenseMatrix *s, CVecBasic *d,
                                       long int k)
{
    CWRAPPER_BEGIN
    unsigned int vec_size = numeric_cast<unsigned>(vecbasic_size(d));
    dense_matrix_rows_cols(
        s, numeric_cast<unsigned>(vec_size + (k >= 0 ? k : -k)),
        numeric_cast<unsigned>(vec_size + (k >= 0 ? k : -k)));
    diag(s->m, d->m, numeric_cast<int>(k));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_eye(CDenseMatrix *s, unsigned long int N,
                                      unsigned long int M, int k)
{
    CWRAPPER_BEGIN
    dense_matrix_rows_cols(s, numeric_cast<unsigned>(N),
                           numeric_cast<unsigned>(M));
    eye(s->m, k);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_diff(CDenseMatrix *result,
                                       const CDenseMatrix *A, basic const x)
{
    if (not is_a_Symbol(x))
        return SYMENGINE_RUNTIME_ERROR;
    CWRAPPER_BEGIN
    diff(A->m, rcp_static_cast<const Symbol>(basic_rcp(x)), result->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE dense_matrix_jacobian(CDenseMatrix *result,
                                           const CDenseMatrix *A,
                                           const CDenseMatrix *x)
{
    CWRAPPER_BEGIN
    jacobian(A->m, x->m, result->m);
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
    return (self->m.insert(basic_rcp(value))).second ? 1 : 0;
}

void setbasic_get(CSetBasic *self, int n, basic result)
{
    basic_rcp(result) = *std::next((self->m).begin(), n);
}

int setbasic_find(CSetBasic *self, basic value)
{
    return self->m.find(basic_rcp(value)) != (self->m).end() ? 1 : 0;
}

int setbasic_erase(CSetBasic *self, const basic value)
{
    return (self->m.erase(basic_rcp(value))) ? 1 : 0;
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
    (self->m)[basic_rcp(key)] = basic_rcp(mapped);
}

int mapbasicbasic_get(CMapBasicBasic *self, const basic key, basic mapped)
{
    auto it = self->m.find(basic_rcp(key));
    if (it != self->m.end()) {
        basic_rcp(mapped) = it->second;
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
    args->m = basic_rcp(self)->get_args();
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_free_symbols(const basic self, CSetBasic *symbols)
{
    CWRAPPER_BEGIN
    symbols->m = SymEngine::free_symbols(*(basic_rcp(self)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_function_symbols(CSetBasic *symbols,
                                            const basic self)
{
    CWRAPPER_BEGIN
    symbols->m
        = SymEngine::atoms<SymEngine::FunctionSymbol>(*(basic_rcp(self)));
    CWRAPPER_END
}

size_t basic_hash(const basic self)
{
    return static_cast<size_t>(basic_rcp(self)->hash());
}

CWRAPPER_OUTPUT_TYPE basic_subs(basic s, const basic e,
                                const CMapBasicBasic *mapbb)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = basic_rcp(e)->subs(mapbb->m);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_subs2(basic s, const basic e, const basic a,
                                 const basic b)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = basic_rcp(e)->subs({{basic_rcp(a), basic_rcp(b)}});
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE function_symbol_set(basic s, const char *c,
                                         const CVecBasic *arg)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = function_symbol(c, arg->m);
    CWRAPPER_END
}

char *function_symbol_get_name(const basic b)
{
    SYMENGINE_ASSERT(is_a<FunctionSymbol>(*(basic_rcp(b))));
    std::string str
        = down_cast<const FunctionSymbol &>(*(basic_rcp(b))).get_name();
    auto cc = new char[str.length() + 1];
    std::strcpy(cc, str.c_str());
    return cc;
}

CWRAPPER_OUTPUT_TYPE basic_coeff(basic c, const basic b, const basic x,
                                 const basic n)
{
    CWRAPPER_BEGIN
    basic_rcp(c)
        = SymEngine::coeff(*(basic_rcp(b)), *(basic_rcp(x)), *(basic_rcp(n)));
    CWRAPPER_END
}

// ----------------------

CWRAPPER_OUTPUT_TYPE vecbasic_linsolve(CVecBasic *sol, const CVecBasic *sys,
                                       const CVecBasic *sym)
{
    CWRAPPER_BEGIN
    vec_basic vb = sym->m;
    SYMENGINE_ASSERT(
        std::all_of(vb.cbegin(), vb.cend(),
                    [](RCP<const Basic> b) { return is_a<const Symbol>(*b); }));
    vec_sym vs(vb.size());
    for (unsigned i = 0; i < vb.size(); i++)
        vs[i] = rcp_static_cast<const Symbol>(vb[i]);
    sol->m = SymEngine::linsolve(sys->m, vs);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_solve_poly(CSetBasic *r, const basic f,
                                      const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Symbol>(*(basic_rcp(s))));
    RCP<const Set> set = SymEngine::solve_poly(
        basic_rcp(f), rcp_static_cast<const Symbol>(basic_rcp(s)));
    if (not is_a<FiniteSet>(*set)) {
        return SYMENGINE_NOT_IMPLEMENTED;
    }
    r->m = down_cast<const FiniteSet &>(*set).get_container();
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
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(b))));
    basic_rcp(s) = SymEngine::gcd(down_cast<const Integer &>(*(basic_rcp(a))),
                                  down_cast<const Integer &>(*(basic_rcp(b))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_lcm(basic s, const basic a, const basic b)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(b))));
    basic_rcp(s) = SymEngine::lcm(down_cast<const Integer &>(*(basic_rcp(a))),
                                  down_cast<const Integer &>(*(basic_rcp(b))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_gcd_ext(basic g, basic s, basic t, const basic a,
                                     const basic b)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(b))));
    SymEngine::RCP<const Integer> g_, s_, t_;
    SymEngine::gcd_ext(SymEngine::outArg(g_), SymEngine::outArg(s_),
                       SymEngine::outArg(t_),
                       down_cast<const Integer &>(*(basic_rcp(a))),
                       down_cast<const Integer &>(*(basic_rcp(b))));
    basic_rcp(g) = g_;
    basic_rcp(s) = s_;
    basic_rcp(t) = t_;
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_nextprime(basic s, const basic a)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(a))));
    basic_rcp(s)
        = SymEngine::nextprime(down_cast<const Integer &>(*(basic_rcp(a))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_mod(basic s, const basic n, const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(n))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(d))));
    basic_rcp(s) = SymEngine::mod(down_cast<const Integer &>(*(basic_rcp(n))),
                                  down_cast<const Integer &>(*(basic_rcp(d))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_quotient(basic s, const basic n, const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(n))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(d))));
    basic_rcp(s)
        = SymEngine::quotient(down_cast<const Integer &>(*(basic_rcp(n))),
                              down_cast<const Integer &>(*(basic_rcp(d))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_quotient_mod(basic q, basic r, const basic n,
                                          const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(n))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(d))));
    SymEngine::RCP<const Integer> q_, r_;
    SymEngine::quotient_mod(SymEngine::outArg(q_), SymEngine::outArg(r_),
                            down_cast<const Integer &>(*(basic_rcp(n))),
                            down_cast<const Integer &>(*(basic_rcp(d))));
    basic_rcp(q) = q_;
    basic_rcp(r) = r_;
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_mod_f(basic s, const basic n, const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(n))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(d))));
    basic_rcp(s)
        = SymEngine::mod_f(down_cast<const Integer &>(*(basic_rcp(n))),
                           down_cast<const Integer &>(*(basic_rcp(d))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_quotient_f(basic s, const basic n, const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(n))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(d))));
    basic_rcp(s)
        = SymEngine::quotient_f(down_cast<const Integer &>(*(basic_rcp(n))),
                                down_cast<const Integer &>(*(basic_rcp(d))));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_quotient_mod_f(basic q, basic r, const basic n,
                                            const basic d)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(n))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(d))));
    SymEngine::RCP<const Integer> q_, r_;
    SymEngine::quotient_mod_f(SymEngine::outArg(q_), SymEngine::outArg(r_),
                              down_cast<const Integer &>(*(basic_rcp(n))),
                              down_cast<const Integer &>(*(basic_rcp(d))));
    basic_rcp(q) = q_;
    basic_rcp(r) = r_;
    CWRAPPER_END
}

int ntheory_mod_inverse(basic b, const basic a, const basic m)
{
    int ret_val;
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(a))));
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(m))));
    SymEngine::RCP<const Integer> b_;
    ret_val = SymEngine::mod_inverse(
        SymEngine::outArg(b_), down_cast<const Integer &>(*(basic_rcp(a))),
        down_cast<const Integer &>(*(basic_rcp(m))));
    basic_rcp(b) = b_;
    return ret_val;
}

CWRAPPER_OUTPUT_TYPE ntheory_fibonacci(basic s, unsigned long a)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::fibonacci(a);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_fibonacci2(basic g, basic s, unsigned long a)
{
    CWRAPPER_BEGIN
    SymEngine::RCP<const Integer> g_, s_;
    SymEngine::fibonacci2(SymEngine::outArg(g_), SymEngine::outArg(s_), a);
    basic_rcp(g) = g_;
    basic_rcp(s) = s_;
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_lucas(basic s, unsigned long a)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::lucas(a);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_lucas2(basic g, basic s, unsigned long a)
{
    CWRAPPER_BEGIN
    SymEngine::RCP<const Integer> g_, s_;
    SymEngine::lucas2(SymEngine::outArg(g_), SymEngine::outArg(s_), a);
    basic_rcp(g) = g_;
    basic_rcp(s) = s_;
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_binomial(basic s, const basic a, unsigned long b)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Integer>(*(basic_rcp(a))));
    basic_rcp(s)
        = SymEngine::binomial(down_cast<const Integer &>(*(basic_rcp(a))), b);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE ntheory_factorial(basic s, unsigned long n)
{
    CWRAPPER_BEGIN
    basic_rcp(s) = SymEngine::factorial(n);
    CWRAPPER_END
}

//! Wrapper for evalf
CWRAPPER_OUTPUT_TYPE basic_evalf(basic s, const basic b, unsigned long bits,
                                 int real)
{

    CWRAPPER_BEGIN
    basic_rcp(s)
        = SymEngine::evalf(*(basic_rcp(b)), bits, (SymEngine::EvalfDomain)real);
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_as_numer_denom(basic numer, basic denom,
                                          const basic x)
{
    CWRAPPER_BEGIN
    SymEngine::as_numer_denom(basic_rcp(x), SymEngine::outArg(basic_rcp(numer)),
                              SymEngine::outArg(basic_rcp(denom)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_add_as_two_terms(basic term1, basic term2,
                                            const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Add>(*(basic_rcp(s))));
    rcp_static_cast<const Add>(basic_rcp(s))
        ->as_two_terms(SymEngine::outArg(basic_rcp(term1)),
                       SymEngine::outArg(basic_rcp(term2)));
    CWRAPPER_END
}

CWRAPPER_OUTPUT_TYPE basic_mul_as_two_terms(basic term1, basic term2,
                                            const basic s)
{
    CWRAPPER_BEGIN
    SYMENGINE_ASSERT(is_a<Mul>(*(basic_rcp(s))));
    rcp_static_cast<const Mul>(basic_rcp(s))
        ->as_two_terms(SymEngine::outArg(basic_rcp(term1)),
                       SymEngine::outArg(basic_rcp(term2)));
    CWRAPPER_END
}

struct CLambdaRealDoubleVisitor {
    SymEngine::LambdaRealDoubleVisitor m;
};

CLambdaRealDoubleVisitor *lambda_real_double_visitor_new()
{
    return new CLambdaRealDoubleVisitor();
}

void lambda_real_double_visitor_init(CLambdaRealDoubleVisitor *self,
                                     const CVecBasic *args,
                                     const CVecBasic *exprs, int perform_cse)
{
    self->m.init(args->m, exprs->m, perform_cse);
}

void lambda_real_double_visitor_call(CLambdaRealDoubleVisitor *self,
                                     double *const outs,
                                     const double *const inps)
{
    self->m.call(outs, inps);
}

void lambda_real_double_visitor_free(CLambdaRealDoubleVisitor *self)
{
    delete self;
}

#ifdef HAVE_SYMENGINE_LLVM
// double
struct CLLVMDoubleVisitor {
    SymEngine::LLVMDoubleVisitor m;
};

CLLVMDoubleVisitor *llvm_double_visitor_new()
{
    return new CLLVMDoubleVisitor();
}

void llvm_double_visitor_init(CLLVMDoubleVisitor *self, const CVecBasic *args,
                              const CVecBasic *exprs, int perform_cse,
                              int opt_level)
{
    self->m.init(args->m, exprs->m, perform_cse, opt_level);
}

void llvm_double_visitor_call(CLLVMDoubleVisitor *self, double *const outs,
                              const double *const inps)
{
    self->m.call(outs, inps);
}

void llvm_double_visitor_free(CLLVMDoubleVisitor *self)
{
    delete self;
}
// float
struct CLLVMFloatVisitor {
    SymEngine::LLVMFloatVisitor m;
};

CLLVMFloatVisitor *llvm_float_visitor_new()
{
    return new CLLVMFloatVisitor();
}

void llvm_float_visitor_init(CLLVMFloatVisitor *self, const CVecBasic *args,
                             const CVecBasic *exprs, int perform_cse,
                             int opt_level)
{
    self->m.init(args->m, exprs->m, perform_cse, opt_level);
}

void llvm_float_visitor_call(CLLVMFloatVisitor *self, float *const outs,
                             const float *const inps)
{
    self->m.call(outs, inps);
}

void llvm_float_visitor_free(CLLVMFloatVisitor *self)
{
    delete self;
}
#ifdef SYMENGINE_HAVE_LLVM_LONG_DOUBLE
// long double
struct CLLVMLongDoubleVisitor {
    SymEngine::LLVMLongDoubleVisitor m;
};

CLLVMLongDoubleVisitor *llvm_long_double_visitor_new()
{
    return new CLLVMLongDoubleVisitor();
}

void llvm_long_double_visitor_init(CLLVMLongDoubleVisitor *self,
                                   const CVecBasic *args,
                                   const CVecBasic *exprs, int perform_cse,
                                   int opt_level)
{
    self->m.init(args->m, exprs->m, perform_cse, opt_level);
}

void llvm_long_double_visitor_call(CLLVMLongDoubleVisitor *self,
                                   long double *const outs,
                                   const long double *const inps)
{
    self->m.call(outs, inps);
}

void llvm_long_double_visitor_free(CLLVMLongDoubleVisitor *self)
{
    delete self;
}
#endif
#endif

CWRAPPER_OUTPUT_TYPE basic_cse(CVecBasic *replacement_syms,
                               CVecBasic *replacement_exprs,
                               CVecBasic *reduced_exprs, const CVecBasic *exprs)
{
    CWRAPPER_BEGIN
    vec_pair replacements;
    SymEngine::cse(replacements, reduced_exprs->m, exprs->m);
    for (auto &p : replacements) {
        replacement_syms->m.push_back(p.first);
        replacement_exprs->m.push_back(p.second);
    }
    CWRAPPER_END
}
//! Print stacktrace on segfault
void symengine_print_stack_on_segfault()
{
    SymEngine::print_stack_on_segfault();
}
}
