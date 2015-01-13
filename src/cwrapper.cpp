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

#define RCP_cast(x) (static_cast<RCP<const Basic> *>(static_cast<void *>(x)))
#define RCP_const_cast(x) (static_cast<const RCP<const Basic> *>(static_cast<const void *>(x)))

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
    RCP_cast(s)->~RCP();
}

void symbol_set(basic s, char* c)
{
    *RCP_cast(s) = CSymPy::symbol(std::string(c));
}

void integer_set_si(basic s, long i)
{
    *RCP_cast(s) = CSymPy::integer(mpz_class(i));
}

void integer_set_ui(basic s, unsigned long i)
{
    *RCP_cast(s) = CSymPy::integer(mpz_class(i));
}

void integer_set_mpz(basic s, const mpz_t i)
{
    *RCP_cast(s) = CSymPy::integer(mpz_class(i));
}

void integer_set_str(basic s, char* c)
{
    *RCP_cast(s) = CSymPy::integer(mpz_class(c, 10));
}

void rational_set_si(basic s, long a, long b)
{
    *RCP_cast(s) = CSymPy::Rational::from_mpq(mpq_class(a, b));
}

void rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    *RCP_cast(s) = CSymPy::Rational::from_mpq(mpq_class(a, b));
}

int rational_set(basic s, const basic a, const basic b)
{
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    *RCP_cast(s) = CSymPy::Rational::from_two_ints(
            rcp_static_cast<const Integer>(*RCP_const_cast(a)),
            rcp_static_cast<const Integer>(*RCP_const_cast(b)));
    return 1;
}

void rational_set_mpq(basic s, const mpq_t i)
{
    *RCP_cast(s) = CSymPy::Rational::from_mpq(mpq_class(i));
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
    *RCP_cast(s) = CSymPy::add(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_sub(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = CSymPy::sub(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_mul(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = CSymPy::mul(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_pow(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = CSymPy::pow(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_div(basic s, const basic a, const basic b)
{
    *RCP_cast(s) = CSymPy::div(*RCP_const_cast(a), *RCP_const_cast(b));
}

void basic_neg(basic s, const basic a)
{
    *RCP_cast(s) = CSymPy::neg(*RCP_const_cast(a));
}

void basic_abs(basic s, const basic a)
{
    *RCP_cast(s) = CSymPy::abs(*RCP_const_cast(a));
}

void basic_expand(basic s, const basic a)
{
    *RCP_cast(s) = CSymPy::expand(*RCP_const_cast(a));
}

char* basic_str(const basic s)
{
    std::string str = (*RCP_const_cast(s))->__str__();
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

}
