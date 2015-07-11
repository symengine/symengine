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
#include <symengine/constants.h>
#include <symengine/visitor.h>

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::zero;
using SymEngine::Symbol;
using SymEngine::Rational;
using SymEngine::Integer;
using SymEngine::Number;
using SymEngine::rcp_static_cast;
using SymEngine::is_a;

namespace SymEngine {

template< typename T >
inline bool is_aligned( T*p, size_t n = alignof(T) ){
    return 0 == reinterpret_cast<uintptr_t>(p) % n ;
}

}


extern "C" {

struct CRCPBasic {
    SymEngine::RCP<const SymEngine::Basic> m;
};

static_assert(sizeof(CRCPBasic) == sizeof(CRCPBasic_C), "Size of 'basic' is not correct");
static_assert(std::alignment_of<CRCPBasic>::value == std::alignment_of<CRCPBasic_C>::value, "Alignment of 'basic' is not correct");

void basic_init(basic s)
{
    new(s) CRCPBasic();
}

void basic_free(basic s)
{
    s->m.~RCP();
}

basic_struct* basic_new_heap()
{
    return new CRCPBasic();
}

void basic_free_heap(basic_struct *s)
{
    delete s;
}

TypeID basic_get_type(const basic s) {
    return s->m->get_type_code();
}

void symbol_set(basic s, char* c)
{
    s->m = SymEngine::symbol(std::string(c));
}

void integer_set_si(basic s, long i)
{
    s->m = SymEngine::integer(mpz_class(i));
}

void integer_set_ui(basic s, unsigned long i)
{
    s->m = SymEngine::integer(mpz_class(i));
}

void integer_set_mpz(basic s, const mpz_t i)
{
    s->m = SymEngine::integer(mpz_class(i));
}

void integer_set_str(basic s, char* c)
{
    s->m = SymEngine::integer(mpz_class(c, 10));
}

signed long integer_get_si(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    return mpz_get_si((rcp_static_cast<const Integer>(s->m))->as_mpz().get_mpz_t());
}

unsigned long integer_get_ui(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    return mpz_get_ui((rcp_static_cast<const Integer>(s->m))->as_mpz().get_mpz_t());
}

void integer_get_mpz(mpz_t a, const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(s->m)));
    mpz_set(a, (rcp_static_cast<const Integer>(s->m))->as_mpz().get_mpz_t());
}

void rational_set_si(basic s, long a, long b)
{
    s->m = SymEngine::Rational::from_mpq(mpq_class(a, b));
}

void rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    s->m = SymEngine::Rational::from_mpq(mpq_class(a, b));
}

int rational_set(basic s, const basic a, const basic b)
{
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    s->m = SymEngine::Rational::from_two_ints(
            rcp_static_cast<const Integer>(a->m),
            rcp_static_cast<const Integer>(b->m));
    return 1;
}

void rational_set_mpq(basic s, const mpq_t i)
{
    s->m = SymEngine::Rational::from_mpq(mpq_class(i));
}

int basic_diff(basic s, const basic expr, basic const symbol)
{
    if (!is_a_Symbol(symbol))
        return 0;
    s->m = expr->m->diff(rcp_static_cast<const Symbol>(symbol->m));
    return 1;
}

void basic_assign(basic a, const basic b) {
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

void basic_neg(basic s, const basic a)
{
    s->m = SymEngine::neg(a->m);
}

void basic_abs(basic s, const basic a)
{
    s->m = SymEngine::abs(a->m);
}

void basic_expand(basic s, const basic a)
{
    s->m = SymEngine::expand(a->m);
}

char* basic_str(const basic s)
{
    std::string str = s->m->__str__();
    char *cc = new char[str.length()+1];
    std::strcpy(cc, str.c_str());
    return cc;
}

void basic_str_free(char* s)
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


// C wrapper for std::vector<int>

struct CVectorInt {
    std::vector<int> m;
};

CVectorInt* vectorint_new()
{
    return new CVectorInt;
}

int vectorint_placement_new_check(void *data, size_t size)
{
    CVectorInt *self = (CVectorInt*)data;
    if (size < sizeof(CVectorInt)) return 1;
    if (!SymEngine::is_aligned(self)) return 2;
    return 0;
}

CVectorInt* vectorint_placement_new(void *data)
{
#if defined(WITH_SYMENGINE_ASSERT)
    // if (size < sizeof(CVectorInt)) return 1; // Requires the 'size' argument
    CVectorInt *self = (CVectorInt*)data;
    SYMENGINE_ASSERT(SymEngine::is_aligned(self));
#endif
    new(data) CVectorInt;
    return (CVectorInt*)data;
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

CVecBasic* vecbasic_new()
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

CSetBasic* setbasic_new()
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

int setbasic_find(CSetBasic *self, basic value)
{
    return self->m.find(value->m) != (self->m).end() ? 1 : 0;
}

size_t setbasic_size(CSetBasic *self)
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

}
