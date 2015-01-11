#include <stdlib.h>

#include "basic.h"
#include "symbol.h"
#include "cwrapper.h"
#include "integer.h"
#include "rational.h"
#include "functions.h"
#include "mul.h"
#include "pow.h"
#include "add.h"
#include "number.h"
#include "constants.h"

using CSymPy::Basic;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::zero;
using CSymPy::Symbol;
using CSymPy::Rational;
using CSymPy::Integer;
using CSymPy::Number;
using CSymPy::rcp_static_cast;
using CSymPy::is_a;

struct CWrapper {
    public:
        RCP<const Basic> ptr;
        CWrapper() {
            ptr = zero;
        }
};

extern "C" {

void basic_free(basic s)
{
    s->ptr.reset();
    delete s;
}

basic basic_new()
{
    return new CWrapper();
}

void symbol_set(basic s, char* c)
{
    s->ptr = CSymPy::symbol(std::string(c));
}

void integer_set_si(basic s, long i)
{
    s->ptr = CSymPy::integer(mpz_class(i));
}

void integer_set_ui(basic s, unsigned long i)
{
    s->ptr = CSymPy::integer(mpz_class(i));
}

void integer_set_mpz(basic s, const mpz_t i)
{
    s->ptr = CSymPy::integer(mpz_class(i));
}

void integer_set_str(basic s, char* c)
{
    s->ptr = CSymPy::integer(mpz_class(c, 10));
}

void rational_set_si(basic s, long a, long b)
{
    s->ptr = CSymPy::Rational::from_mpq(mpq_class(a, b));
}

void rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    s->ptr = CSymPy::Rational::from_mpq(mpq_class(a, b));
}

int rational_set(basic s, const basic a, const basic b)
{
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    s->ptr = CSymPy::Rational::from_two_ints(rcp_static_cast<const Integer>(a->ptr),
                rcp_static_cast<const Integer>(b->ptr));
    return 1;
}

void rational_set_mpq(basic s, const mpq_t i)
{
    s->ptr = CSymPy::Rational::from_mpq(mpq_class(i));
}

int basic_diff(basic s, const basic expr, basic const symbol)
{
    if (!is_a_Symbol(symbol))
        return 0;
    s->ptr = expr->ptr->diff(rcp_static_cast<const Symbol>(symbol->ptr));
    return 1;
}

void basic_assign(basic a, const basic b) {
    a->ptr = RCP<const Basic>(b->ptr);
}

void basic_add(basic s, const basic a, const basic b)
{
    s->ptr = CSymPy::add(a->ptr, b->ptr);
}

void basic_sub(basic s, const basic a, const basic b)
{
    s->ptr = CSymPy::sub(a->ptr, b->ptr);
}

void basic_mul(basic s, const basic a, const basic b)
{
    s->ptr = CSymPy::mul(a->ptr, b->ptr);
}

void basic_pow(basic s, const basic a, const basic b)
{
    s->ptr = CSymPy::pow(a->ptr, b->ptr);
}

void basic_div(basic s, const basic a, const basic b)
{
    s->ptr = CSymPy::div(a->ptr, b->ptr);
}

void basic_neg(basic s, const basic a)
{
    s->ptr = CSymPy::neg(a->ptr);
}

void basic_abs(basic s, const basic a)
{
    s->ptr = CSymPy::abs(a->ptr);
}

void basic_expand(basic s, const basic a)
{
    s->ptr = CSymPy::expand(a->ptr);
}

char* basic_str(const basic s)
{
    std::string str = s->ptr->__str__();
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
    return is_a<Integer>(*(c->ptr));
}
int is_a_Rational(const basic c)
{
    return is_a<Rational>(*(c->ptr));
}
int is_a_Symbol(const basic c)
{
    return is_a<Symbol>(*(c->ptr));
}

}
