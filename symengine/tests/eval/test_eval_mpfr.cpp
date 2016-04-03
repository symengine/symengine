#include "catch.hpp"
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/eval_mpfr.h>
#include <symengine/constants.h>
#include <symengine/functions.h>

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::pi;
using SymEngine::E;
using SymEngine::EulerGamma;
using SymEngine::mul;
using SymEngine::erf;
using SymEngine::sub;
using SymEngine::eval_mpfr;
using SymEngine::integer_class;
using SymEngine::print_stack_on_segfault;
using SymEngine::max;
using SymEngine::min;
using SymEngine::loggamma;

TEST_CASE("precision: eval_mpfr", "[eval_mpfr]")
{
    mpfr_t a;
    mpfr_init2(a, 53);
    RCP<const Basic> s = mul(pi, integer(1963319607));
    RCP<const Basic> t = integer(integer_class(6167950454));
    RCP<const Basic> r = sub(s, t);
    // value of `r` is approximately 0.000000000149734291

    eval_mpfr(a, *r, MPFR_RNDN);
    // `eval_mpfr` was done with a precision of 53 bits (precision of `a`) and
    // rounding mode `MPFR_RNDN`
    // With 53 bit precision, `s` and `t` have the same value.
    // Hence value of `r` was  rounded down to `0.000000000000000`
    REQUIRE(mpfr_cmp_si(a, 0) == 0);

    mpfr_set_prec(a, 100);
    eval_mpfr(a, *r, MPFR_RNDN);
    // `eval_mpfr` was done with a precision of 100 bits (precision of `a`) and
    // rounding mode `MPFR_RNDN`
    // With 100 bit precision, `s` and `t` are not equal in value.
    // Value of `r` is a positive quantity with value 0.000000000149734291.....
    REQUIRE(mpfr_cmp_si(a, 0) == 1);

    // Check that value of `r` (`a`) starts with 0.000000000149734291
    REQUIRE(mpfr_cmp_d(a, 0.000000000149734291) == 1);
    REQUIRE(mpfr_cmp_d(a, 0.000000000149734292) == -1);

    s = mul(EulerGamma, integer(100000000));
    t = integer(57721566);
    r = div(sub(s, t), integer(100000000));
    // value of `r` is approximately 0.0000000049015328606065120900824024...

    eval_mpfr(a, *r, MPFR_RNDN);
    // Check that value of `r` (`a`) starts with 0.00000000490153
    REQUIRE(mpfr_cmp_d(a, 0.00000000490153) == 1);
    REQUIRE(mpfr_cmp_d(a, 0.00000000490154) == -1);

    s = mul(E, integer(100000));
    t = integer(271828);
    r = div(sub(s, t), integer(100000000));

    eval_mpfr(a, *r, MPFR_RNDN);
    // Check that value of `r` (`a`) starts with 0.00000000182845
    REQUIRE(mpfr_cmp_d(a, 0.00000000182845) == 1);
    REQUIRE(mpfr_cmp_d(a, 0.00000000182846) == -1);

    r = max({integer(3), integer(2)});

    eval_mpfr(a, *r, MPFR_RNDN);
    REQUIRE(mpfr_cmp_d(a, 3.00000000000001) == -1);
    REQUIRE(mpfr_cmp_d(a, 2.99999999999999) == 1);

    r = min({sqrt(integer(3)), sqrt(integer(2))});

    eval_mpfr(a, *r, MPFR_RNDN);
    REQUIRE(mpfr_cmp_d(a, 1.41421356238) == -1);
    REQUIRE(mpfr_cmp_d(a, 1.41421356236) == 1);

    r = loggamma(E);
    eval_mpfr(a, *r, MPFR_RNDN);
    REQUIRE(mpfr_cmp_d(a, 0.44946174183) == -1);
    REQUIRE(mpfr_cmp_d(a, 0.44946174181) == 1);

    r = loggamma(integer(5));
    eval_mpfr(a, *r, MPFR_RNDN);
    REQUIRE(mpfr_cmp_d(a, 3.17805383035) == -1);
    REQUIRE(mpfr_cmp_d(a, 3.17805383033) == 1);

    r = erf(integer(2));

    eval_mpfr(a, *r, MPFR_RNDN);
    REQUIRE(mpfr_cmp_d(a, 0.995322265019) == -1);
    REQUIRE(mpfr_cmp_d(a, 0.995322265017) == 1);

    r = erf(div(E, pi));

    eval_mpfr(a, *r, MPFR_RNDN);
    REQUIRE(mpfr_cmp_d(a, 0.778918254988) == -1);
    REQUIRE(mpfr_cmp_d(a, 0.778918254986) == 1);

    mpfr_clear(a);
}
