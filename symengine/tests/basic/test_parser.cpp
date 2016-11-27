#include "catch.hpp"

#include <symengine/visitor.h>
#include <symengine/eval_double.h>
#include <symengine/parser.h>
#include <symengine/polys/basic_conversions.h>
#include <symengine/symengine_exception.h>

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
using SymEngine::pi;
using SymEngine::erf;
using SymEngine::erfc;
using SymEngine::function_symbol;
using SymEngine::real_double;
using SymEngine::RealDouble;
using SymEngine::E;
using SymEngine::parse;
using SymEngine::max;
using SymEngine::min;
using SymEngine::loggamma;
using SymEngine::gamma;
using SymEngine::UIntPoly;
using SymEngine::from_basic;
using SymEngine::ParseError;

using namespace SymEngine::literals;

TEST_CASE("Parsing: integers, basic operations", "[parser]")
{
    std::string s;
    RCP<const Basic> res;

    s = "-3-5";
    res = parse(s);
    REQUIRE(eq(*res, *integer(-8)));

    s = "((3)+(1*0))";
    res = parse(s);
    REQUIRE(eq(*res, *integer(3)));

    s = "((2))*(1+(2*3))";
    res = parse(s);
    REQUIRE(eq(*res, *integer(14)));

    s = "(1+1)*((1+1)+(1+1))";
    res = parse(s);
    REQUIRE(eq(*res, *integer(8)));

    s = "(1*3)*(2+4)*(2)";
    res = parse(s);
    REQUIRE(eq(*res, *integer(36)));

    s = "(1+3)/(2+4)";
    res = parse(s);
    REQUIRE(eq(*res, *div(integer(2), integer(3))));

    s = "2*3 + 50*2";
    res = parse(s);
    REQUIRE(eq(*res, *integer(106)));

    s = "2**(3+2)+ 10";
    res = parse(s);
    REQUIRE(eq(*res, *integer(42)));

    s = "(5^3)/8 + 12";
    res = parse(s);
    REQUIRE(eq(*res, *add(div(integer(125), integer(8)), integer(12))));

    s = "3*2+3-5+2/2";
    res = parse(s);
    REQUIRE(eq(*res, *integer(5)));

    s = "4^2/2+2";
    res = parse(s);
    REQUIRE(eq(*res, *integer(10)));

    s = "(1+2*(3+1)-5/(2+2))";
    res = parse(s);
    REQUIRE(eq(*res, *add(integer(9), div(integer(-5), integer(4)))));

    s = "2 + -3";
    res = parse(s);
    REQUIRE(eq(*res, *integer(-1)));

    s = "10000000000000000000000000";
    res = parse(s);
    REQUIRE(eq(*res, *pow(integer(10), integer(25))));

    // Make sure that parsing and printing works correctly
    s = "0.123123123e-10";
    res = parse(s);
    REQUIRE(eq(*res, *parse(res->__str__())));

    s = "123123123123123.";
    res = parse(s);
    REQUIRE(eq(*res, *parse(res->__str__())));

#ifdef HAVE_SYMENGINE_MPFR
    s = "1.231231232123123123123123123123e8";
    res = parse(s);
    REQUIRE(eq(*res, *parse(res->__str__())));
#endif
}

TEST_CASE("Parsing: symbols", "[parser]")
{
    std::string s;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> w = symbol("w1");
    RCP<const Basic> l = symbol("l0ngn4me");

    s = "x + 2*y";
    res = parse(s);
    REQUIRE(eq(*res, *add(x, mul(integer(2), y))));

    s = "w1*y";
    res = parse(s);
    REQUIRE(eq(*res, *mul(w, y)));

    s = "x**(3+w1)-2/y";
    res = parse(s);
    REQUIRE(eq(*res, *add(pow(x, add(integer(3), w)), div(integer(-2), y))));

    s = "l0ngn4me - w1*y + 2^(x)";
    res = parse(s);
    REQUIRE(eq(*res, *add(add(l, neg(mul(w, y))), pow(integer(2), x))));

    s = "4*x/8 - (w1*y)";
    res = parse(s);
    REQUIRE(eq(*res, *add(neg(mul(w, y)), div(x, integer(2)))));

    s = "3*y + (2*y)";
    res = parse(s);
    REQUIRE(eq(*res, *mul(y, integer(5))));

    s = "3*y/(1+x)";
    res = parse(s);
    REQUIRE(eq(*res, *div(mul(y, integer(3)), add(x, integer(1)))));

    s = "y/x*x";
    res = parse(s);
    REQUIRE(eq(*res, *y));

    s = "x * -y";
    res = parse(s);
    REQUIRE(eq(*res, *mul(x, mul(y, integer(-1)))));

    s = "x ^ --y";
    res = parse(s);
    REQUIRE(eq(*res, *pow(x, y)));

    s = "x**2e-1+3e+2-2e-2";
    res = parse(s);
    REQUIRE(eq(*res, *add(real_double(299.98), pow(x, real_double(0.2)))));
}

TEST_CASE("Parsing: functions", "[parser]")
{
    std::string s;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");

    s = "sin(x)";
    res = parse(s);
    REQUIRE(eq(*res, *sin(x)));

    s = "asin(-1)";
    res = parse(s);
    REQUIRE(eq(*res, *neg(div(pi, integer(2)))));

    s = "asin(sin(x))";
    res = parse(s);
    REQUIRE(eq(*res, *asin(sin(x))));

    s = "beta(x, y)";
    res = parse(s);
    REQUIRE(eq(*res, *beta(x, y)));

    s = "erf(erf(x*y)) + y";
    res = parse(s);
    REQUIRE(eq(*res, *add(erf(erf(mul(x, y))), y)));

    s = "erfc(sin(x))+erfc(x*y)";
    res = parse(s);
    REQUIRE(eq(*res, *add(erfc(mul(x, y)), erfc(sin(x)))));

    s = "beta(sin(x+3), gamma(2^y+sin(y)))";
    res = parse(s);
    REQUIRE(eq(*res, *beta(sin(add(x, integer(3))),
                           gamma(add(sin(y), pow(integer(2), y))))));

    s = "y^(abs(sin(3) + x)) + sinh(2)";
    res = parse(s);
    REQUIRE(
        eq(*res, *add(pow(y, abs(add(sin(integer(3)), x))), sinh(integer(2)))));

    s = "2 + zeta(2, x) + zeta(ln(3))";
    res = parse(s);
    REQUIRE(eq(*res, *add(integer(2),
                          add(zeta(integer(2), x), zeta(log(integer(3)))))));

    s = "sin(asin(x)) + y";
    res = parse(s);
    REQUIRE(eq(*res, *add(x, y)));

    s = "log(x, gamma(y))*sin(3)";
    res = parse(s);
    REQUIRE(eq(*res, *mul(log(x, gamma(y)), sin(integer(3)))));

    s = "loggamma(x)*gamma(y)";
    res = parse(s);
    REQUIRE(eq(*res, *mul(loggamma(x), gamma(y))));

    s = "loggamma(x)+loggamma(x)";
    res = parse(s);
    REQUIRE(eq(*res, *mul(integer(2), loggamma(x))));

    s = "max(x, x, y)";
    res = parse(s);
    REQUIRE(eq(*res, *max({x, y})));

    s = "max(x, y, max(x))";
    res = parse(s);
    REQUIRE(eq(*res, *max({x, y})));

    s = "sin(max(log(x, y), min(x, y)))";
    res = parse(s);
    REQUIRE(eq(*res, *sin(max({log(x, y), min({x, y})}))));
}

TEST_CASE("Parsing: constants", "[parser]")
{
    std::string s;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");

    s = "E*pi";
    res = parse(s);
    REQUIRE(eq(*res, *mul(E, pi)));

    s = "sin(pi/2)";
    res = parse(s);
    REQUIRE(eq(*res, *one));

    s = "log(e)";
    res = parse(s);
    REQUIRE(eq(*res, *one));

    s = "ln(e/e) + sin(pi*2/2) + 3*x";
    res = parse(s);
    REQUIRE(eq(*res, *mul(integer(3), x)));

    s = "(3+4*I)/(5+cos(pi/2)*I)";
    res = parse(s);
    REQUIRE(eq(*res, *div(Complex::from_two_nums(*integer(3), *integer(4)),
                          integer(5))));

    s = "(2*I +6*I)*3*I + 4*I";
    res = parse(s);
    REQUIRE(eq(*res, *Complex::from_two_nums(*integer(-24), *integer(4))));
}

TEST_CASE("Parsing: function_symbols", "[parser]")
{
    std::string s;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("wt");

    s = "f(x)";
    res = parse(s);
    REQUIRE(eq(*res, *function_symbol("f", x)));

    s = "my_func(x, wt) + sin(f(y))";
    res = parse(s);
    REQUIRE(eq(*res, *add(function_symbol("my_func", {x, z}),
                          sin(function_symbol("f", y)))));

    s = "func(x, y, wt) + f(sin(x))";
    res = parse(s);
    REQUIRE(eq(*res, *add(function_symbol("func", {x, y, z}),
                          function_symbol("f", sin(x)))));

    s = "f(g(2^x))";
    res = parse(s);
    REQUIRE(eq(
        *res, *function_symbol("f", function_symbol("g", pow(integer(2), x)))));
}

TEST_CASE("Parsing: doubles", "[parser]")
{
    std::string s;
    double d;
    RCP<const Basic> res;
    RCP<const Basic> x = symbol("x");

    s = "1.324";
    res = parse(s);
    REQUIRE(eq(*res, *real_double(1.324)));

    s = "0.0324*x + 2*3";
    res = parse(s);
    REQUIRE(eq(*res, *add(mul(real_double(0.0324), x), integer(6))));

    s = "1.324/(2+3)";
    res = parse(s);
    REQUIRE(is_a<RealDouble>(*res));
    d = static_cast<const RealDouble &>(*res).as_double();
    REQUIRE(std::abs(d - 0.2648) < 1e-12);

    s = "sqrt(2.0)+5";
    res = parse(s);
    REQUIRE(is_a<RealDouble>(*res));
    d = static_cast<const RealDouble &>(*res).as_double();
    REQUIRE(std::abs(d - (::sqrt(2) + 5)) < 1e-12);
}

TEST_CASE("Parsing: polys", "[parser]")
{
    std::string s;
    RCP<const UIntPoly> poly1, poly2, poly3, poly4;
    RCP<const Basic> x = symbol("x");

    s = "x + 2*x**2 + 1";
    poly1 = from_basic<UIntPoly>(parse(s));
    poly2 = UIntPoly::from_vec(x, {{1_z, 1_z, 2_z}});
    REQUIRE(eq(*poly1, *poly2));

    s = "2*(x+1)**10 + 3*(x+2)**5";
    poly1 = from_basic<UIntPoly>(parse(s));
    // double-braced initialization of 2-element vector
    // causes compiler error with boost.multiprecision
    // so use single brace.
    poly2 = pow_upoly(*UIntPoly::from_vec(x, {1_z, 1_z}), 10);
    poly3 = UIntPoly::from_vec(x, {{2_z}});
    poly2 = mul_upoly(*poly2, *poly3);
    poly3 = pow_upoly(*UIntPoly::from_vec(x, {2_z, 1_z}), 5);
    poly4 = UIntPoly::from_vec(x, {{3_z}});
    poly3 = mul_upoly(*poly4, *poly3);
    poly2 = add_upoly(*poly2, *poly3);
    REQUIRE(eq(*poly1, *poly2));

    s = "((x+1)**5)*(x+2)*(2*x + 1)**3";
    poly1 = from_basic<UIntPoly>(parse(s));

    poly2 = pow_upoly(*UIntPoly::from_vec(x, {1_z, 1_z}), 5);
    poly3 = UIntPoly::from_vec(x, {2_z, 1_z});
    poly2 = mul_upoly(*poly2, *poly3);
    poly3 = pow_upoly(*UIntPoly::from_vec(x, {1_z, 2_z}), 3);
    poly2 = mul_upoly(*poly2, *poly3);
    REQUIRE(eq(*poly1, *poly2));
}

TEST_CASE("Parsing: errors", "[parser]")
{
    std::string s;

    s = "x+y+";
    CHECK_THROWS_AS(parse(s), ParseError);

    s = "x + (y))";
    CHECK_THROWS_AS(parse(s), ParseError);

    s = "x + max((3, 2+1)";
    CHECK_THROWS_AS(parse(s), ParseError);

    s = "2..33 + 2";
    CHECK_THROWS_AS(parse(s), ParseError);

    s = "(2)(3)";
    CHECK_THROWS_AS(parse(s), ParseError);

    s = "sin(x y)";
    CHECK_THROWS_AS(parse(s), ParseError);

    s = "max(,3,2)";
    CHECK_THROWS_AS(parse(s), ParseError);
}
