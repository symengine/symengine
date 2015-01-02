#include <stdlib.h>

#include "basic.h"
#include "symbol.h"
#include "cwrapper.h"
#include "integer.h"
#include "rational.h"
#include "mul.h"
#include "pow.h"
#include "add.h"
#include "number.h"

using CSymPy::Basic;
using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::Symbol;
using CSymPy::Rational;
using CSymPy::Integer;
using CSymPy::Number;
using CSymPy::rcp_static_cast;
using CSymPy::is_a;

extern "C" {

void basic_free(basic s)
{
    s->ptr.reset();
}

basic basic_new()
{
    return new CWrapper();
}

void basic_symbol(basic s, char* c)
{
    basic_free(s);
    s->ptr = CSymPy::symbol(std::string(c));
}

void basic_integer_si(basic s, long i)
{
    basic_free(s);
    s->ptr = CSymPy::integer(i);
}

void basic_integer_ui(basic s, unsigned long i)
{
    basic_free(s);
    s->ptr = CSymPy::integer(i);
}

void basic_integer_mpz(basic s, mpz_t i)
{
    basic_free(s);
    s->ptr = CSymPy::integer(mpz_class(i));
}

void basic_integer_str(basic s, char* c)
{
    basic_free(s);
    mpz_class i(c, 10);
    s->ptr = CSymPy::integer(i);
}

void basic_rational_si(basic s, long a, long b)
{
    basic_free(s);
    s->ptr = CSymPy::Rational::from_two_ints(CSymPy::integer(a), CSymPy::integer(b));
}

void basic_rational_ui(basic s, unsigned long a, unsigned long b)
{
    basic_free(s);
    s->ptr = CSymPy::Rational::from_two_ints(CSymPy::integer(a), CSymPy::integer(b));
}

int basic_rational(basic s, basic a, basic b)
{
    basic_free(s);
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    s->ptr = CSymPy::Rational::from_two_ints(rcp_static_cast<const Integer>(a->ptr),
                rcp_static_cast<const Integer>(b->ptr));
    return 1;
}

void basic_rational_mpq(basic s, mpq_t i)
{
    basic_free(s);
    s->ptr = CSymPy::Rational::from_mpq(mpq_class(i));
}

int basic_diff(basic s, basic expr, basic symbol)
{
    if (!is_a_Symbol(symbol))
        return 0;
    RCP<const Basic> r = expr->ptr->diff(rcp_static_cast<const Symbol>(symbol->ptr));
    basic_free(s);
    s->ptr = r;
    return 1;
}

void basic_assign(basic a, basic b) {
    basic_free(a);
    a->ptr = RCP<const Basic>(b->ptr);
}

void basic_add(basic s, basic expr, basic symbol)
{
    RCP<const Basic> r = CSymPy::add(expr->ptr, symbol->ptr);
    basic_free(s);
    s->ptr = r;
}

void basic_mul(basic s, basic expr, basic symbol)
{
    RCP<const Basic> r = CSymPy::mul(expr->ptr, symbol->ptr);
    basic_free(s);
    s->ptr = r;
}

void basic_pow(basic s, basic expr, basic symbol)
{
    RCP<const Basic> r = CSymPy::pow(expr->ptr, symbol->ptr);
    basic_free(s);
    s->ptr = r;
}

void basic_div(basic s, basic expr, basic symbol)
{
    RCP<const Basic> r = CSymPy::div(expr->ptr, symbol->ptr);
    basic_free(s);
    s->ptr = r;
}

int basic_is_null(basic s)
{
    return s->ptr.is_null() ? 1 : 0 ;
}

char* basic_str(basic s) {
    std::string str = "";
    if (!basic_is_null(s)) {
        str = s->ptr->__str__();
    }
    char* cc = (char*) malloc(sizeof(char) * (str.size() + 1));
    std::copy(str.begin(), str.end(), cc);
    cc[str.size()] = '\0';
    return cc;
}

int is_a_Integer(basic c)
{
    if (basic_is_null(c)) return 0;
    return (c->ptr->get_type_code() == CSymPy::INTEGER) ? 1 : 0;
}
int is_a_Rational(basic c)
{
    if (basic_is_null(c)) return 0;
    return (c->ptr->get_type_code() == CSymPy::RATIONAL) ? 1 : 0;
}
int is_a_Symbol(basic c)
{
    if (basic_is_null(c)) return 0;
    return (c->ptr->get_type_code() == CSymPy::SYMBOL) ? 1 : 0;
}

}
