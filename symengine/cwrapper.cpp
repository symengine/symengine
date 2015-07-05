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

void basic_init(basic s)
{
//    if (!SymEngine::is_aligned(s)) return 2;
//    if (size < sizeof(CRCPBasic)) return 1;

    new(s) CRCPBasic();
}

void basic_free(basic s)
{
    s->m.~RCP();
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
    *RCP_cast(s) = SymEngine::Rational::from_mpq(mpq_class(i));
}

int basic_diff(basic s, const basic expr, basic const symbol)
{
    if (!is_a_Symbol(symbol))
        return 0;
    *RCP_cast(s) = (*RCP_const_cast(expr))->diff(rcp_static_cast<const Symbol>
            (*RCP_const_cast(symbol)));
    return 1;
}

void basic_assign(basic a, const basic b) {
    *RCP_cast(a) = RCP<const Basic>(*RCP_const_cast(b));
}

void basic_add(basic s, const basic a, const basic b)
{
    s->m = SymEngine::add(a->m, b->m);
}

void basic_sub(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = SymEngine::sub(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_mul(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = SymEngine::mul(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_pow(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = SymEngine::pow(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_div(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = SymEngine::div(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_neg(basic s, const basic a)
{
    *RCP_cast(s) = SymEngine::neg(*RCP_const_cast(a));
}

void basic_abs(basic s, const basic a)
{
    *RCP_cast(s) = SymEngine::abs(*RCP_const_cast(a));
}

void basic_expand(basic s, const basic a)
{
    *RCP_cast(s) = SymEngine::expand(*RCP_const_cast(a));
}

char* basic_str(const basic s)
{
    std::string str = (*RCP_const_cast(s))->__str__();
    char *cc = new char[str.length()+1];
    std::strcpy(cc, str.c_str());
    return cc;
}

char* basic_str2(const basic2 s)
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
    return is_a<Integer>(*(*RCP_const_cast(c)));
}
int is_a_Rational(const basic c)
{
    return is_a<Rational>(*(*RCP_const_cast(c)));
}
int is_a_Symbol(const basic c)
{
    return is_a<Symbol>(*(*RCP_const_cast(c)));
}


// C wrapper for std::vector<int>

struct CVectorInt {
    std::vector<int> m;
};

CVectorInt* vectorint_new()
{
    return new CVectorInt;
}

int vectorint_placement_new(CVectorInt *self, size_t size)
{
    if (!SymEngine::is_aligned(self)) return 2;
    if (size < sizeof(CVectorInt)) return 1;
    new(self) CVectorInt;
    return 0;
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
    self->m.push_back(*RCP_const_cast(value));
}

void vecbasic_get(CVecBasic *self, int n, basic result)
{
    *RCP_cast(result) = self->m[n];
}

size_t vecbasic_size(CVecBasic *self)
{
    return self->m.size();
}

// ----------------------

void basic_get_args(const basic self, CVecBasic *args)
{
    args->m = (*RCP_const_cast(self))->get_args();
}

}
