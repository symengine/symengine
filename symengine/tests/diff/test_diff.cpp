#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/add.h>
#include <symengine/diff.h>
#include <symengine/rational.h>
#include <symengine/polynomial.h>
#include <symengine/complex.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>

using SymEngine::RCP;
using SymEngine::Basic;
using SymEngine::div;
using SymEngine::pow;
using SymEngine::univariate_polynomial;
using SymEngine::mul;
using SymEngine::integer;
using SymEngine::print_stack_on_segfault;
using SymEngine::symbol;
using SymEngine::Complex;
using SymEngine::Rational;
using SymEngine::Number;
using SymEngine::add;
using SymEngine::Symbol;
using SymEngine::Integer;
using SymEngine::Subs;
using SymEngine::Derivative;
using SymEngine::function_symbol;
using SymEngine::I;
using SymEngine::real_double;
using SymEngine::complex_double;
using SymEngine::Sin;
using SymEngine::diff;
using SymEngine::one;
using SymEngine::zero;

TEST_CASE("test_diff(): diff", "[diff]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> im1 = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> im3 = integer(-3);
    RCP<const Basic> i4 = integer(4);
    RCP<const Basic> i5 = integer(5);
    RCP<const Basic> i6 = integer(6);
    RCP<const Basic> i9 = integer(9);
    RCP<const Basic> i10 = integer(10);
    RCP<const Basic> i27 = integer(27);

    RCP<const Basic> r1;
    RCP<const Basic> r2;

    r1 = integer(5);
    r2 = r1->diff(x);
    REQUIRE(eq(*r2, *zero));

    r1 = Rational::from_two_ints(*integer(2), *integer(3));
    r2 = r1->diff(x);
    REQUIRE(eq(*r2, *zero));

    r1 = sqrt(x);
    r1 = diff(diff(r1, x), x);
    r2 = mul(div(im1, i4), pow(x, div(im3, i2)));
    REQUIRE(eq(*r1, *r2));

    r1 = exp(pow(x, i3));
    r1 = diff(r1, x);
    r2 = mul(mul(i3, exp(pow(x, i3))), pow(x, i2));
    REQUIRE(eq(*r1, *r2));

    r1 = pow(x, x);
    r1 = diff(r1, x);
    r2 = mul(pow(x, x), add(log(x), one));
    REQUIRE(eq(*r1, *r2));

    r1 = pow(x, y);
    r1 = diff(r1, x);
    r2 = mul(pow(x, sub(y, one)), y);
    REQUIRE(eq(*r1, *r2));

    r1 = pow(y, x);
    r1 = diff(r1, x);
    r2 = mul(pow(y, x), log(y));
    REQUIRE(eq(*r1, *r2));

    r1 = pow(add(x, y), i2);
    r1 = diff(r1, x);
    r2 = mul(i2, add(x, y));
    REQUIRE(eq(*r1, *r2));

    r1 = add(add(i2, mul(i3, x)), mul(i5, pow(x, i2)));
    r1 = diff(r1, x);
    r2 = add(i3, mul(i10, x));
    REQUIRE(eq(*r1, *r2));
    r1 = diff(r1, x);
    r2 = i10; 
    REQUIRE(eq(*r1, *r2));

    r1 = add(mul(mul(pow(x, y), pow(y, x)), i2), one);
    r1 = diff(r1, x);
    r2 = add(mul(i2, mul(pow(x, y), mul(pow(y, x), log(y)))), mul(i2, mul(pow(x, y), mul(pow(y, x), div(y, x)))));
    REQUIRE(eq(*r1, *r2));
}