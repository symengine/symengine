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

#define RCP_cast(x) (static_cast<RCP<const Basic> *>(static_cast<void *>(x)));
#define RCP_cast_const(x) (static_cast<const RCP<const Basic> *>(static_cast<const void *>(x)));

extern "C" {

void basic_new(basic s)
{
    if (sizeof(RCP<const Basic>) > SIZE_OF_RCP_BASIC) {
        // The preallocated array is smaller than what is needed for
        // RCP<const Basic>
        throw std::runtime_error("Incorrect value in SIZE_OF_RCP_BASIC");
    }
    // No allocation is being done, but the constructor of RCP is called and
    // the instance is initialized at the memory address 's'.
    new(s) RCP<const Basic>();
}

void basic_free(basic s)
{
    RCP<const Basic> *p = RCP_cast(s);
    p->~RCP();
}

void symbol_set(basic s, char* c)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::symbol(std::string(c));
}

void integer_set_si(basic s, long i)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::integer(mpz_class(i));
}

void integer_set_ui(basic s, unsigned long i)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::integer(mpz_class(i));
}

void integer_set_mpz(basic s, const mpz_t i)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::integer(mpz_class(i));
}

void integer_set_str(basic s, char* c)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::integer(mpz_class(c, 10));
}

void rational_set_si(basic s, long a, long b)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::Rational::from_mpq(mpq_class(a, b));
}

void rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::Rational::from_mpq(mpq_class(a, b));
}

int rational_set(basic s, const basic a, const basic b)
{
    RCP<const Basic> *p = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    *p = CSymPy::Rational::from_two_ints(rcp_static_cast<const Integer>(*a2),
                rcp_static_cast<const Integer>(*b2));
    return 1;
}

void rational_set_mpq(basic s, const mpq_t i)
{
    RCP<const Basic> *p = RCP_cast(s);
    *p = CSymPy::Rational::from_mpq(mpq_class(i));
}

int basic_diff(basic s, const basic expr, basic const symbol)
{
    if (!is_a_Symbol(symbol))
        return 0;
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *expr2 = RCP_cast_const(expr);
    const RCP<const Basic> *symbol2 = RCP_cast_const(symbol);
    *s2 = (*expr2)->diff(rcp_static_cast<const Symbol>(*symbol2));
    return 1;
}

void basic_assign(basic a, const basic b) {
    RCP<const Basic> *a2 = RCP_cast(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    *a2 = RCP<const Basic>(*b2);
}

void basic_add(basic s, const basic a, const basic b)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    *s2 = CSymPy::add(*a2, *b2);
}

void basic_sub(basic s, const basic a, const basic b)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    *s2 = CSymPy::sub(*a2, *b2);
}

void basic_mul(basic s, const basic a, const basic b)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    *s2 = CSymPy::mul(*a2, *b2);
}

void basic_pow(basic s, const basic a, const basic b)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    *s2 = CSymPy::pow(*a2, *b2);
}

void basic_div(basic s, const basic a, const basic b)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    const RCP<const Basic> *b2 = RCP_cast_const(b);
    *s2 = CSymPy::div(*a2, *b2);
}

void basic_neg(basic s, const basic a)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    *s2 = CSymPy::neg(*a2);
}

void basic_abs(basic s, const basic a)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    *s2 = CSymPy::abs(*a2);
}

void basic_expand(basic s, const basic a)
{
    RCP<const Basic> *s2 = RCP_cast(s);
    const RCP<const Basic> *a2 = RCP_cast_const(a);
    *s2 = CSymPy::expand(*a2);
}

char* basic_str(const basic s)
{
    const RCP<const Basic> *s2 = RCP_cast_const(s);
    std::string str = (*s2)->__str__();
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
    const RCP<const Basic> *c2 = RCP_cast_const(c);
    return is_a<Integer>(*(*c2));
}
int is_a_Rational(const basic c)
{
    const RCP<const Basic> *c2 = RCP_cast_const(c);
    return is_a<Rational>(*(*c2));
}
int is_a_Symbol(const basic c)
{
    const RCP<const Basic> *c2 = RCP_cast_const(c);
    return is_a<Symbol>(*(*c2));
}

}
