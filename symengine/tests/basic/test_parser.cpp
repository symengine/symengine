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
using SymEngine::expressionParser;

TEST_CASE("Parsing: integers, basic operations", "[parser]")
{
    std::string s;
    expressionParser p;
    RCP<const Basic> res;

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
    expressionParser p;
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
}
