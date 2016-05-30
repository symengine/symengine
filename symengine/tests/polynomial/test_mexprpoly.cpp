#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/printer.h>
#include <symengine/polys/uexprpoly.h>

using SymEngine::uexpr_poly;
using SymEngine::UExprPoly;
using SymEngine::Expression;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::map_int_Expr;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;
using SymEngine::MultivariateIntPolynomialExpr;
using SymEngine::MultivariatePolynomialExpr;
using SymEngine::Integer;
using SymEngine::Precedence;
using SymEngine::PrecedenceEnum;
using SymEngine::vec_uint;
using SymEngine::vec_int;
using SymEngine::vec_basic;
using SymEngine::vec_uint;
using SymEngine::RCPBasicKeyLess;
using SymEngine::map_basic_uint;

TEST_CASE("Constructing MultivariatePolynomialExpr",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression num1(integer(2));                             // 2
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp2(integer(2) - Expression(symbol("d")));  //(2 - d)
    Expression comp3(integer(-3) + Expression(symbol("e"))); //(-3 + e)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)
    vec_basic s;
    vec_int v;

    const MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    const MultivariatePolynomialExpr pprime
        = MultivariatePolynomialExpr::create(
            {y, x},
            {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    const MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 0}, comp1}, {{0, 0}, comp2}, {{2, 2}, comp3}, {{3, 4}, comp4}});
    const MultivariatePolynomialExpr p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});
    const MultivariatePolynomialExpr p4
        = MultivariatePolynomialExpr::create(s, {{v, Expression(0)}});
    const MultivariatePolynomialExpr p5
        = MultivariatePolynomialExpr::create(s, {{v, comp1}});
    const MultivariatePolynomialExpr p6
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, comp1},
                                                      {{0, -1}, comp2},
                                                      {{-2, 2}, comp3},
                                                      {{-3, -3}, comp4}});

    REQUIRE(vec_basic_eq_perm(
        p1.get_args(), {mul(integer(2), mul(pow(x, integer(2)), y)),
                        mul(negB.get_basic(), mul(x, pow(y, integer(2)))),
                        mul(symbol("a"), mul(x, y)), mul(integer(-3), y)}));
    REQUIRE(vec_basic_eq_perm(
        pprime.get_args(), {mul(negB.get_basic(), mul(pow(x, integer(2)), y)),
                            mul(integer(2), mul(x, pow(y, integer(2)))),
                            mul(symbol("a"), mul(x, y)), mul(integer(-3), x)}));
    REQUIRE(vec_basic_eq_perm(
        p2.get_args(),
        {mul(comp4.get_basic(), mul(pow(x, integer(3)), pow(y, integer(4)))),
         mul(comp3.get_basic(), mul(pow(x, integer(2)), pow(y, integer(2)))),
         mul(comp1.get_basic(), x), comp2.get_basic()}));
    REQUIRE(vec_basic_eq_perm(p3.get_args(), s));
    REQUIRE(vec_basic_eq_perm(p4.get_args(), s));
    REQUIRE(vec_basic_eq_perm(p5.get_args(), {comp1.get_basic()}));
    REQUIRE(vec_basic_eq_perm(
        p6.get_args(),
        {comp1.get_basic(),
         mul(comp3.get_basic(), mul(pow(x, integer(-2)), pow(y, integer(2)))),
         mul(comp2.get_basic(), pow(y, integer(-1))),
         mul(comp4.get_basic(),
             mul(pow(x, integer(-3)), pow(y, integer(-3))))}));
}

TEST_CASE("Testing MultivariatePolynomialExpr::__eq__(), __hash__, and compare",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> i = symbol("i");
    RCP<const Symbol> j = symbol("j");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression sum(add(i, j));
    Expression difference(sub(mul(two, i), j));
    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 0}, sum}, {{1, -1}, Expression(a)}, {{0, 2}, sum}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 0}, sum}, {{1, -1}, Expression(a) * -1}, {{0, 2}, sum}});
    MultivariatePolynomialExpr p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 0}, sum + sum}, {{0, 2}, sum + sum}});
    MultivariatePolynomialExpr p4 = MultivariatePolynomialExpr::create(
        {a, b}, {{{2, 0}, sum * 2}, {{0, 2}, sum * 2}});
    vec_basic s;
    vec_int v;
    MultivariatePolynomialExpr p5
        = MultivariatePolynomialExpr::create(s, {{v, Expression(0)}});
    MultivariatePolynomialExpr p6
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}});
    MultivariatePolynomialExpr p7
        = MultivariatePolynomialExpr::create(s, {{v, sum}});
    MultivariatePolynomialExpr p8
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, sum}});

    REQUIRE(p1.__eq__(p1));
    REQUIRE(!(p2.__eq__(p1)));
    REQUIRE(p3.__eq__(add_mult_poly(p1, p2)));
    REQUIRE(p5.__eq__(p6));
    REQUIRE(p7.__eq__(p8));
    REQUIRE(!p6.__eq__(p7));

    // Only requre that the same polynomial hash to the same value and that
    // different polynomials
    // hash to different values
    // Don't want to require a polynomial to have a particular hash in case
    // someone comes up with
    // a better hash function
    REQUIRE(p3.__hash__() == add_mult_poly(p1, p2).__hash__());
    REQUIRE(p1.__hash__() != p2.__hash__());
    REQUIRE(p3.__hash__() != p4.__hash__());

    // Same for compare.
    REQUIRE(0 == p3.compare(add_mult_poly(p1, p2)));
    REQUIRE(0 != p1.compare(p2));
    REQUIRE(0 != p3.compare(p4));
}

TEST_CASE("Testing MultivariatePolynomialExpr::eval",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));
    Expression ex(add(b, c));
    Expression why(mul(a, b));
    Expression zee(div(a, b));

    MultivariatePolynomialExpr p
        = MultivariatePolynomialExpr::create({x, y, z}, {{{2, 0, 0}, expr1},
                                                         {{0, 2, 0}, expr2},
                                                         {{0, 0, 2}, expr3},
                                                         {{1, 1, 1}, expr4},
                                                         {{1, 1, 0}, expr1},
                                                         {{0, 1, 1}, expr2},
                                                         {{1, 0, 0}, expr1},
                                                         {{0, 1, 0}, expr2},
                                                         {{0, 0, 1}, expr3},
                                                         {{0, 0, 0}, expr4},
                                                         {{-1, -1, -1}, expr1},
                                                         {{-2, -2, -2}, expr2},
                                                         {{-2, 2, -2}, expr3}});
    std::map<RCP<const Basic>, Expression, RCPBasicKeyLess> m1
        = {{x, Expression(0)}, {y, Expression(0)}, {z, Expression(0)}};
    std::map<RCP<const Basic>, Expression, RCPBasicKeyLess> m2
        = {{x, ex}, {y, why}, {z, zee}};
    REQUIRE(p.eval(m1) == expr4);
    REQUIRE(p.eval(m2)
            == expr1 * pow_ex(ex, 2) + expr2 * pow_ex(why, 2)
                   + expr3 * pow_ex(zee, 2) + expr4 * ex * why * zee
                   + expr1 * ex * why + expr2 * why * zee + expr1 * ex
                   + expr2 * why + expr3 * zee + expr4
                   + expr1 * pow_ex(ex, -1) * pow_ex(why, -1) * pow_ex(zee, -1)
                   + expr2 * pow_ex(ex, -2) * pow_ex(why, -2) * pow_ex(zee, -2)
                   + expr3 * pow_ex(ex, -2) * pow_ex(why, 2) * pow_ex(zee, -2));
}

TEST_CASE("Testing derivative of MultivariatePolynomialExpr",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));
    MultivariatePolynomialExpr p
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 1}, expr1},
                                                      {{1, 2}, expr2},
                                                      {{2, 0}, expr3},
                                                      {{0, 2}, expr4},
                                                      {{1, 0}, expr1},
                                                      {{0, 1}, expr2},
                                                      {{0, 0}, expr3},
                                                      {{-1, 0}, expr4},
                                                      {{0, -2}, expr1}});

    MultivariatePolynomialExpr q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, expr1 * 2},
                                                      {{0, 2}, expr2},
                                                      {{1, 0}, expr3 * 2},
                                                      {{0, 0}, expr1},
                                                      {{-2, 0}, expr4 * -1}});
    MultivariatePolynomialExpr q2
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 0}, expr1},
                                                      {{1, 1}, expr2 * 2},
                                                      {{0, 1}, expr4 * 2},
                                                      {{0, 0}, expr2},
                                                      {{0, -3}, expr1 * -2}});
    MultivariatePolynomialExpr q3
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}});

    REQUIRE(MultivariatePolynomialExpr::diff(p, x).__eq__(q1));
    REQUIRE(MultivariatePolynomialExpr::diff(p, y).__eq__(q2));
    REQUIRE(MultivariatePolynomialExpr::diff(p, z).__eq__(q3));
}

TEST_CASE("Testing MultivariatePolynomialExpr::get_args()",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));
    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, Expression(1)},
                    {{1, 1, 1}, Expression(2)},
                    {{0, 0, 2}, Expression(1)}});
    MultivariatePolynomialExpr p2
        = MultivariatePolynomialExpr::create({x, y, z}, {{{0, 0, 0}, expr1},
                                                         {{1, 1, 1}, expr2},
                                                         {{0, 0, 2}, expr3},
                                                         {{0, 2, 0}, expr4},
                                                         {{-1, -1, -1}, expr2},
                                                         {{0, 0, -2}, expr3},
                                                         {{0, -2, 0}, expr4}});
    REQUIRE(
        vec_basic_eq_perm(p1.get_args(), {mul(integer(2), mul(x, mul(y, z))),
                                          pow(z, integer(2)), one}));
    REQUIRE(vec_basic_eq_perm(
        p2.get_args(),
        {mul(expr2.get_basic(), mul(x, mul(y, z))),
         mul(expr4.get_basic(), pow(y, integer(2))),
         mul(expr3.get_basic(), pow(z, integer(2))), expr1.get_basic(),
         mul(expr3.get_basic(), pow(z, integer(-2))),
         mul(expr4.get_basic(), pow(y, integer(-2))),
         mul(expr2.get_basic(),
             mul(pow(x, integer(-1)),
                 mul(pow(y, integer(-1)), pow(z, integer(-1)))))}));
}

TEST_CASE("Testing MultivariatePolynomialExpr negation"
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression num1(integer(2));                             // 2
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp2(integer(2) - Expression(symbol("d")));  //(2 - d)
    Expression comp3(integer(-3) + Expression(symbol("e"))); //(-3 + e)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 1}, a}, {{1, -2}, negB}, {{-2, 1}, num1}, {{0, 1}, negNum}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 0}, comp1}, {{0, 0}, comp2}, {{2, 2}, comp3}, {{3, 4}, comp4}});
    MultivariatePolynomialExpr p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});

    MultivariatePolynomialExpr q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, a * -1},
                                                      {{1, -2}, negB * -1},
                                                      {{-2, 1}, num1 * -1},
                                                      {{0, 1}, negNum * -1}});
    MultivariatePolynomialExpr q2
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, comp1 * -1},
                                                      {{0, 0}, comp2 * -1},
                                                      {{2, 2}, comp3 * -1},
                                                      {{3, 4}, comp4 * -1}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});

    REQUIRE(p1.neg().__eq__(q1));
    REQUIRE(p2.neg().__eq__(q2));
    REQUIRE(p3.neg().__eq__(q3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExprs with the same set of variables",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression num1(integer(2));                             // 2
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 1}, a}, {{1, 0}, negB}, {{2, -1}, num1}, {{0, -1}, negNum}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 0}, comp1}, {{0, 0}, comp4}, {{0, -1}, comp4}});
    MultivariatePolynomialExpr p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a},
                 {{1, 0}, negB + comp1},
                 {{2, -1}, num1},
                 {{0, 0}, comp4},
                 {{0, -1}, comp4 + negNum}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, -1}, num1},
                 {{1, 1}, a},
                 {{1, 0}, (-1 * comp1) + negB},
                 {{0, -1}, negNum - comp4},
                 {{0, 0}, comp4 * -1}});
    MultivariatePolynomialExpr q22
        = MultivariatePolynomialExpr::create({x, y}, {{{2, -1}, -1 * num1},
                                                      {{1, 1}, -1 * a},
                                                      {{1, 0}, comp1 - negB},
                                                      {{0, -1}, comp4 - negNum},
                                                      {{0, 0}, comp4}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 1}, a * comp1},
                 {{1, 1}, a * comp4},
                 {{1, 0}, a * comp4 + negB * comp4},
                 {{2, 0}, negB * comp1},
                 {{1, -1}, negB * comp4 + negNum * comp1},
                 {{3, -1}, num1 * comp1},
                 {{2, -1}, num1 * comp4},
                 {{2, -2}, num1 * comp4},
                 {{0, -1}, negNum * comp4},
                 {{0, -2}, negNum * comp4}});

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(add_mult_poly(p1, p3).__eq__(p1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q22));
    REQUIRE(sub_mult_poly(p1, p3).__eq__(p1));

    REQUIRE(mul_mult_poly(p1, p2).__eq__(q3));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, p3).__eq__(p3));
    REQUIRE(mul_mult_poly(p3, p1).__eq__(p3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivaritePolynomials with disjoint sets of varables",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> n = symbol("n");
    RCP<const Symbol> m = symbol("m");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a}, {{-1, 0}, negB}, {{0, 0}, negNum}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {m, n}, {{{1, 0}, comp1}, {{2, -1}, comp4}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{0, 0, 1, 1}, a},
                       {{0, 0, -1, 0}, negB},
                       {{0, 0, 0, 0}, negNum},
                       {{1, 0, 0, 0}, comp1},
                       {{2, -1, 0, 0}, comp4}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{0, 0, 1, 1}, a},
                       {{0, 0, -1, 0}, negB},
                       {{0, 0, 0, 0}, negNum},
                       {{1, 0, 0, 0}, comp1 * -1},
                       {{2, -1, 0, 0}, comp4 * -1}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{0, 0, 1, 1}, a * -1},
                       {{0, 0, -1, 0}, negB * -1},
                       {{0, 0, 0, 0}, negNum * -1},
                       {{1, 0, 0, 0}, comp1},
                       {{2, -1, 0, 0}, comp4}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{2, -1, 1, 1}, a * comp4},
                       {{2, -1, -1, 0}, negB * comp4},
                       {{2, -1, 0, 0}, negNum * comp4},
                       {{1, 0, 1, 1}, a * comp1},
                       {{1, 0, -1, 0}, negB * comp1},
                       {{1, 0, 0, 0}, negNum * comp1}});

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q4));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExprs with an overlapping set of variables",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, -1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {y, z}, {{{1, 0}, comp1}, {{-2, 1}, comp4}});

    MultivariatePolynomialExpr q1
        = MultivariatePolynomialExpr::create({x, y, z}, {{{1, -1, 0}, a},
                                                         {{1, 0, 0}, negB},
                                                         {{0, 0, 0}, negNum},
                                                         {{0, 1, 0}, comp1},
                                                         {{0, -2, 1}, comp4}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, -1, 0}, a},
                    {{1, 0, 0}, negB},
                    {{0, 0, 0}, negNum},
                    {{0, 1, 0}, comp1 * -1},
                    {{0, -2, 1}, comp4 * -1}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, -1, 0}, a * -1},
                    {{1, 0, 0}, negB * -1},
                    {{0, 0, 0}, negNum * -1},
                    {{0, 1, 0}, comp1},
                    {{0, -2, 1}, comp4}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 0, 0}, a * comp1},
                    {{1, -3, 1}, a * comp4},
                    {{1, 1, 0}, negB * comp1},
                    {{1, -2, 1}, negB * comp4},
                    {{0, 1, 0}, negNum * comp1},
                    {{0, -2, 1}, comp4 * negNum}});

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q4));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "UExprPolys with the same variable",
          "[MultivariatePolynomialExpr][UExprPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));
    Expression expr5(add(b, c));
    Expression expr6(mul(a, b));
    Expression expr7(div(a, b));
    RCP<const UExprPoly> p1
        = uexpr_poly(x, {{1, expr1}, {2, expr2}, {0, expr3}});
    RCP<const UExprPoly> p2 = uexpr_poly(x, {{0, expr4}, {1, expr1}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x}, {{{1}, expr1 + expr1}, {{0}, expr4 + expr3}, {{2}, expr2}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x}, {{{0}, expr3 - expr4}, {{1}, expr1 - expr1}, {{2}, expr2}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x}, {{{0}, expr4 - expr3}, {{1}, expr1 - expr1}, {{2}, expr2 * -1}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x}, {{{3}, expr2 * expr1},
              {{2}, expr2 * expr4 + expr1 * expr1},
              {{1}, expr1 * expr4 + expr1 * expr3},
              {{0}, expr3 * expr4}});

    REQUIRE(add_mult_poly(*p1, *p2).__eq__(q1));
    REQUIRE(add_mult_poly(*p2, *p1).__eq__(q1));
    REQUIRE(sub_mult_poly(*p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, *p1).__eq__(q3));
    REQUIRE(mul_mult_poly(*p1, *p2).__eq__(q4));
    REQUIRE(mul_mult_poly(*p2, *p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "UExprPolys with the different variables",
          "[MultivariatePolynomialExpr][UExprPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(div(b, a));
    Expression expr5(add(b, c));
    RCP<const UExprPoly> p1
        = uexpr_poly(x, {{1, expr1}, {2, expr2}, {0, expr3}});
    RCP<const UExprPoly> p2 = uexpr_poly(y, {{0, expr4}, {1, expr5}});

    MultivariatePolynomialExpr q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1},
                                                      {{2, 0}, expr2},
                                                      {{0, 0}, expr3 + expr4},
                                                      {{0, 0}, expr4},
                                                      {{0, 1}, expr5}});
    MultivariatePolynomialExpr q2
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1},
                                                      {{2, 0}, expr2},
                                                      {{0, 0}, expr3 - expr4},
                                                      {{0, 1}, expr5 * -1}});
    MultivariatePolynomialExpr q3
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1 * -1},
                                                      {{2, 0}, expr2 * -1},
                                                      {{0, 0}, expr4 - expr3},
                                                      {{0, 1}, expr5}});
    MultivariatePolynomialExpr q4
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 1}, expr2 * expr5},
                                                      {{2, 0}, expr2 * expr4},
                                                      {{1, 1}, expr1 * expr5},
                                                      {{1, 0}, expr1 * expr4},
                                                      {{0, 1}, expr3 * expr5},
                                                      {{0, 0}, expr3 * expr4}});

    REQUIRE(add_mult_poly(*p1, *p2).__eq__(q1));
    REQUIRE(add_mult_poly(*p2, *p1).__eq__(q1));
    REQUIRE(sub_mult_poly(*p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, *p1).__eq__(q3));
    REQUIRE(mul_mult_poly(*p1, *p2).__eq__(q4));
    REQUIRE(mul_mult_poly(*p2, *p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExprs with a UExprPoly whose variable "
          "are in the variable set",
          "[MultivariatePolynomialExpr][UExprPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const UExprPoly> p2
        = uexpr_poly(y, {{0, comp4}, {1, Expression(integer(2))}, {2, comp1}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a},
                 {{0, 2}, comp1},
                 {{1, 0}, negB},
                 {{0, 1}, Expression(2)},
                 {{0, 0}, comp4 + negNum}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a},
                 {{0, 2}, comp1 * -1},
                 {{1, 0}, negB},
                 {{0, 1}, Expression(-2)},
                 {{0, 0}, -1 * comp4 + negNum}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a * -1},
                 {{0, 2}, comp1},
                 {{1, 0}, negB * -1},
                 {{0, 1}, Expression(2)},
                 {{0, 0}, comp4 - negNum}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 3}, a * comp1},
                 {{1, 2}, 2 * a + negB * comp1},
                 {{1, 1}, a * comp4 + negB * 2},
                 {{0, 2}, negNum * comp1},
                 {{1, 0}, negB * comp4},
                 {{0, 1}, 2 * negNum},
                 {{0, 0}, negNum * comp4}});

    REQUIRE(add_mult_poly(p1, *p2).__eq__(q1));
    REQUIRE(add_mult_poly(*p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, *p2).__eq__(q4));
    REQUIRE(mul_mult_poly(*p2, p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExprs with a UExprPoly whose variables "
          "are not in the variable set",
          "[MultivariatePolynomialExpr][UExprPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const UExprPoly> p2
        = uexpr_poly(z, {{0, comp4}, {1, Expression(integer(2))}, {2, comp1}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 0}, a},
                    {{0, 0, 2}, comp1},
                    {{1, 0, 0}, negB},
                    {{0, 0, 1}, Expression(2)},
                    {{0, 0, 0}, negNum + comp4}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 0}, a},
                    {{0, 0, 2}, comp1 * -1},
                    {{1, 0, 0}, negB},
                    {{0, 0, 1}, Expression(-2)},
                    {{0, 0, 0}, negNum - comp4}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 0}, a * -1},
                    {{0, 0, 2}, comp1},
                    {{1, 0, 0}, negB * -1},
                    {{0, 0, 1}, Expression(2)},
                    {{0, 0, 0}, -1 * negNum + comp4}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 2}, a * comp1},
                    {{1, 1, 1}, 2 * a},
                    {{1, 0, 2}, negB * comp1},
                    {{1, 1, 0}, a * comp4},
                    {{1, 0, 1}, 2 * negB},
                    {{0, 0, 2}, negNum * comp1},
                    {{1, 0, 0}, negB * comp4},
                    {{0, 0, 1}, 2 * negNum},
                    {{0, 0, 0}, negNum * comp4}});

    REQUIRE(add_mult_poly(p1, *p2).__eq__(q1));
    REQUIRE(add_mult_poly(*p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, *p2).__eq__(q4));
    REQUIRE(mul_mult_poly(*p2, p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExpr with empty set of variables ",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    vec_basic s;
    vec_int v;
    MultivariatePolynomialExpr p1
        = MultivariatePolynomialExpr::create(s, {{v, expr1}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2}, {{0, 1}, expr3}, {{-1, 0}, expr4}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2 + expr1}, {{0, 1}, expr3}, {{-1, 0}, expr4}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2 - expr1}, {{0, 1}, expr3}, {{-1, 0}, expr4}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{0, 0}, expr1 - expr2}, {{0, 1}, -1 * expr3}, {{-1, 0}, -1 * expr4}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2 * expr1},
                 {{0, 1}, expr3 * expr1},
                 {{-1, 0}, expr4 * expr1}});

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q3));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q2));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q4));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q4));
}
/*
TEST_CASE("Testing Precedence of MultivariatePolynomialExpr",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));
    Expression expr5(a);
    Precedence Prec;
    RCP<const MultivariatePolynomialExpr> p1
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}});
    RCP<const MultivariatePolynomialExpr> p2 =
MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 0}, expr1}, {{0, 0}, expr2}});
    RCP<const MultivariatePolynomialExpr> p3
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, expr5}});
    RCP<const MultivariatePolynomialExpr> p4
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, Expression(1)}});
    RCP<const MultivariatePolynomialExpr> p5
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, expr4}});
    RCP<const MultivariatePolynomialExpr> p6
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 0}, Expression(1)}});
    RCP<const MultivariatePolynomialExpr> p7
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1}});

    REQUIRE(Prec.getPrecedence(p1) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p2) == PrecedenceEnum::Add);
    REQUIRE(Prec.getPrecedence(p3) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p4) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p5) == PrecedenceEnum::Mul);
    REQUIRE(Prec.getPrecedence(p6) == PrecedenceEnum::Pow);
    REQUIRE(Prec.getPrecedence(p7) == PrecedenceEnum::Mul);
}*/
TEST_CASE("Testing truncation of MultivariatePolynomialExpr",
          "[MultivariatePolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    map_basic_uint m = {{x, 1}, {y, 2}, {z, 3}};

    MultivariatePolynomialExpr p1
        = MultivariatePolynomialExpr::create({x, y, z}, {{{0, 0, 0}, expr1},
                                                         {{1, 0, 0}, expr2},
                                                         {{0, 1, 0}, expr3},
                                                         {{0, 0, 1}, expr4},
                                                         {{2, 0, 0}, expr1},
                                                         {{0, 3, 0}, expr2},
                                                         {{0, 0, 4}, expr3},
                                                         {{1, 2, 3}, expr4}});

    MultivariatePolynomialExpr q1
        = MultivariatePolynomialExpr::create({x, y, z}, {{{0, 0, 0}, expr1},
                                                         {{1, 0, 0}, expr2},
                                                         {{0, 1, 0}, expr3},
                                                         {{0, 0, 1}, expr4},
                                                         {{1, 2, 3}, expr4}});

    REQUIRE(q1.__eq__(p1.truncate(m)));
}

TEST_CASE("Testing operator==, operator!=",
          "[MultivariatePolynomialExpr],[Expression]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    MultivariatePolynomialExpr p1
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}});
    MultivariatePolynomialExpr p2
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, expr1}});
    MultivariatePolynomialExpr p3
        = MultivariatePolynomialExpr::create({x, y, z}, {{{0, 0, 0}, expr2}});
    MultivariatePolynomialExpr p4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, Expression(0)}});

    REQUIRE(p1 == 0);
    REQUIRE(p2 == expr1);
    REQUIRE(p1 != expr1);
    REQUIRE(p2 != 0);
    REQUIRE(p3 != p1);
    REQUIRE(p1 == p4);
}

TEST_CASE("Testing operator+, operator+=",
          "[MultivariatePolynomialExpr],[Expression]")
{
    RCP<const Symbol> w = symbol("w");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y, z},
        {{{0, 0, 0}, Expression(10)}, {{1, 1, 1}, expr2}, {{0, 0, 4}, expr3}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, Expression(-10)},
                    {{1, 1, 1}, expr2},
                    {{0, 0, 2}, expr3},
                    {{0, 2, 0}, expr4},
                    {{-1, -1, -1}, expr2},
                    {{0, 0, -2}, expr3},
                    {{0, -2, 0}, expr4}});

    MultivariatePolynomialExpr p3
        = MultivariatePolynomialExpr::create({w, y}, {{{1, 1}, expr1}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 1}, expr2 + expr2},
                    {{0, 0, 4}, expr3},
                    {{0, 0, 2}, expr3},
                    {{0, 2, 0}, expr4},
                    {{-1, -1, -1}, expr2},
                    {{0, 0, -2}, expr3},
                    {{0, -2, 0}, expr4}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, expr1 + Expression(-10)},
                    {{1, 1, 1}, expr2},
                    {{0, 0, 2}, expr3},
                    {{0, 2, 0}, expr4},
                    {{-1, -1, -1}, expr2},
                    {{0, 0, -2}, expr3},
                    {{0, -2, 0}, expr4}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {w, x, y, z}, {{{0, 0, 0, 0}, Expression(10)},
                       {{0, 1, 1, 1}, expr2},
                       {{0, 0, 0, 4}, expr3},
                       {{1, 0, 1, 0}, expr1}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 1}, expr2}, {{0, 0, 4}, expr3}});

    REQUIRE(q1 == p1 + p2);
    REQUIRE(q3 == p1 + p3);
    REQUIRE(q2 == p2 + expr1);
    REQUIRE(q2 == expr1 + p2);
    REQUIRE(q4 == Expression(-10) + p1);

    MultivariatePolynomialExpr p4 = p1;
    MultivariatePolynomialExpr p5 = p1;

    p1 += p2;
    p2 += expr1;
    p4 += Expression(-10);
    p5 += p3;

    REQUIRE(q1 == p1);
    REQUIRE(q2 == p2);
    REQUIRE(q4 == p4);
    REQUIRE(p5 == q3);
}

TEST_CASE("Testing operator-, operator-=",
          "[MultivariatePolynomialExpr],[Expression]")
{
    RCP<const Symbol> w = symbol("w");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y, z},
        {{{0, 0, 0}, Expression(10)}, {{1, 1, 1}, expr2}, {{0, 0, 4}, expr3}});
    MultivariatePolynomialExpr p2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, Expression(10)},
                    {{1, 1, 1}, expr2},
                    {{0, 0, 2}, expr3},
                    {{0, 2, 0}, expr4},
                    {{-1, -1, -1}, expr2},
                    {{0, 0, -2}, expr3},
                    {{0, -2, 0}, expr4}});
    MultivariatePolynomialExpr p3
        = MultivariatePolynomialExpr::create({w, y}, {{{1, 1}, expr1}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 1}, expr2 - expr2},
                    {{0, 0, 4}, expr3},
                    {{0, 0, 2}, expr3 * -1},
                    {{0, 2, 0}, expr4 * -1},
                    {{-1, -1, -1}, expr2 * -1},
                    {{0, 0, -2}, expr3 * -1},
                    {{0, -2, 0}, expr4 * -1}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, Expression(10) - expr1},
                    {{1, 1, 1}, expr2},
                    {{0, 0, 2}, expr3},
                    {{0, 2, 0}, expr4},
                    {{-1, -1, -1}, expr2},
                    {{0, 0, -2}, expr3},
                    {{0, -2, 0}, expr4}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, expr1 - Expression(10)},
                    {{1, 1, 1}, expr2 * -1},
                    {{0, 0, 2}, expr3 * -1},
                    {{0, 2, 0}, expr4 * -1},
                    {{-1, -1, -1}, expr2 * -1},
                    {{0, 0, -2}, expr3 * -1},
                    {{0, -2, 0}, expr4 * -1}});
    MultivariatePolynomialExpr q4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 1}, expr2}, {{0, 0, 4}, expr3}});
    MultivariatePolynomialExpr q5 = MultivariatePolynomialExpr::create(
        {w, x, y, z}, {{{0, 0, 0, 0}, Expression(10)},
                       {{0, 1, 1, 1}, expr2},
                       {{0, 0, 0, 4}, expr3},
                       {{1, 0, 1, 0}, expr1 * -1}});

    REQUIRE(q1 == p1 - p2);
    REQUIRE(q2 == p2 - expr1);
    REQUIRE(q4 == p1 - Expression(10));
    REQUIRE(q3 == expr1 - p2);
    REQUIRE(q5 == p1 - p3);

    MultivariatePolynomialExpr p4 = p1;
    MultivariatePolynomialExpr p5 = p1;

    p1 -= p2;
    p2 -= expr1;
    p4 -= Expression(10);
    p5 -= p3;

    REQUIRE(q1 == p1);
    REQUIRE(q2 == p2);
    REQUIRE(q4 == p4);
    REQUIRE(q5 == p5);
}

TEST_CASE("Testing operator*, operator*=",
          "[MultivariatePolynomialExpr],[Expression]")
{
    RCP<const Symbol> w = symbol("w");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y, z},
        {{{0, 0, 0}, expr1}, {{1, 1, 1}, expr2}, {{0, 0, 4}, expr3}});
    MultivariatePolynomialExpr p2
        = MultivariatePolynomialExpr::create({x, y, z}, {{{0, 0, 0}, expr1},
                                                         {{1, 1, 1}, expr2},
                                                         {{0, 0, 2}, expr3},
                                                         {{0, 2, 0}, expr4},
                                                         {{-1, -1, -1}, expr2},
                                                         {{0, 0, -2}, expr3},
                                                         {{0, -2, 0}, expr4}});
    MultivariatePolynomialExpr p3
        = MultivariatePolynomialExpr::create({w, y}, {{{1, 1}, expr1}});
    MultivariatePolynomialExpr p5
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(1)},
                                                      {{1, 0}, Expression(1)},
                                                      {{0, 1}, Expression(1)}});
    MultivariatePolynomialExpr p6
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, Expression(1)},
                                                      {{0, 1}, Expression(-1)},
                                                      {{0, 0}, Expression(1)}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, expr1 * expr1 + expr2 * expr2},
                    {{1, 1, 1}, expr2 * expr1 + expr1 * expr2},
                    {{0, 0, 2}, expr3 * expr1 + expr3 * expr3},
                    {{0, 2, 0}, expr4 * expr1},
                    {{-1, -1, -1}, expr2 * expr1},
                    {{0, 0, -2}, expr3 * expr1},
                    {{0, -2, 0}, expr4 * expr1},

                    {{2, 2, 2}, expr2 * expr2},
                    {{1, 1, 3}, expr3 * expr2},
                    {{1, 3, 1}, expr4 * expr2},
                    {{1, 1, -1}, expr3 * expr2},
                    {{1, -1, 1}, expr4 * expr2},

                    {{0, 0, 4}, expr1 * expr3},
                    {{1, 1, 5}, expr2 * expr3},
                    {{0, 0, 6}, expr3 * expr3},
                    {{0, 2, 4}, expr4 * expr3},
                    {{-1, -1, 3}, expr2 * expr3},
                    {{0, -2, 4}, expr4 * expr3}});
    MultivariatePolynomialExpr q2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, expr1 * expr1},
                    {{1, 1, 1}, expr2 * expr1},
                    {{0, 0, 2}, expr3 * expr1},
                    {{0, 2, 0}, expr4 * expr1},
                    {{-1, -1, -1}, expr2 * expr1},
                    {{0, 0, -2}, expr3 * expr1},
                    {{0, -2, 0}, expr4 * expr1}});
    MultivariatePolynomialExpr q3 = MultivariatePolynomialExpr::create(
        {w, x, y, z}, {{{1, 0, 1, 0}, expr1 * expr1},
                       {{1, 1, 2, 1}, expr2 * expr1},
                       {{1, 0, 1, 4}, expr3 * expr1}});
    MultivariatePolynomialExpr q4
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(1)},
                                                      {{1, 0}, Expression(1)},
                                                      {{0, 2}, Expression(-1)},
                                                      {{2, 1}, Expression(1)},
                                                      {{1, 2}, Expression(1)}});

    REQUIRE(q1 == p1 * p2);
    REQUIRE(q2 == p2 * expr1);
    REQUIRE(q2 == expr1 * p2);
    REQUIRE(q3 == p3 * p1);

    MultivariatePolynomialExpr p4 = p1;
    MultivariatePolynomialExpr p7 = p1;
    MultivariatePolynomialExpr p8 = p1;

    p1 *= p2;
    p2 *= expr1;
    p4 *= p3;
    p5 *= p6;
    p7 *= 0;
    p8 *= MultivariatePolynomialExpr::create({x, y, z},
                                             {{{0, 0, 0}, Expression(0)}});

    REQUIRE(q1 == p1);
    REQUIRE(q2 == p2);
    REQUIRE(q3 == p4);
    REQUIRE(p5 == q4);
    REQUIRE(p7 == 0);
    REQUIRE(p8 == 0);
}

TEST_CASE("Testing operator/, operator/=",
          "[MultivariatePolynomialExpr],[Expression]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(add(a, b));
    Expression expr2(sub(mul(two, a), b));
    Expression expr3(mul(a, c));
    Expression expr4(pow(b, a));

    MultivariatePolynomialExpr p1 = MultivariatePolynomialExpr::create(
        {x, y, z},
        {{{0, 0, 0}, expr1}, {{1, 1, 1}, expr2}, {{0, 0, 4}, expr3}});

    MultivariatePolynomialExpr q1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, expr1 / expr1},
                    {{1, 1, 1}, expr2 / expr1},
                    {{0, 0, 4}, expr3 / expr1}});

    REQUIRE(q1 == p1 / expr1);

    p1 /= expr1;

    REQUIRE(q1 == p1);
}
