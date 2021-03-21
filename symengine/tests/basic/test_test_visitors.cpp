#include "catch.hpp"
#include <symengine/test_visitors.h>
#include <symengine/sets.h>

using SymEngine::symbol;
using SymEngine::tribool;
using SymEngine::integer;
using SymEngine::Number;
using SymEngine::Basic;
using SymEngine::Symbol;
using SymEngine::Rational;
using SymEngine::RCP;
using SymEngine::interval;
using SymEngine::Set;
using SymEngine::Complex;
using SymEngine::pi;
using SymEngine::boolTrue;
using SymEngine::Inf;
using SymEngine::Nan;

TEST_CASE("Test is zero", "[is_zero]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Number> i1 = integer(0);
    RCP<const Number> i2 = integer(3);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> rat2 = Rational::from_two_ints(*integer(0), *integer(1));
    RCP<const Basic> s1 = interval(i1, i2);
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i2);
    RCP<const Basic> rel1 = Eq(x, i1);
    RCP<const Basic> rel2 = Lt(x, i1);
    RCP<const Symbol> t = symbol("t");
    RCP<const Basic> f = function_symbol("f", t);
    RCP<const Basic> d1 = f->diff(t);

    REQUIRE(is_zero(*x) == tribool::indeterminate);
    REQUIRE(is_true(is_zero(*i1)));
    REQUIRE(is_zero(*i2) == tribool::trifalse);
    REQUIRE(is_zero(*rat1) == tribool::trifalse);
    REQUIRE(is_zero(*rat2) == tribool::tritrue);
    REQUIRE(is_zero(*s1) == tribool::trifalse);
    REQUIRE(is_zero(*c1) == tribool::trifalse);
    REQUIRE(is_zero(*rel1) == tribool::trifalse);
    REQUIRE(is_zero(*rel2) == tribool::trifalse);
    REQUIRE(is_zero(*pi) == tribool::trifalse);
    REQUIRE(is_zero(*d1) == tribool::indeterminate);
    REQUIRE(is_zero(*boolTrue) == tribool::trifalse);
    REQUIRE(is_zero(*pi) == tribool::trifalse);
}

TEST_CASE("Test is positive", "[is_positive]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Number> i1 = integer(0);
    RCP<const Number> i2 = integer(3);
    RCP<const Number> i3 = integer(-2);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> rat2 = Rational::from_two_ints(*integer(0), *integer(1));
    RCP<const Basic> rat3 = Rational::from_two_ints(*integer(-1), *integer(3));
    RCP<const Basic> s1 = interval(i1, i2);
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i2);
    RCP<const Number> c2 = Complex::from_two_nums(*i3, *i3);
    RCP<const Basic> rel1 = Eq(x, i1);
    RCP<const Basic> rel2 = Lt(x, i1);
    RCP<const Symbol> t = symbol("t");
    RCP<const Basic> f = function_symbol("f", t);
    RCP<const Basic> d1 = f->diff(t);

    REQUIRE(is_indeterminate(is_positive(*x)));
    REQUIRE(is_false(is_positive(*i1)));
    REQUIRE(is_true(is_positive(*i2)));
    REQUIRE(is_false(is_positive(*i3)));
    REQUIRE(is_true(is_positive(*rat1)));
    REQUIRE(is_false(is_positive(*rat2)));
    REQUIRE(is_false(is_positive(*rat3)));
    REQUIRE(is_false(is_positive(*s1)));
    REQUIRE(is_false(is_positive(*c1)));
    REQUIRE(is_false(is_positive(*c2)));
    REQUIRE(is_false(is_positive(*rel1)));
    REQUIRE(is_false(is_positive(*rel2)));
    REQUIRE(is_true(is_positive(*pi)));
    REQUIRE(is_indeterminate(is_positive(*d1)));
    REQUIRE(is_false(is_positive(*boolTrue)));
}

TEST_CASE("Test is non positive", "[is_nonpositive]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Number> i1 = integer(0);
    RCP<const Number> i2 = integer(3);
    RCP<const Number> i3 = integer(-2);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> rat2 = Rational::from_two_ints(*integer(0), *integer(1));
    RCP<const Basic> rat3 = Rational::from_two_ints(*integer(-1), *integer(3));
    RCP<const Basic> s1 = interval(i1, i2);
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i2);
    RCP<const Number> c2 = Complex::from_two_nums(*i3, *i3);
    RCP<const Basic> rel1 = Eq(x, i1);
    RCP<const Basic> rel2 = Lt(x, i1);
    RCP<const Symbol> t = symbol("t");
    RCP<const Basic> f = function_symbol("f", t);
    RCP<const Basic> d1 = f->diff(t);

    REQUIRE(is_indeterminate(is_nonpositive(*x)));
    REQUIRE(is_true(is_nonpositive(*i1)));
    REQUIRE(is_false(is_nonpositive(*i2)));
    REQUIRE(is_true(is_nonpositive(*i3)));
    REQUIRE(is_false(is_nonpositive(*rat1)));
    REQUIRE(is_true(is_nonpositive(*rat2)));
    REQUIRE(is_true(is_nonpositive(*rat3)));
    REQUIRE(is_false(is_nonpositive(*s1)));
    REQUIRE(is_false(is_nonpositive(*c1)));
    REQUIRE(is_false(is_nonpositive(*c2)));
    REQUIRE(is_false(is_nonpositive(*rel1)));
    REQUIRE(is_false(is_nonpositive(*rel2)));
    REQUIRE(is_false(is_nonpositive(*pi)));
    REQUIRE(is_indeterminate(is_nonpositive(*d1)));
    REQUIRE(is_false(is_nonpositive(*boolTrue)));
}

TEST_CASE("Test is negative", "[is_negative]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Number> i1 = integer(0);
    RCP<const Number> i2 = integer(3);
    RCP<const Number> i3 = integer(-2);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> rat2 = Rational::from_two_ints(*integer(0), *integer(1));
    RCP<const Basic> rat3 = Rational::from_two_ints(*integer(-1), *integer(3));
    RCP<const Basic> s1 = interval(i1, i2);
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i2);
    RCP<const Number> c2 = Complex::from_two_nums(*i3, *i3);
    RCP<const Basic> rel1 = Eq(x, i1);
    RCP<const Basic> rel2 = Lt(x, i1);
    RCP<const Symbol> t = symbol("t");
    RCP<const Basic> f = function_symbol("f", t);
    RCP<const Basic> d1 = f->diff(t);

    REQUIRE(is_indeterminate(is_negative(*x)));
    REQUIRE(is_false(is_negative(*i1)));
    REQUIRE(is_false(is_negative(*i2)));
    REQUIRE(is_true(is_negative(*i3)));
    REQUIRE(is_false(is_negative(*rat1)));
    REQUIRE(is_false(is_negative(*rat2)));
    REQUIRE(is_true(is_negative(*rat3)));
    REQUIRE(is_false(is_negative(*s1)));
    REQUIRE(is_false(is_negative(*c1)));
    REQUIRE(is_false(is_negative(*c2)));
    REQUIRE(is_false(is_negative(*rel1)));
    REQUIRE(is_false(is_negative(*rel2)));
    REQUIRE(is_false(is_negative(*pi)));
    REQUIRE(is_indeterminate(is_negative(*d1)));
    REQUIRE(is_false(is_negative(*boolTrue)));
}

TEST_CASE("Test is nonnegative", "[is_nonnegative]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Number> i1 = integer(0);
    RCP<const Number> i2 = integer(3);
    RCP<const Number> i3 = integer(-2);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> rat2 = Rational::from_two_ints(*integer(0), *integer(1));
    RCP<const Basic> rat3 = Rational::from_two_ints(*integer(-1), *integer(3));
    RCP<const Basic> s1 = interval(i1, i2);
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i2);
    RCP<const Number> c2 = Complex::from_two_nums(*i3, *i3);
    RCP<const Basic> rel1 = Eq(x, i1);
    RCP<const Basic> rel2 = Lt(x, i1);
    RCP<const Symbol> t = symbol("t");
    RCP<const Basic> f = function_symbol("f", t);
    RCP<const Basic> d1 = f->diff(t);

    REQUIRE(is_indeterminate(is_nonnegative(*x)));
    REQUIRE(is_true(is_nonnegative(*i1)));
    REQUIRE(is_true(is_nonnegative(*i2)));
    REQUIRE(is_false(is_nonnegative(*i3)));
    REQUIRE(is_true(is_nonnegative(*rat1)));
    REQUIRE(is_true(is_nonnegative(*rat2)));
    REQUIRE(is_false(is_nonnegative(*rat3)));
    REQUIRE(is_false(is_nonnegative(*s1)));
    REQUIRE(is_false(is_nonnegative(*c1)));
    REQUIRE(is_false(is_nonnegative(*c2)));
    REQUIRE(is_false(is_nonnegative(*rel1)));
    REQUIRE(is_false(is_nonnegative(*rel2)));
    REQUIRE(is_true(is_nonnegative(*pi)));
    REQUIRE(is_indeterminate(is_nonnegative(*d1)));
    REQUIRE(is_false(is_nonnegative(*boolTrue)));
}

TEST_CASE("Test is_real", "[is_real]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Number> i1 = integer(0);
    RCP<const Number> i2 = integer(3);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> s1 = interval(i1, i2);
    RCP<const Number> c1 = Complex::from_two_nums(*i1, *i2);
    RCP<const Basic> rel1 = Eq(x, i1);
    RCP<const Symbol> t = symbol("t");
    RCP<const Basic> f = function_symbol("f", t);
    RCP<const Basic> d1 = f->diff(t);
    RCP<const Basic> e1 = add(x, x);
    RCP<const Basic> e2 = add(x, Inf);
    RCP<const Basic> e3 = add(x, c1);

    REQUIRE(is_indeterminate(is_real(*x)));
    REQUIRE(is_true(is_real(*i1)));
    REQUIRE(is_true(is_real(*i2)));
    REQUIRE(is_true(is_real(*rat1)));
    REQUIRE(is_false(is_real(*s1)));
    REQUIRE(is_false(is_real(*c1)));
    REQUIRE(is_false(is_real(*rel1)));
    REQUIRE(is_true(is_real(*pi)));
    REQUIRE(is_indeterminate(is_real(*d1)));
    REQUIRE(is_false(is_real(*boolTrue)));
    REQUIRE(is_indeterminate(is_real(*e1)));
    REQUIRE(is_indeterminate(is_real(*e2)));
    REQUIRE(is_indeterminate(is_real(*e3)));
    REQUIRE(is_false(is_real(*Inf)));
    REQUIRE(is_false(is_real(*Nan)));
}

TEST_CASE("Test is_polynomial", "[is_polynomial]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Number> i1 = integer(1);
    RCP<const Number> i2 = integer(3);
    RCP<const Number> i3 = integer(-2);
    RCP<const Basic> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Basic> e1 = add(x, i1);
    RCP<const Basic> e2 = add(x, y);
    RCP<const Basic> e3 = add(add(x, pi), y);
    RCP<const Basic> e4 = mul(x, y);
    RCP<const Basic> e5 = mul(i2, mul(x, y));
    RCP<const Basic> e6 = mul(i2, mul(x, add(x, y)));
    RCP<const Basic> e7 = pow(x, y);
    RCP<const Basic> e8 = pow(x, i2);
    RCP<const Basic> e9 = pow(x, rat1);
    RCP<const Basic> e10 = pow(integer(2), x);
    RCP<const Basic> e11 = div(i1, x);
    RCP<const Basic> e12 = pow(x, i3);
    RCP<const Basic> e13 = pow(x, x);
    RCP<const Basic> e14 = pow(x, mul(i3, y));
    RCP<const Basic> e15 = pow(mul(i2, x), y);
    RCP<const Basic> e16 = add(add(mul(x, x), mul(i2, x)), i3);
    RCP<const Basic> e17 = sqrt(x);
    RCP<const Basic> e18 = pow(sqrt(x), i2);
    RCP<const Basic> e19 = sqrt(y);
    RCP<const Basic> e20 = add(add(mul(x, x), mul(i2, mul(x, e19))), i3);
    RCP<const Basic> e21
        = add(add(mul(mul(x, x), mul(y, y)), mul(x, pow(y, i2))), exp(i3));
    RCP<const Basic> e22
        = add(add(mul(mul(x, x), mul(y, y)), mul(x, pow(y, i2))), exp(x));
    RCP<const Basic> e23 = sin(x);
    RCP<const Basic> b1 = interval(i1, i2);
    RCP<const Basic> rel1 = Eq(x, i1);

    REQUIRE(is_polynomial(*x));
    REQUIRE(is_polynomial(*i1));
    REQUIRE(is_polynomial(*rat1));
    REQUIRE(is_polynomial(*pi));
    REQUIRE(is_polynomial(*e1));
    REQUIRE(is_polynomial(*e2));
    REQUIRE(is_polynomial(*e3));
    REQUIRE(is_polynomial(*e4));
    REQUIRE(is_polynomial(*e5));
    REQUIRE(is_polynomial(*e6));
    REQUIRE(!is_polynomial(*e7));
    REQUIRE(is_polynomial(*e8));
    REQUIRE(!is_polynomial(*e9));
    REQUIRE(!is_polynomial(*e10));
    REQUIRE(!is_polynomial(*e11));
    REQUIRE(!is_polynomial(*b1));
    REQUIRE(is_polynomial(*rat1, {x, y, z}));
    REQUIRE(is_polynomial(*pi, {x, y, z}));
    REQUIRE(is_polynomial(*x, {x}));
    REQUIRE(is_polynomial(*x, {y}));
    REQUIRE(is_polynomial(*e8, {x}));
    REQUIRE(!is_polynomial(*e12, {x}));
    REQUIRE(is_polynomial(*e12, {y}));
    REQUIRE(!is_polynomial(*e10, {x}));
    REQUIRE(is_polynomial(*e10, {y}));
    REQUIRE(!is_polynomial(*e7, {x}));
    REQUIRE(!is_polynomial(*e7, {y}));
    REQUIRE(!is_polynomial(*e13, {x}));
    REQUIRE(!is_polynomial(*e14, {x}));
    REQUIRE(!is_polynomial(*e15, {x}));
    REQUIRE(is_polynomial(*e16, {x}));
    REQUIRE(is_polynomial(*e16, {y}));
    REQUIRE(is_polynomial(*e16));
    REQUIRE(!is_polynomial(*e17, {x}));
    REQUIRE(is_polynomial(*e17, {y}));
    REQUIRE(!is_polynomial(*e17));
    REQUIRE(!is_polynomial(*e18, {x}));
    REQUIRE(is_polynomial(*e20, {x}));
    REQUIRE(!is_polynomial(*e20, {y}));
    REQUIRE(is_polynomial(*e21));
    REQUIRE(!is_polynomial(*e22));
    REQUIRE(is_polynomial(*e21, {x, y}));
    REQUIRE(!is_polynomial(*e22, {x, y}));
    REQUIRE(!is_polynomial(*e23));
    REQUIRE(is_polynomial(*e23, {y}));
    REQUIRE(!is_polynomial(*rel1));
}
