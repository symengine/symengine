#include <stdlib.h>

#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/cwrapper.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/functions.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/add.h>
#include <symengine/number.h>
#include <symengine/complex.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/printer.h>

#define xstr(s) str(s)
#define str(s) #s

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::zero;
using SymEngine::Symbol;
using SymEngine::Rational;
using SymEngine::Integer;
using SymEngine::integer_class;
using SymEngine::rational_class;
using SymEngine::Number;
using SymEngine::Complex;
using SymEngine::rcp_static_cast;
using SymEngine::is_a;
using SymEngine::RCPBasicKeyLess;
using SymEngine::set_basic;
using SymEngine::get_mpz_t;
using SymEngine::mp_get_ui;
using SymEngine::mp_get_si;

namespace SymEngine
{

template <typename T>
inline bool is_aligned(T *p, size_t n = alignof(T))
{
    return 0 == reinterpret_cast<uintptr_t>(p) % n;
}
}

extern "C" {

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

void symbol_set(basic s, const char *c)
{
    s->m = SymEngine::symbol(std::string(c));
}

void integer_set_si(basic s, long i)
{
    s->m = SymEngine::integer(integer_class(i));
}

void integer_set_ui(basic s, unsigned long i)
{
    s->m = SymEngine::integer(integer_class(i));
}

void integer_set_mpz(basic s, const mpz_t i)
{
    s->m = SymEngine::integer(integer_class(i));
}

void integer_set_str(basic s, char *c)
{
    s->m = SymEngine::integer(integer_class(c));
}

signed long integer_get_si(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    return mp_get_si((rcp_static_cast<const Integer>(s->m))->as_mpz());
}

unsigned long integer_get_ui(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    return mp_get_ui((rcp_static_cast<const Integer>(s->m))->as_mpz());
}

void integer_get_mpz(mpz_t a, const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    mpz_set(a, get_mpz_t((rcp_static_cast<const Integer>(s->m))->as_mpz()));
}

void rational_set_si(basic s, long a, long b)
{
    s->m = SymEngine::Rational::from_mpq(rational_class(a, b));
}

void rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    s->m = SymEngine::Rational::from_mpq(rational_class(a, b));
}

int rational_set(basic s, const basic a, const basic b)
{
    if (not is_a_Integer(a) or not is_a_Integer(b)) {
        return 0;
    }
    s->m = SymEngine::Rational::from_two_ints(
        *(rcp_static_cast<const Integer>(a->m)),
        *(rcp_static_cast<const Integer>(b->m)));
    return 1;
}

void rational_set_mpq(basic s, const mpq_t i)
{
    s->m = SymEngine::Rational::from_mpq(rational_class(i));
}

void complex_set(basic s, const basic re, const basic im)
{
    s->m = SymEngine::Complex::from_two_nums(
        *(rcp_static_cast<const Number>(re->m)),
        *(rcp_static_cast<const Number>(im->m)));
}

void complex_set_rat(basic s, const basic re, const basic im)
{
    s->m = SymEngine::Complex::from_two_rats(
        *(rcp_static_cast<const Rational>(re->m)),
        *(rcp_static_cast<const Rational>(im->m)));
}

void complex_set_mpq(basic s, const mpq_t re, const mpq_t im)
{
    s->m = SymEngine::Complex::from_mpq(rational_class(re), rational_class(im));
}

int basic_diff(basic s, const basic expr, basic const symbol)
{
    if (not is_a_Symbol(symbol))
        return 0;
    s->m = expr->m->diff(rcp_static_cast<const Symbol>(symbol->m));
    return 1;
}

void basic_assign(basic a, const basic b)
{
    a->m = b->m;
}

void basic_add(basic s, const basic a, const basic b)
{
    s->m = SymEngine::add(a->m, b->m);
}

void basic_sub(basic s, const basic a, const basic b)
{
    s->m = SymEngine::sub(a->m, b->m);
}

void basic_mul(basic s, const basic a, const basic b)
{
    s->m = SymEngine::mul(a->m, b->m);
}

void basic_pow(basic s, const basic a, const basic b)
{
    s->m = SymEngine::pow(a->m, b->m);
}

void basic_div(basic s, const basic a, const basic b)
{
    s->m = SymEngine::div(a->m, b->m);
}

int basic_eq(const basic a, const basic b)
{
    return SymEngine::eq(*(a->m), *(b->m)) ? 1 : 0;
}

int basic_neq(const basic a, const basic b)
{
    return SymEngine::neq(*(a->m), *(b->m)) ? 1 : 0;
}

#define IMPLEMENT_ONE_ARG_FUNC(func)                                           \
    void basic_##func(basic s, const basic a)                                  \
    {                                                                          \
        s->m = SymEngine::func(a->m);                                          \
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

void basic_get_args(const basic self, CVecBasic *args)
{
    args->m = self->m->get_args();
}

void basic_free_symbols(const basic self, CSetBasic *symbols)
{
    symbols->m = SymEngine::free_symbols(*(self->m));
}

size_t basic_hash(const basic self)
{
    return self->m->hash();
}

void basic_subs(basic s, const basic e, const CMapBasicBasic *mapbb)
{
    s->m = e->m->subs(mapbb->m);
}

void basic_subs2(basic s, const basic e, const basic a, const basic b)
{
    s->m = e->m->subs({{a->m, b->m}});
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

void ntheory_gcd(basic s, const basic a, const basic b)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(b->m)));
    s->m = SymEngine::gcd(static_cast<const Integer &>(*(a->m)),
                          static_cast<const Integer &>(*(b->m)));
}

void ntheory_lcm(basic s, const basic a, const basic b)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(b->m)));
    s->m = SymEngine::lcm(static_cast<const Integer &>(*(a->m)),
                          static_cast<const Integer &>(*(b->m)));
}

void ntheory_nextprime(basic s, const basic a)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    s->m = SymEngine::nextprime(static_cast<const Integer &>(*(a->m)));
}

void ntheory_mod(basic s, const basic n, const basic d)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(n->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(d->m)));
    s->m = SymEngine::mod(static_cast<const Integer &>(*(n->m)),
                          static_cast<const Integer &>(*(d->m)));
}

void ntheory_quotient(basic s, const basic n, const basic d)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(n->m)));
    SYMENGINE_ASSERT(is_a<Integer>(*(d->m)));
    s->m = SymEngine::quotient(static_cast<const Integer &>(*(n->m)),
                               static_cast<const Integer &>(*(d->m)));
}

void ntheory_fibonacci(basic s, unsigned long a)
{
    s->m = SymEngine::fibonacci(a);
}

void ntheory_lucas(basic s, unsigned long a)
{
    s->m = SymEngine::lucas(a);
}

void ntheory_binomial(basic s, const basic a, unsigned long b)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(a->m)));
    s->m = SymEngine::binomial(static_cast<const Integer &>(*(a->m)), b);
}

//! Print stacktrace on segfault
void symengine_print_stack_on_segfault()
{
    SymEngine::print_stack_on_segfault();
}
}
