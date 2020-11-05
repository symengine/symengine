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
