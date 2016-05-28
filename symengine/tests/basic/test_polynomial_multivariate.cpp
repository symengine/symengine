#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/pow.h>
#include <symengine/printer.h>

using SymEngine::Expression;
using SymEngine::UnivariateIntPolynomial;
using SymEngine::UnivariatePolynomial;
using SymEngine::univariate_int_polynomial;
using SymEngine::univariate_polynomial;
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

using namespace SymEngine::literals;

TEST_CASE("Constructing MultivariateIntPolynomialExpr",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const MultivariateIntPolynomialExpr> P
        = MultivariateIntPolynomialExpr::create({x, y}, {{{1, 2}, 1_z},
                                                     {{1, 1}, 2_z},
                                                     {{0, 1}, 2_z},
                                                     {{1, 0}, 3_z},
                                                     {{0, 0}, 0_z}});

    REQUIRE(vec_basic_eq_perm(
        P->get_args(), {mul(x, pow(y, integer(2))), mul(integer(2), mul(x, y)),
                        mul(integer(3), x), mul(integer(2), y)}));

    RCP<const MultivariateIntPolynomialExpr> Pprime
        = MultivariateIntPolynomialExpr::create({y, x}, {{{1, 2}, 1_z},
                                                     {{1, 1}, 2_z},
                                                     {{0, 1}, 2_z},
                                                     {{1, 0}, 3_z},
                                                     {{0, 0}, 0_z}});

    REQUIRE(vec_basic_eq_perm(Pprime->get_args(),
                              {mul(pow(x, integer(2)), y),
                               mul(integer(2), mul(x, y)), mul(integer(2), x),
                               mul(integer(3), y)}));

    RCP<const MultivariateIntPolynomialExpr> P2
        = MultivariateIntPolynomialExpr::create({x, y}, {{{0, 0}, 0_z}});

    set_sym s;
    vec_uint v;
    RCP<const MultivariateIntPolynomialExpr> P3
        = MultivariateIntPolynomialExpr::create(s, {{v, 0_z}});

    REQUIRE(vec_basic_eq_perm(P2->get_args(), s));
    REQUIRE(vec_basic_eq_perm(P3->get_args(), s));

    RCP<const MultivariateIntPolynomialExpr> P4
        = MultivariateIntPolynomialExpr::create(s, {{v, 5_z}});
    REQUIRE(vec_basic_eq_perm(P4->get_args(), {integer(5)}));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr::__hash__() and compare",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    
    RCP<const MultivariateIntPolynomialExpr> p1
        = MultivariateIntPolynomialExpr::create(
            {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> p3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    RCP<const MultivariateIntPolynomialExpr> p4 = MultivariateIntPolynomialExpr::create(
        {a, b}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});

    // Only requre that the same polynomial hash to the same value and that
    // different polynomials
    // hash to different values
    // Don't want to require a polynomial to have a particular hash in case
    // someone comes up with
    // a better hash function
    REQUIRE(p3->__hash__() == add_mult_poly(*p1, *p2)->__hash__());
    REQUIRE(p1->__hash__() != p2->__hash__());
    REQUIRE(p3->__hash__() != p4->__hash__());

    // Same for compare.
    REQUIRE(0 == p3->compare(*add_mult_poly(*p1, *p2)));
    REQUIRE(0 != p1->compare(*p2));
    REQUIRE(0 != p3->compare(*p4));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr::__eq__(const Basic &o)",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const MultivariateIntPolynomialExpr> p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> p3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    RCP<const MultivariateIntPolynomialExpr> p4
        = MultivariateIntPolynomialExpr::create({x}, {{{0}, 5_z}});
    RCP<const MultivariateIntPolynomialExpr> p5
        = MultivariateIntPolynomialExpr::create({y}, {{{0}, 5_z}});
    RCP<const MultivariateIntPolynomialExpr> p6
        = MultivariateIntPolynomialExpr::create({x}, {{{0}, 0_z}});
    RCP<const MultivariateIntPolynomialExpr> p7
        = MultivariateIntPolynomialExpr::create({y}, {{{0}, 0_z}});

    REQUIRE(p1->__eq__(*p1));
    REQUIRE(!(p2->__eq__(*p1)));
    REQUIRE(p3->__eq__(*add_mult_poly(*p1, *p2)));
    REQUIRE(p4->__eq__(*p5));
    REQUIRE(p6->__eq__(*p7));
    REQUIRE(!p5->__eq__(*p6));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr::eval((std::map<RCP<const "
          "Symbol>, integer_class, RCPSymbolCompare> &vals)",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{2, 0, 0}, 1_z},
                                                        {{0, 2, 0}, 2_z},
                                                        {{0, 0, 2}, 3_z},
                                                        {{1, 1, 1}, 4_z},
                                                        {{1, 1, 0}, 1_z},
                                                        {{0, 1, 1}, 2_z},
                                                        {{1, 0, 0}, 1_z},
                                                        {{0, 1, 0}, 2_z},
                                                        {{0, 0, 1}, 3_z},
                                                        {{0, 0, 0}, 5_z}});
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> m1
        = {{x, 1_z}, {y, 2_z}, {z, 5_z}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m2
        = {{x, 0_z}, {y, 0_z}, {z, 0_z}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m3
        = {{x, -1_z}, {y, -2_z}, {z, -5_z}};

    REQUIRE(171_z == p->eval(m1));
    REQUIRE(5_z == p->eval(m2));
    REQUIRE(51_z == p->eval(m3));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr negation",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {{{1, 0, 0}, 1_z}, {{0, 1, 0}, -2_z}, {{0, 0, 1}, 3_z}});
    RCP<const MultivariateIntPolynomialExpr> p2 = p->neg();

    RCP<const MultivariateIntPolynomialExpr> q = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {{{1, 0, 0}, -1_z}, {{0, 1, 0}, 2_z}, {{0, 0, 1}, -3_z}});

    REQUIRE(p2->__eq__(*q));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with the same set of variables",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, 2_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 0, 0}, 4_z}});
    RCP<const MultivariateIntPolynomialExpr> p2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z}});

    RCP<const MultivariateIntPolynomialExpr> q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 2_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 7_z}});
    RCP<const MultivariateIntPolynomialExpr> q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{3, 2, 1}, 4_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 1, 2}, -1_z},
                                                        {{0, 0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {
                                                           {{2, 4, 6}, 1_z},
                                                           {{5, 3, 3}, 3_z},
                                                           {{6, 4, 2}, -4_z},
                                                           {{7, 3, 1}, -6_z},
                                                           {{1, 3, 5}, 1_z},
                                                           {{3, 3, 3}, 2_z},
                                                           {{4, 2, 2}, 3_z},
                                                           {{0, 1, 2}, 4_z},
                                                           {{4, 1, 0}, 9_z},
                                                           {{0, 0, 0}, 12_z},
                                                           {{3, 2, 1}, -2_z},
                                                           {{1, 2, 3}, 7_z},
                                                       });

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q3));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariteIntPolynomials with disjoint sets of varables",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p1
        = MultivariateIntPolynomialExpr::create({a, b, c}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, 2_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 0, 0}, 4_z}});
    RCP<const MultivariateIntPolynomialExpr> p2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z}});

    RCP<const MultivariateIntPolynomialExpr> q1 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, 1_z},
                             {{3, 2, 1, 0, 0, 0}, 2_z},
                             {{4, 1, 0, 0, 0, 0}, 3_z},
                             {{0, 0, 0, 0, 0, 0}, 7_z},
                             {{0, 0, 0, 1, 2, 3}, 1_z},
                             {{0, 0, 0, 3, 2, 1}, -2_z},
                             {{0, 0, 0, 0, 1, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q2 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, 1_z},
                             {{3, 2, 1, 0, 0, 0}, 2_z},
                             {{4, 1, 0, 0, 0, 0}, 3_z},
                             {{0, 0, 0, 0, 0, 0}, 1_z},
                             {{0, 0, 0, 1, 2, 3}, -1_z},
                             {{0, 0, 0, 3, 2, 1}, 2_z},
                             {{0, 0, 0, 0, 1, 2}, -1_z}});
    RCP<const MultivariateIntPolynomialExpr> q3 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, -1_z},
                             {{3, 2, 1, 0, 0, 0}, -2_z},
                             {{4, 1, 0, 0, 0, 0}, -3_z},
                             {{0, 0, 0, 0, 0, 0}, -1_z},
                             {{0, 0, 0, 1, 2, 3}, 1_z},
                             {{0, 0, 0, 3, 2, 1}, -2_z},
                             {{0, 0, 0, 0, 1, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q4 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 1, 2, 3}, 1_z},
                             {{3, 2, 1, 1, 2, 3}, 2_z},
                             {{4, 1, 0, 1, 2, 3}, 3_z},
                             {{0, 0, 0, 1, 2, 3}, 4_z},

                             {{1, 2, 3, 3, 2, 1}, -2_z},
                             {{3, 2, 1, 3, 2, 1}, -4_z},
                             {{4, 1, 0, 3, 2, 1}, -6_z},
                             {{0, 0, 0, 3, 2, 1}, -8_z},

                             {{1, 2, 3, 0, 1, 2}, 1_z},
                             {{3, 2, 1, 0, 1, 2}, 2_z},
                             {{4, 1, 0, 0, 1, 2}, 3_z},
                             {{0, 0, 0, 0, 1, 2}, 4_z},

                             {{1, 2, 3, 0, 0, 0}, 3_z},
                             {{3, 2, 1, 0, 0, 0}, 6_z},
                             {{4, 1, 0, 0, 0, 0}, 9_z},
                             {{0, 0, 0, 0, 0, 0}, 12_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with an overlapping set of variables",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 2}, 1_z}, {{4, 0}, 3_z}, {{0, 3}, 4_z}});
    RCP<const MultivariateIntPolynomialExpr> p2 = MultivariateIntPolynomialExpr::create(
        {y, z}, {{{2, 1}, -2_z}, {{0, 2}, 1_z}, {{1, 0}, 3_z}});

    RCP<const MultivariateIntPolynomialExpr> q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{4, 0, 0}, 3_z},
                                                        {{0, 3, 0}, 4_z},
                                                        {{0, 2, 1}, -2_z},
                                                        {{0, 0, 2}, 1_z},
                                                        {{0, 1, 0}, 3_z}});

    RCP<const MultivariateIntPolynomialExpr> q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{4, 0, 0}, 3_z},
                                                        {{0, 3, 0}, 4_z},
                                                        {{0, 2, 1}, 2_z},
                                                        {{0, 0, 2}, -1_z},
                                                        {{0, 1, 0}, -3_z}});

    RCP<const MultivariateIntPolynomialExpr> q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, -1_z},
                                                        {{4, 0, 0}, -3_z},
                                                        {{0, 3, 0}, -4_z},
                                                        {{0, 2, 1}, -2_z},
                                                        {{0, 0, 2}, 1_z},
                                                        {{0, 1, 0}, 3_z}});

    RCP<const MultivariateIntPolynomialExpr> q4
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 4, 1}, -2_z},
                                                        {{4, 2, 1}, -6_z},
                                                        {{0, 5, 1}, -8_z},

                                                        {{1, 2, 2}, 1_z},
                                                        {{4, 0, 2}, 3_z},
                                                        {{0, 3, 2}, 4_z},

                                                        {{1, 3, 0}, 3_z},
                                                        {{4, 1, 0}, 9_z},
                                                        {{0, 4, 0}, 12_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing derivative of MultivariateIntPolynomialExpr",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p
        = MultivariateIntPolynomialExpr::create({x, y}, {{{2, 1}, 3_z},
                                                     {{1, 2}, 2_z},
                                                     {{2, 0}, 3_z},
                                                     {{0, 2}, 2_z},
                                                     {{1, 0}, 3_z},
                                                     {{0, 1}, 2_z},
                                                     {{0, 0}, 5_z}});

    RCP<const MultivariateIntPolynomialExpr> q1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 1}, 6_z}, {{0, 2}, 2_z}, {{1, 0}, 6_z}, {{0, 0}, 3_z}});
    RCP<const MultivariateIntPolynomialExpr> q2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 3_z}, {{1, 1}, 4_z}, {{0, 1}, 4_z}, {{0, 0}, 2_z}});
    RCP<const MultivariateIntPolynomialExpr> q3
        = MultivariateIntPolynomialExpr::create({x, y}, {{{0, 0}, 0_z}});
    REQUIRE(eq(*p->diff(x), *q1));
    REQUIRE(eq(*p->diff(y), *q2));
    REQUIRE(eq(*p->diff(z), *q3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with a UnivariateIntPolynomial whose "
          "variable are in the variable set",
          "[MultivariateIntPolynomialExpr][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 2_z},
                                                        {{1, 0, 0}, 1_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(x, {{1, 1_z}, {2, 1_z}});
    RCP<const UnivariateIntPolynomial> p3
        = univariate_int_polynomial(y, {{1, 1_z}, {2, 1_z}});

    RCP<const MultivariateIntPolynomialExpr> q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 3_z},
                                                        {{1, 0, 0}, 2_z}});
    RCP<const MultivariateIntPolynomialExpr> q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, -1_z},
                                                        {{3, 2, 1}, 2_z},
                                                        {{0, 1, 2}, -1_z},
                                                        {{0, 0, 0}, -3_z},
                                                        {{2, 0, 0}, -1_z}});
    RCP<const MultivariateIntPolynomialExpr> q4
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{2, 2, 3}, 1_z},
                                                        {{4, 2, 1}, -2_z},
                                                        {{1, 1, 2}, 1_z},
                                                        {{1, 0, 0}, 3_z},
                                                        {{3, 0, 0}, 3_z},

                                                        {{3, 2, 3}, 1_z},
                                                        {{5, 2, 1}, -2_z},
                                                        {{2, 1, 2}, 1_z},
                                                        {{2, 0, 0}, 4_z},
                                                        {{4, 0, 0}, 2_z}});
    RCP<const MultivariateIntPolynomialExpr> q5
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 2_z},
                                                        {{1, 0, 0}, 1_z},
                                                        {{0, 1, 0}, 1_z},
                                                        {{0, 2, 0}, 1_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));

    REQUIRE(eq(*add_mult_poly(*p1, *p3), *q5));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with a UnivariateIntPolynomial whose "
          "variables are not in the variable set",
          "[MultivariateIntPolynomialExpr][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 2}, 1_z}, {{2, 1}, -2_z}, {{0, 1}, 1_z}, {{0, 0}, 3_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(z, {{1, 1_z}, {2, 1_z}});

    RCP<const MultivariateIntPolynomialExpr> q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{2, 1, 0}, -2_z},
                                                        {{0, 1, 0}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{0, 0, 1}, 1_z},
                                                        {{0, 0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{2, 1, 0}, -2_z},
                                                        {{0, 1, 0}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{0, 0, 1}, -1_z},
                                                        {{0, 0, 2}, -1_z}});
    RCP<const MultivariateIntPolynomialExpr> q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, -1_z},
                                                        {{2, 1, 0}, 2_z},
                                                        {{0, 1, 0}, -1_z},
                                                        {{0, 0, 0}, -3_z},
                                                        {{0, 0, 1}, 1_z},
                                                        {{0, 0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q4
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 1}, 1_z},
                                                        {{2, 1, 1}, -2_z},
                                                        {{0, 1, 1}, 1_z},
                                                        {{0, 0, 1}, 3_z},

                                                        {{1, 2, 2}, 1_z},
                                                        {{2, 1, 2}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 2}, 3_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UnivariateIntPolynomials with different variables",
          "[MultivariateIntPolynomialExpr][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UnivariateIntPolynomial> p1
        = univariate_int_polynomial(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(y, {{0, 1_z}, {1, 1_z}});

    RCP<const MultivariateIntPolynomialExpr> q1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 0}, -1_z}, {{2, 0}, 3_z}, {{0, 0}, 1_z}, {{0, 1}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q2 = MultivariateIntPolynomialExpr::create(
        {x, y},
        {{{1, 0}, -1_z}, {{2, 0}, 3_z}, {{0, 0}, -1_z}, {{0, 1}, -1_z}});
    RCP<const MultivariateIntPolynomialExpr> q3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 0}, 1_z}, {{2, 0}, -3_z}, {{0, 0}, 1_z}, {{0, 1}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q4 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 1}, 3_z}, {{2, 0}, 3_z}, {{1, 1}, -1_z}, {{1, 0}, -1_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UnivariateIntPolynomials with the same variables",
          "[MultivariateIntPolynomialExpr][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> p1
        = univariate_int_polynomial(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 1_z}});

    RCP<const MultivariateIntPolynomialExpr> q1
        = MultivariateIntPolynomialExpr::create({x}, {{{0}, 1_z}, {{2}, 3_z}});
    RCP<const MultivariateIntPolynomialExpr> q2 = MultivariateIntPolynomialExpr::create(
        {x}, {{{0}, -1_z}, {{1}, -2_z}, {{2}, 3_z}});
    RCP<const MultivariateIntPolynomialExpr> q3 = MultivariateIntPolynomialExpr::create(
        {x}, {{{0}, 1_z}, {{1}, 2_z}, {{2}, -3_z}});
    RCP<const MultivariateIntPolynomialExpr> q4 = MultivariateIntPolynomialExpr::create(
        {x}, {{{1}, -1_z}, {{2}, 2_z}, {{3}, 3_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
}

TEST_CASE("Testing addition, subtraction, and multiplication of "
          "MultivariateIntPolynomialExprs with empty variable sets",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    set_sym s;
    vec_uint v;
    RCP<const MultivariateIntPolynomialExpr> p1
        = MultivariateIntPolynomialExpr::create(s, {{v, 2_z}});
    RCP<const MultivariateIntPolynomialExpr> p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 5_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});

    RCP<const MultivariateIntPolynomialExpr> q1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 7_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, -3_z}, {{0, 1}, -1_z}, {{1, 0}, -1_z}});
    RCP<const MultivariateIntPolynomialExpr> q3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 3_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> q4 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 10_z}, {{0, 1}, 2_z}, {{1, 0}, 2_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}
/*
TEST_CASE("Testing Precedence of MultivariateIntPolynomialExpr",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    Precedence Prec;
    RCP<const MultivariateIntPolynomialExpr> p1
        = MultivariateIntPolynomialExpr::create({x, y}, {{{0, 0}, 0_z}});
    RCP<const MultivariateIntPolynomialExpr> p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 0}, 2_z}, {{0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> p3
        = MultivariateIntPolynomialExpr::create({x, y}, {{{0, 0}, 5_z}});
    RCP<const MultivariateIntPolynomialExpr> p4
        = MultivariateIntPolynomialExpr::create({x, y}, {{{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomialExpr> p5
        = MultivariateIntPolynomialExpr::create({x, y}, {{{1, 1}, 4_z}});
    RCP<const MultivariateIntPolynomialExpr> p6
        = MultivariateIntPolynomialExpr::create({x, y}, {{{2, 0}, 1_z}});
    REQUIRE(Prec.getPrecedence(p1) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p2) == PrecedenceEnum::Add);
    REQUIRE(Prec.getPrecedence(p3) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p4) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p5) == PrecedenceEnum::Mul);
    REQUIRE(Prec.getPrecedence(p6) == PrecedenceEnum::Pow);
}*/

TEST_CASE("Testing MultivariateIntPolynomialExpr::get_args()",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomialExpr> p = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, 1_z}, {{1, 1, 1}, 2_z}, {{0, 0, 2}, 1_z}});
    REQUIRE(
        vec_basic_eq_perm(p->get_args(), {mul(integer(2), mul(x, mul(y, z))),
                                          pow(z, integer(2)), one}));
}

TEST_CASE("Constructing MultivariatePolynomialExpr", "[MultivariatePolynomialExpr]")
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

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomialExpr> pprime = MultivariatePolynomialExpr::create(
        {y, x},
        {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 0}, comp1}, {{0, 0}, comp2}, {{2, 2}, comp3}, {{3, 4}, comp4}});
    RCP<const MultivariatePolynomialExpr> p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});
    RCP<const MultivariatePolynomialExpr> p4
        = MultivariatePolynomialExpr::create(s, {{v, Expression(0)}});
    RCP<const MultivariatePolynomialExpr> p5
        = MultivariatePolynomialExpr::create(s, {{v, comp1}});
    RCP<const MultivariatePolynomialExpr> p6
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, comp1},
                                                  {{0, -1}, comp2},
                                                  {{-2, 2}, comp3},
                                                  {{-3, -3}, comp4}});

    REQUIRE(vec_basic_eq_perm(
        p1->get_args(), {mul(integer(2), mul(pow(x, integer(2)), y)),
                         mul(negB.get_basic(), mul(x, pow(y, integer(2)))),
                         mul(symbol("a"), mul(x, y)), mul(integer(-3), y)}));
    REQUIRE(
        vec_basic_eq_perm(pprime->get_args(),
                          {mul(negB.get_basic(), mul(pow(x, integer(2)), y)),
                           mul(integer(2), mul(x, pow(y, integer(2)))),
                           mul(symbol("a"), mul(x, y)), mul(integer(-3), x)}));
    REQUIRE(vec_basic_eq_perm(
        p2->get_args(),
        {mul(comp4.get_basic(), mul(pow(x, integer(3)), pow(y, integer(4)))),
         mul(comp3.get_basic(), mul(pow(x, integer(2)), pow(y, integer(2)))),
         mul(comp1.get_basic(), x), comp2.get_basic()}));
    REQUIRE(vec_basic_eq_perm(p3->get_args(), s));
    REQUIRE(vec_basic_eq_perm(p4->get_args(), s));
    REQUIRE(vec_basic_eq_perm(p5->get_args(), {comp1.get_basic()}));
    REQUIRE(vec_basic_eq_perm(
        p6->get_args(),
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
    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 0}, sum}, {{1, -1}, Expression(a)}, {{0, 2}, sum}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 0}, sum}, {{1, -1}, Expression(a) * -1}, {{0, 2}, sum}});
    RCP<const MultivariatePolynomialExpr> p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{2, 0}, sum + sum}, {{0, 2}, sum + sum}});
    RCP<const MultivariatePolynomialExpr> p4 = MultivariatePolynomialExpr::create(
        {a, b}, {{{2, 0}, sum * 2}, {{0, 2}, sum * 2}});
    vec_basic s;
    vec_int v;
    RCP<const MultivariatePolynomialExpr> p5
        = MultivariatePolynomialExpr::create(s, {{v, Expression(0)}});
    RCP<const MultivariatePolynomialExpr> p6
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}});
    RCP<const MultivariatePolynomialExpr> p7
        = MultivariatePolynomialExpr::create(s, {{v, sum}});
    RCP<const MultivariatePolynomialExpr> p8
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, sum}});

    REQUIRE(p1->__eq__(*p1));
    REQUIRE(!(p2->__eq__(*p1)));
    REQUIRE(p3->__eq__(*add_mult_poly(*p1, *p2)));
    REQUIRE(p5->__eq__(*p6));
    REQUIRE(p7->__eq__(*p8));
    REQUIRE(!p6->__eq__(*p7));

    // Only requre that the same polynomial hash to the same value and that
    // different polynomials
    // hash to different values
    // Don't want to require a polynomial to have a particular hash in case
    // someone comes up with
    // a better hash function
    REQUIRE(p3->__hash__() == add_mult_poly(*p1, *p2)->__hash__());
    REQUIRE(p1->__hash__() != p2->__hash__());
    REQUIRE(p3->__hash__() != p4->__hash__());

    // Same for compare.
    REQUIRE(0 == p3->compare(*add_mult_poly(*p1, *p2)));
    REQUIRE(0 != p1->compare(*p2));
    REQUIRE(0 != p3->compare(*p4));
}

TEST_CASE("Testing MultivariatePolynomialExpr::eval", "[MultivariatePolynomialExpr]")
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

    RCP<const MultivariatePolynomialExpr> p
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
    REQUIRE(p->eval(m1) == expr4);
    REQUIRE(p->eval(m2)
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
    RCP<const MultivariatePolynomialExpr> p
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 1}, expr1},
                                                  {{1, 2}, expr2},
                                                  {{2, 0}, expr3},
                                                  {{0, 2}, expr4},
                                                  {{1, 0}, expr1},
                                                  {{0, 1}, expr2},
                                                  {{0, 0}, expr3},
                                                  {{-1, 0}, expr4},
                                                  {{0, -2}, expr1}});

    RCP<const MultivariatePolynomialExpr> q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, expr1 * 2},
                                                  {{0, 2}, expr2},
                                                  {{1, 0}, expr3 * 2},
                                                  {{0, 0}, expr1},
                                                  {{-2, 0}, expr4 * -1}});
    RCP<const MultivariatePolynomialExpr> q2
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 0}, expr1},
                                                  {{1, 1}, expr2 * 2},
                                                  {{0, 1}, expr4 * 2},
                                                  {{0, 0}, expr2},
                                                  {{0, -3}, expr1 * -2}});
    RCP<const MultivariatePolynomialExpr> q3
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}});

    REQUIRE(eq(*(p->diff(x)), *q1));
    REQUIRE(eq(*(p->diff(y)), *q2));
    REQUIRE(eq(*(p->diff(z)), *q3));
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
    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, Expression(1)},
                    {{1, 1, 1}, Expression(2)},
                    {{0, 0, 2}, Expression(1)}});
    RCP<const MultivariatePolynomialExpr> p2
        = MultivariatePolynomialExpr::create({x, y, z}, {{{0, 0, 0}, expr1},
                                                     {{1, 1, 1}, expr2},
                                                     {{0, 0, 2}, expr3},
                                                     {{0, 2, 0}, expr4},
                                                     {{-1, -1, -1}, expr2},
                                                     {{0, 0, -2}, expr3},
                                                     {{0, -2, 0}, expr4}});
    REQUIRE(
        vec_basic_eq_perm(p1->get_args(), {mul(integer(2), mul(x, mul(y, z))),
                                           pow(z, integer(2)), one}));
    REQUIRE(vec_basic_eq_perm(
        p2->get_args(),
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

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 1}, a}, {{1, -2}, negB}, {{-2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 0}, comp1}, {{0, 0}, comp2}, {{2, 2}, comp3}, {{3, 4}, comp4}});
    RCP<const MultivariatePolynomialExpr> p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});

    RCP<const MultivariatePolynomialExpr> q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, a * -1},
                                                  {{1, -2}, negB * -1},
                                                  {{-2, 1}, num1 * -1},
                                                  {{0, 1}, negNum * -1}});
    RCP<const MultivariatePolynomialExpr> q2
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, comp1 * -1},
                                                  {{0, 0}, comp2 * -1},
                                                  {{2, 2}, comp3 * -1},
                                                  {{3, 4}, comp4 * -1}});
    RCP<const MultivariatePolynomialExpr> q3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});

    REQUIRE(eq(*p1->neg(), *q1));
    REQUIRE(eq(*p2->neg(), *q2));
    REQUIRE(eq(*p3->neg(), *q3));
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

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{1, 1}, a}, {{1, 0}, negB}, {{2, -1}, num1}, {{0, -1}, negNum}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 0}, comp1}, {{0, 0}, comp4}, {{0, -1}, comp4}});
    RCP<const MultivariatePolynomialExpr> p3 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, Expression(integer(0))}});

    RCP<const MultivariatePolynomialExpr> q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, a},
                                                  {{1, 0}, negB + comp1},
                                                  {{2, -1}, num1},
                                                  {{0, 0}, comp4},
                                                  {{0, -1}, comp4 + negNum}});
    RCP<const MultivariatePolynomialExpr> q2
        = MultivariatePolynomialExpr::create({x, y}, {{{2, -1}, num1},
                                                  {{1, 1}, a},
                                                  {{1, 0}, (-1 * comp1) + negB},
                                                  {{0, -1}, negNum - comp4},
                                                  {{0, 0}, comp4 * -1}});
    RCP<const MultivariatePolynomialExpr> q22
        = MultivariatePolynomialExpr::create({x, y}, {{{2, -1}, -1 * num1},
                                                  {{1, 1}, -1 * a},
                                                  {{1, 0}, comp1 - negB},
                                                  {{0, -1}, comp4 - negNum},
                                                  {{0, 0}, comp4}});
    RCP<const MultivariatePolynomialExpr> q3 = MultivariatePolynomialExpr::create(
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

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*add_mult_poly(*p1, *p3), *p1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q22));
    REQUIRE(eq(*sub_mult_poly(*p1, *p3), *p1));

    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q3));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p3), *p3));
    REQUIRE(eq(*mul_mult_poly(*p3, *p1), *p3));
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

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a}, {{-1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {m, n}, {{{1, 0}, comp1}, {{2, -1}, comp4}});

    RCP<const MultivariatePolynomialExpr> q1 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{0, 0, 1, 1}, a},
                       {{0, 0, -1, 0}, negB},
                       {{0, 0, 0, 0}, negNum},
                       {{1, 0, 0, 0}, comp1},
                       {{2, -1, 0, 0}, comp4}});
    RCP<const MultivariatePolynomialExpr> q2 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{0, 0, 1, 1}, a},
                       {{0, 0, -1, 0}, negB},
                       {{0, 0, 0, 0}, negNum},
                       {{1, 0, 0, 0}, comp1 * -1},
                       {{2, -1, 0, 0}, comp4 * -1}});
    RCP<const MultivariatePolynomialExpr> q3 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{0, 0, 1, 1}, a * -1},
                       {{0, 0, -1, 0}, negB * -1},
                       {{0, 0, 0, 0}, negNum * -1},
                       {{1, 0, 0, 0}, comp1},
                       {{2, -1, 0, 0}, comp4}});
    RCP<const MultivariatePolynomialExpr> q4 = MultivariatePolynomialExpr::create(
        {m, n, x, y}, {{{2, -1, 1, 1}, a * comp4},
                       {{2, -1, -1, 0}, negB * comp4},
                       {{2, -1, 0, 0}, negNum * comp4},
                       {{1, 0, 1, 1}, a * comp1},
                       {{1, 0, -1, 0}, negB * comp1},
                       {{1, 0, 0, 0}, negNum * comp1}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
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

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, -1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {y, z}, {{{1, 0}, comp1}, {{-2, 1}, comp4}});

    RCP<const MultivariatePolynomialExpr> q1
        = MultivariatePolynomialExpr::create({x, y, z}, {{{1, -1, 0}, a},
                                                     {{1, 0, 0}, negB},
                                                     {{0, 0, 0}, negNum},
                                                     {{0, 1, 0}, comp1},
                                                     {{0, -2, 1}, comp4}});
    RCP<const MultivariatePolynomialExpr> q2
        = MultivariatePolynomialExpr::create({x, y, z}, {{{1, -1, 0}, a},
                                                     {{1, 0, 0}, negB},
                                                     {{0, 0, 0}, negNum},
                                                     {{0, 1, 0}, comp1 * -1},
                                                     {{0, -2, 1}, comp4 * -1}});
    RCP<const MultivariatePolynomialExpr> q3
        = MultivariatePolynomialExpr::create({x, y, z}, {{{1, -1, 0}, a * -1},
                                                     {{1, 0, 0}, negB * -1},
                                                     {{0, 0, 0}, negNum * -1},
                                                     {{0, 1, 0}, comp1},
                                                     {{0, -2, 1}, comp4}});
    RCP<const MultivariatePolynomialExpr> q4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 0, 0}, a * comp1},
                    {{1, -3, 1}, a * comp4},
                    {{1, 1, 0}, negB * comp1},
                    {{1, -2, 1}, negB * comp4},
                    {{0, 1, 0}, negNum * comp1},
                    {{0, -2, 1}, comp4 * negNum}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "UnivariatePolynomials with the same variable",
          "[MultivariatePolynomialExpr][UnivariatePolynomial]")
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
    RCP<const UnivariatePolynomial> p1 = univariate_polynomial(
        x, UnivariateExprPolynomial({{1, expr1}, {2, expr2}, {0, expr3}}));
    RCP<const UnivariatePolynomial> p2 = univariate_polynomial(
        x, UnivariateExprPolynomial({{0, expr4}, {1, expr1}}));

    RCP<const MultivariatePolynomialExpr> q1 = MultivariatePolynomialExpr::create(
        {x}, {{{1}, expr1 + expr1}, {{0}, expr4 + expr3}, {{2}, expr2}});
    RCP<const MultivariatePolynomialExpr> q2 = MultivariatePolynomialExpr::create(
        {x}, {{{0}, expr3 - expr4}, {{1}, expr1 - expr1}, {{2}, expr2}});
    RCP<const MultivariatePolynomialExpr> q3 = MultivariatePolynomialExpr::create(
        {x}, {{{0}, expr4 - expr3}, {{1}, expr1 - expr1}, {{2}, expr2 * -1}});
    RCP<const MultivariatePolynomialExpr> q4 = MultivariatePolynomialExpr::create(
        {x}, {{{3}, expr2 * expr1},
              {{2}, expr2 * expr4 + expr1 * expr1},
              {{1}, expr1 * expr4 + expr1 * expr3},
              {{0}, expr3 * expr4}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "UnivariatePolynomials with the different variables",
          "[MultivariatePolynomialExpr][UnivariatePolynomial]")
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
    RCP<const UnivariatePolynomial> p1 = univariate_polynomial(
        x, UnivariateExprPolynomial({{1, expr1}, {2, expr2}, {0, expr3}}));
    RCP<const UnivariatePolynomial> p2 = univariate_polynomial(
        y, UnivariateExprPolynomial({{0, expr4}, {1, expr5}}));

    RCP<const MultivariatePolynomialExpr> q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1},
                                                  {{2, 0}, expr2},
                                                  {{0, 0}, expr3 + expr4},
                                                  {{0, 0}, expr4},
                                                  {{0, 1}, expr5}});
    RCP<const MultivariatePolynomialExpr> q2
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1},
                                                  {{2, 0}, expr2},
                                                  {{0, 0}, expr3 - expr4},
                                                  {{0, 1}, expr5 * -1}});
    RCP<const MultivariatePolynomialExpr> q3
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 0}, expr1 * -1},
                                                  {{2, 0}, expr2 * -1},
                                                  {{0, 0}, expr4 - expr3},
                                                  {{0, 1}, expr5}});
    RCP<const MultivariatePolynomialExpr> q4
        = MultivariatePolynomialExpr::create({x, y}, {{{2, 1}, expr2 * expr5},
                                                  {{2, 0}, expr2 * expr4},
                                                  {{1, 1}, expr1 * expr5},
                                                  {{1, 0}, expr1 * expr4},
                                                  {{0, 1}, expr3 * expr5},
                                                  {{0, 0}, expr3 * expr4}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExprs with a UnivariatePolynomial whose variable "
          "are in the variable set",
          "[MultivariatePolynomialExpr][UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const UnivariatePolynomial> p2 = univariate_polynomial(
        y, UnivariateExprPolynomial(
               {{0, comp4}, {1, Expression(integer(2))}, {2, comp1}}));

    RCP<const MultivariatePolynomialExpr> q1
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, a},
                                                  {{0, 2}, comp1},
                                                  {{1, 0}, negB},
                                                  {{0, 1}, Expression(2)},
                                                  {{0, 0}, comp4 + negNum}});
    RCP<const MultivariatePolynomialExpr> q2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a},
                 {{0, 2}, comp1 * -1},
                 {{1, 0}, negB},
                 {{0, 1}, Expression(-2)},
                 {{0, 0}, -1 * comp4 + negNum}});
    RCP<const MultivariatePolynomialExpr> q3
        = MultivariatePolynomialExpr::create({x, y}, {{{1, 1}, a * -1},
                                                  {{0, 2}, comp1},
                                                  {{1, 0}, negB * -1},
                                                  {{0, 1}, Expression(2)},
                                                  {{0, 0}, comp4 - negNum}});
    RCP<const MultivariatePolynomialExpr> q4 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 3}, a * comp1},
                 {{1, 2}, 2 * a + negB * comp1},
                 {{1, 1}, a * comp4 + negB * 2},
                 {{0, 2}, negNum * comp1},
                 {{1, 0}, negB * comp4},
                 {{0, 1}, 2 * negNum},
                 {{0, 0}, negNum * comp4}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomialExprs with a UnivariatePolynomial whose variables "
          "are not in the variable set",
          "[MultivariatePolynomialExpr][UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    RCP<const MultivariatePolynomialExpr> p1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const UnivariatePolynomial> p2 = univariate_polynomial(
        z, UnivariateExprPolynomial(
               {{0, comp4}, {1, Expression(integer(2))}, {2, comp1}}));

    RCP<const MultivariatePolynomialExpr> q1 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 0}, a},
                    {{0, 0, 2}, comp1},
                    {{1, 0, 0}, negB},
                    {{0, 0, 1}, Expression(2)},
                    {{0, 0, 0}, negNum + comp4}});
    RCP<const MultivariatePolynomialExpr> q2 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 0}, a},
                    {{0, 0, 2}, comp1 * -1},
                    {{1, 0, 0}, negB},
                    {{0, 0, 1}, Expression(-2)},
                    {{0, 0, 0}, negNum - comp4}});
    RCP<const MultivariatePolynomialExpr> q3 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 0}, a * -1},
                    {{0, 0, 2}, comp1},
                    {{1, 0, 0}, negB * -1},
                    {{0, 0, 1}, Expression(2)},
                    {{0, 0, 0}, -1 * negNum + comp4}});
    RCP<const MultivariatePolynomialExpr> q4 = MultivariatePolynomialExpr::create(
        {x, y, z}, {{{1, 1, 2}, a * comp1},
                    {{1, 1, 1}, 2 * a},
                    {{1, 0, 2}, negB * comp1},
                    {{1, 1, 0}, a * comp4},
                    {{1, 0, 1}, 2 * negB},
                    {{0, 0, 2}, negNum * comp1},
                    {{1, 0, 0}, negB * comp4},
                    {{0, 0, 1}, 2 * negNum},
                    {{0, 0, 0}, negNum * comp4}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
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
    RCP<const MultivariatePolynomialExpr> p1
        = MultivariatePolynomialExpr::create(s, {{v, expr1}});
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2}, {{0, 1}, expr3}, {{-1, 0}, expr4}});

    RCP<const MultivariatePolynomialExpr> q1 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2 + expr1}, {{0, 1}, expr3}, {{-1, 0}, expr4}});
    RCP<const MultivariatePolynomialExpr> q2 = MultivariatePolynomialExpr::create(
        {x, y}, {{{0, 0}, expr2 - expr1}, {{0, 1}, expr3}, {{-1, 0}, expr4}});
    RCP<const MultivariatePolynomialExpr> q3 = MultivariatePolynomialExpr::create(
        {x, y},
        {{{0, 0}, expr1 - expr2}, {{0, 1}, -1 * expr3}, {{-1, 0}, -1 * expr4}});
    RCP<const MultivariatePolynomialExpr> q4
        = MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, expr2 * expr1},
                                                  {{0, 1}, expr3 * expr1},
                                                  {{-1, 0}, expr4 * expr1}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q3));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q2));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

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
    RCP<const MultivariatePolynomialExpr> p2 = MultivariatePolynomialExpr::create(
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
}
/*
TEST_CASE("Testing equality of MultivariateExprPolynomials with Expressions",
          "[MultivariateExprPolynomial],[Expression]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Integer> two = make_rcp<const Integer>(integer_class(2));
    Expression expr1(mul(a, c));
    MultivariateExprPolynomial p1(
        MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, Expression(0)}}));
    MultivariateExprPolynomial p2(
        MultivariatePolynomialExpr::create({x, y}, {{{0, 0}, expr1}}));
    REQUIRE(p1 == 0);
    REQUIRE(p2 == expr1);
}
*/
>>>>>>> fixed polynomial tests
