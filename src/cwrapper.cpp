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

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::rcp;
using SymEngine::zero;
using SymEngine::Symbol;
using SymEngine::Rational;
using SymEngine::Integer;
using SymEngine::Number;
using SymEngine::rcp_static_cast;
using SymEngine::is_a;

#define RCP_cast_general(x, CONST) (reinterpret_cast<CONST RCP<const Basic> *>(x))
#define RCP_cast(x) RCP_cast_general(x, )
#define RCP_const_cast(x) RCP_cast_general(x, const)

extern "C" {

void basic_init(basic s)
{
#if defined(WITH_SYMENGINE_RCP)
    // These checks only happen at compile time.
    // Check that 'basic' has the correct size:
    static_assert(sizeof(RCP<const Basic>) == sizeof(basic), "Size of 'basic' is not correct");
    // Check that 'basic' has the correct alignment:
    static_assert(std::alignment_of<RCP<const Basic>>::value == std::alignment_of<basic>::value, "Alignment of 'basic' is not correct");
#else
    throw std::runtime_error("Teuchos::RCP is not compatible with the C wrappers");
#endif
    // No allocation is being done, but the constructor of RCP is called and
    // the instance is initialized at the memory address 's'. The above checks
    // make sure that 's' has the correct size and alignment, which is
    // necessary for placement new, otherwise the results are undefined.
    new(s) RCP<const Basic>();
}

void basic_free(basic s)
{
    RCP_cast(s)->~RCP();
}

void symbol_set(basic s, char* c)
{
    *RCP_cast(s) = SymEngine::symbol(std::string(c));
}

void integer_set_si(basic s, long i)
{
    *RCP_cast(s) = SymEngine::integer(mpz_class(i));
}

void integer_set_ui(basic s, unsigned long i)
{
    *RCP_cast(s) = SymEngine::integer(mpz_class(i));
}

void integer_set_mpz(basic s, const mpz_t i)
{
    *RCP_cast(s) = SymEngine::integer(mpz_class(i));
}

void integer_set_str(basic s, char* c)
{
    *RCP_cast(s) = SymEngine::integer(mpz_class(c, 10));
}

signed long integer_get_si(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(*RCP_const_cast(s))));
    return mpz_get_si((rcp_static_cast<const Integer>(*RCP_const_cast(s)))->as_mpz().get_mpz_t());
}

unsigned long integer_get_ui(const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(*RCP_const_cast(s))));
    return mpz_get_ui((rcp_static_cast<const Integer>(*RCP_const_cast(s)))->as_mpz().get_mpz_t());
}

void integer_get_mpz(mpz_t a, const basic s)
{
    SYMENGINE_ASSERT(is_a<Integer>(*(*RCP_const_cast(s))));
    mpz_set(a, (rcp_static_cast<const Integer>(*RCP_const_cast(s)))->as_mpz().get_mpz_t());
}

void rational_set_si(basic s, long a, long b)
{
    *RCP_cast(s) = SymEngine::Rational::from_mpq(mpq_class(a, b));
}

void rational_set_ui(basic s, unsigned long a, unsigned long b)
{
    *RCP_cast(s) = SymEngine::Rational::from_mpq(mpq_class(a, b));
}

int rational_set(basic s, const basic a, const basic b)
{
    if (!is_a_Integer(a) || !is_a_Integer(b)) {
        return 0;
    }
    *RCP_cast(s) = SymEngine::Rational::from_two_ints(
            rcp_static_cast<const Integer>(*RCP_const_cast(a)),
            rcp_static_cast<const Integer>(*RCP_const_cast(b)));
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
    *RCP_cast(s) = SymEngine::add(*RCP_const_cast(a), *RCP_const_cast(b));
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
