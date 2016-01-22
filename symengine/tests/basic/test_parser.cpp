#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/visitor.h>
#include <symengine/eval_double.h>
#include <symengine/parser.cpp>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Complex;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::Number;
using SymEngine::pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::has_symbol;
using SymEngine::is_a;
using SymEngine::ExpressionParser;
using SymEngine::pi;

TEST_CASE("Parsing: integers, basic operations", "[parser]")
{
    std::string s;
    ExpressionParser p;
    RCP<const Basic> res;

    s = "-3-5";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(-8)));

    s = "((3)+(1*0))";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(3)));

    s = "((2))*(1+(2*3))";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(14)));

    s = "(1+1)*((1+1)+(1+1))";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(8)));

    s = "(1*3)*(2+4)*(2)";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(36)));

    s = "(1+3)/(2+4)";
    res = p.parse(s);
    REQUIRE(eq(*res, *div(integer(2), integer(3))));

    s = "2*3 + 50*2";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(106)));

    s = "2^(3+2)+ 10";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(42)));

    s = "(5^3)/8 + 12";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(div(integer(125), integer(8)), integer(12))));

    s = "3*2+3-5+2/2";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(5)));

    s = "4^2/2+2";
    res = p.parse(s);
    REQUIRE(eq(*res, *integer(10)));

    s = "(1+2*(3+1)-5/(2+2))";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(integer(9), div(integer(-5), integer(4)))));
}

TEST_CASE("Parsing: symbols", "[parser]")
{
    std::string s;
    ExpressionParser p;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> w = symbol("w1");
    RCP<const Basic> l = symbol("l0ngn4me");

    s = "x + 2*y";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(x, mul(integer(2), y))));

    s = "w1*y";
    res = p.parse(s);
    REQUIRE(eq(*res, *mul(w ,y)));

    s = "x^(3+w1)-2/y";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(pow(x, add(integer(3), w)), div(integer(-2), y))));

    s = "l0ngn4me - w1*y + 2^(x)";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(add(l, neg(mul(w, y))), pow(integer(2), x))));

    s = "4*x/8 - (w1*y)";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(neg(mul(w, y)), div(x, integer(2)))));

    s = "3*y + (2*y)";
    res = p.parse(s);
    REQUIRE(eq(*res, *mul(y, integer(5))));

    s = "3*y/(1+x)";
    res = p.parse(s);
    REQUIRE(eq(*res, *div(mul(y, integer(3)), add(x, integer(1)))));

    s = "y/x*x";
    res = p.parse(s);
    REQUIRE(eq(*res, *y));
}

TEST_CASE("Parsing: functions", "[parser]")
{
    std::string s;
    ExpressionParser p;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");

    s = "sin(x)";
    res = p.parse(s);
    REQUIRE(eq(*res, *sin(x)));

    s = "arcsin(-1)";
    res = p.parse(s);
    REQUIRE(eq(*res, *neg(div(pi, integer(2)))));

    s = "arcsin(sin(x))";
    res = p.parse(s);
    REQUIRE(eq(*res, *asin(sin(x))));

    s = "beta(x,y)";
    res = p.parse(s);
    REQUIRE(eq(*res, *beta(x,y)));

    s = "beta(sin(x+3), gamma(2^y+sin(y)))";
    res = p.parse(s);
    REQUIRE(eq(*res, *beta(sin(add(x, integer(3))), gamma(add(sin(y), pow(integer(2), y))))));

    s = "y^(abs(sin(3) + x)) + sinh(2)";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(pow(y, abs(add(sin(integer(3)), x))), sinh(integer(2)))));

    s = "2 + zeta(2, x) + zeta(ln(3))";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(integer(2), add(zeta(integer(2), x), zeta(log(integer(3)))))));

    s = "sin(arcsin(x)) + y";
    res = p.parse(s);
    REQUIRE(eq(*res, *add(x, y)));

    s = "log(x, gamma(y))*sin(3)";
    res = p.parse(s);
    REQUIRE(eq(*res, *mul(log(x, gamma(y)), sin(integer(3)))));
}

TEST_CASE("Parsing: constants", "[parser]")
{
    std::string s;
    ExpressionParser p;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");

    s = "sin(pi/2)";
    res = p.parse(s);
    REQUIRE(eq(*res, *one));

    s = "log(e)";
    res = p.parse(s);
    REQUIRE(eq(*res, *one));

    s = "ln(e/e) + sin(pi*2/2) + 3*x";
    res = p.parse(s);
    REQUIRE(eq(*res, *mul(integer(3), x)));
}