#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/eval_mpfr.h>
#include <symengine/constants.h>

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::pi;
using SymEngine::mul;
using SymEngine::sub;
using SymEngine::eval_mpfr;
using SymEngine::print_stack_on_segfault;

void test_precision()
{
    mpfr_t a;
    mpfr_init2(a, 53);
    RCP<const Basic> s = mul(pi, integer(1963319607));
    RCP<const Basic> t = integer(mpz_class(6167950454));
    RCP<const Basic> r = sub(s, t);
    // value of `r` is approximately 0.000000000149734291

    eval_mpfr(a, *r, MPFR_RNDN);
    // `eval_mpfr` was done with a precision of 53 bits (precision of `a`) and rounding mode `MPFR_RNDN`
    // With 53 bit precision, `s` and `t` have the same value.
    // Hence value of `r` was  rounded down to `0.000000000000000`
    assert(mpfr_cmp_si(a, 0) == 0);

    mpfr_set_prec(a, 100);
    eval_mpfr(a, *r, MPFR_RNDN);
    // `eval_mpfr` was done with a precision of 100 bits (precision of `a`) and rounding mode `MPFR_RNDN`
    // With 100 bit precision, `s` and `t` are not equal in value.
    // Value of `r` is a positive quantity with value 0.000000000149734291.....
    assert(mpfr_cmp_si(a, 0) == 1);

    // Check that value of `r` (`a`) starts with 0.000000000149734291
    assert(mpfr_cmp_d(a, 0.000000000149734291) == 1);
    assert(mpfr_cmp_d(a, 0.000000000149734292) == -1);

    mpfr_clear(a);
}

int main(int argc, char* argv[])
{
    print_stack_on_segfault();

    test_precision();

    mpfr_free_cache();

    return 0;
}
