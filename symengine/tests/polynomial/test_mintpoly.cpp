#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uexprpoly.h>
#include <symengine/polynomial_multivariate.h>
#include <symengine/pow.h>
#include <symengine/printer.h>

using SymEngine::Expression;
using SymEngine::UIntPoly;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;
using SymEngine::MultivariateIntPolynomial;
using SymEngine::MultivariatePolynomial;
using SymEngine::Integer;
using SymEngine::Precedence;
using SymEngine::PrecedenceEnum;
using SymEngine::vec_uint;
using SymEngine::vec_int;
using SymEngine::vec_basic;
using SymEngine::vec_uint;
using SymEngine::RCPBasicKeyLess;

using namespace SymEngine::literals;

TEST_CASE("Constructing MultivariateIntPolynomial",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const MultivariateIntPolynomial> P
        = MultivariateIntPolynomial::create({x, y}, {{{1, 2}, 1_z},
                                                     {{1, 1}, 2_z},
                                                     {{0, 1}, 2_z},
                                                     {{1, 0}, 3_z},
                                                     {{0, 0}, 0_z}});

    REQUIRE(vec_basic_eq_perm(
        P->get_args(), {mul(x, pow(y, integer(2))), mul(integer(2), mul(x, y)),
                        mul(integer(3), x), mul(integer(2), y)}));

    RCP<const MultivariateIntPolynomial> Pprime
        = MultivariateIntPolynomial::create({y, x}, {{{1, 2}, 1_z},
                                                     {{1, 1}, 2_z},
                                                     {{0, 1}, 2_z},
                                                     {{1, 0}, 3_z},
                                                     {{0, 0}, 0_z}});

    REQUIRE(vec_basic_eq_perm(Pprime->get_args(),
                              {mul(pow(x, integer(2)), y),
                               mul(integer(2), mul(x, y)), mul(integer(2), x),
                               mul(integer(3), y)}));

    RCP<const MultivariateIntPolynomial> P2
        = MultivariateIntPolynomial::create({x, y}, {{{0, 0}, 0_z}});

    vec_basic s;
    vec_uint v;
    RCP<const MultivariateIntPolynomial> P3
        = MultivariateIntPolynomial::create(s, {{v, 0_z}});

    REQUIRE(vec_basic_eq_perm(P2->get_args(), s));
    REQUIRE(vec_basic_eq_perm(P3->get_args(), s));

    RCP<const MultivariateIntPolynomial> P4
        = MultivariateIntPolynomial::create(s, {{v, 5_z}});
    REQUIRE(vec_basic_eq_perm(P4->get_args(), {integer(5)}));
}

TEST_CASE("Testing MultivariateIntPolynomial::__hash__() and compare",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const MultivariateIntPolynomial> p1

        = MultivariateIntPolynomial::create(
            {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p3 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    RCP<const MultivariateIntPolynomial> p4 = MultivariateIntPolynomial::create(
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

TEST_CASE("Testing MultivariateIntPolynomial::__eq__(const Basic &o)",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p3 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    RCP<const MultivariateIntPolynomial> p4
        = MultivariateIntPolynomial::create({x}, {{{0}, 5_z}});
    RCP<const MultivariateIntPolynomial> p5
        = MultivariateIntPolynomial::create({y}, {{{0}, 5_z}});
    RCP<const MultivariateIntPolynomial> p6
        = MultivariateIntPolynomial::create({x}, {{{0}, 0_z}});
    RCP<const MultivariateIntPolynomial> p7
        = MultivariateIntPolynomial::create({y}, {{{0}, 0_z}});

    REQUIRE(p1->__eq__(*p1));
    REQUIRE(!(p2->__eq__(*p1)));
    REQUIRE(p3->__eq__(*add_mult_poly(*p1, *p2)));
    REQUIRE(p4->__eq__(*p5));
    REQUIRE(p6->__eq__(*p7));
    REQUIRE(!p5->__eq__(*p6));
}

TEST_CASE("Testing MultivariateIntPolynomial::eval((std::map<RCP<const "
          "Symbol>, integer_class, RCPSymbolCompare> &vals)",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p
        = MultivariateIntPolynomial::create({x, y, z}, {{{2, 0, 0}, 1_z},
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
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> m2
        = {{x, 0_z}, {y, 0_z}, {z, 0_z}};
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> m3
        = {{x, -1_z}, {y, -2_z}, {z, -5_z}};

    REQUIRE(171_z == p->eval(m1));
    REQUIRE(5_z == p->eval(m2));
    REQUIRE(51_z == p->eval(m3));
}

TEST_CASE("Testing MultivariateIntPolynomial negation",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p = MultivariateIntPolynomial::create(
        {x, y, z}, {{{1, 0, 0}, 1_z}, {{0, 1, 0}, -2_z}, {{0, 0, 1}, 3_z}});
    RCP<const MultivariateIntPolynomial> p2 = p->neg();

    RCP<const MultivariateIntPolynomial> q = MultivariateIntPolynomial::create(
        {x, y, z}, {{{1, 0, 0}, -1_z}, {{0, 1, 0}, 2_z}, {{0, 0, 1}, -3_z}});

    REQUIRE(p2->__eq__(*q));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomials with the same set of variables",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, 2_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 0, 0}, 4_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z}});

    RCP<const MultivariateIntPolynomial> q1
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 2_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 7_z}});
    RCP<const MultivariateIntPolynomial> q2
        = MultivariateIntPolynomial::create({x, y, z}, {{{3, 2, 1}, 4_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 1, 2}, -1_z},
                                                        {{0, 0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> q3
        = MultivariateIntPolynomial::create({x, y, z}, {
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
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> c = symbol("c");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::create({a, b, c}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, 2_z},
                                                        {{4, 1, 0}, 3_z},
                                                        {{0, 0, 0}, 4_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z}});

    RCP<const MultivariateIntPolynomial> q1 = MultivariateIntPolynomial::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, 1_z},
                             {{3, 2, 1, 0, 0, 0}, 2_z},
                             {{4, 1, 0, 0, 0, 0}, 3_z},
                             {{0, 0, 0, 0, 0, 0}, 7_z},
                             {{0, 0, 0, 1, 2, 3}, 1_z},
                             {{0, 0, 0, 3, 2, 1}, -2_z},
                             {{0, 0, 0, 0, 1, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> q2 = MultivariateIntPolynomial::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, 1_z},
                             {{3, 2, 1, 0, 0, 0}, 2_z},
                             {{4, 1, 0, 0, 0, 0}, 3_z},
                             {{0, 0, 0, 0, 0, 0}, 1_z},
                             {{0, 0, 0, 1, 2, 3}, -1_z},
                             {{0, 0, 0, 3, 2, 1}, 2_z},
                             {{0, 0, 0, 0, 1, 2}, -1_z}});
    RCP<const MultivariateIntPolynomial> q3 = MultivariateIntPolynomial::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, -1_z},
                             {{3, 2, 1, 0, 0, 0}, -2_z},
                             {{4, 1, 0, 0, 0, 0}, -3_z},
                             {{0, 0, 0, 0, 0, 0}, -1_z},
                             {{0, 0, 0, 1, 2, 3}, 1_z},
                             {{0, 0, 0, 3, 2, 1}, -2_z},
                             {{0, 0, 0, 0, 1, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> q4 = MultivariateIntPolynomial::create(
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
          "MultivariateIntPolynomials with an overlapping set of variables",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::create(
        {x, y}, {{{1, 2}, 1_z}, {{4, 0}, 3_z}, {{0, 3}, 4_z}});
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::create(
        {y, z}, {{{2, 1}, -2_z}, {{0, 2}, 1_z}, {{1, 0}, 3_z}});

    RCP<const MultivariateIntPolynomial> q1
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{4, 0, 0}, 3_z},
                                                        {{0, 3, 0}, 4_z},
                                                        {{0, 2, 1}, -2_z},
                                                        {{0, 0, 2}, 1_z},
                                                        {{0, 1, 0}, 3_z}});

    RCP<const MultivariateIntPolynomial> q2
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{4, 0, 0}, 3_z},
                                                        {{0, 3, 0}, 4_z},
                                                        {{0, 2, 1}, 2_z},
                                                        {{0, 0, 2}, -1_z},
                                                        {{0, 1, 0}, -3_z}});

    RCP<const MultivariateIntPolynomial> q3
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 0}, -1_z},
                                                        {{4, 0, 0}, -3_z},
                                                        {{0, 3, 0}, -4_z},
                                                        {{0, 2, 1}, -2_z},
                                                        {{0, 0, 2}, 1_z},
                                                        {{0, 1, 0}, 3_z}});

    RCP<const MultivariateIntPolynomial> q4
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 4, 1}, -2_z},
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

TEST_CASE("Testing derivative of MultivariateIntPolynomial",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p
        = MultivariateIntPolynomial::create({x, y}, {{{2, 1}, 3_z},
                                                     {{1, 2}, 2_z},
                                                     {{2, 0}, 3_z},
                                                     {{0, 2}, 2_z},
                                                     {{1, 0}, 3_z},
                                                     {{0, 1}, 2_z},
                                                     {{0, 0}, 5_z}});

    RCP<const MultivariateIntPolynomial> q1 = MultivariateIntPolynomial::create(
        {x, y}, {{{1, 1}, 6_z}, {{0, 2}, 2_z}, {{1, 0}, 6_z}, {{0, 0}, 3_z}});
    RCP<const MultivariateIntPolynomial> q2 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 0}, 3_z}, {{1, 1}, 4_z}, {{0, 1}, 4_z}, {{0, 0}, 2_z}});
    RCP<const MultivariateIntPolynomial> q3
        = MultivariateIntPolynomial::create({x, y}, {{{0, 0}, 0_z}});
    REQUIRE(eq(*p->diff(x), *q1));
    REQUIRE(eq(*p->diff(y), *q2));
    REQUIRE(eq(*p->diff(z), *q3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomials with a UIntPoly whose "
          "variable are in the variable set",
          "[MultivariateIntPolynomial][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 2_z},
                                                        {{1, 0, 0}, 1_z}});
    RCP<const UIntPoly> p2 = UIntPoly::from_dict(x, {{1, 1_z}, {2, 1_z}});
    RCP<const UIntPoly> p3 = UIntPoly::from_dict(y, {{1, 1_z}, {2, 1_z}});

    RCP<const MultivariateIntPolynomial> q1
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 3_z},
                                                        {{1, 0, 0}, 2_z}});
    RCP<const MultivariateIntPolynomial> q2
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                        {{3, 2, 1}, -2_z},
                                                        {{0, 1, 2}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{2, 0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> q3
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, -1_z},
                                                        {{3, 2, 1}, 2_z},
                                                        {{0, 1, 2}, -1_z},
                                                        {{0, 0, 0}, -3_z},
                                                        {{2, 0, 0}, -1_z}});
    RCP<const MultivariateIntPolynomial> q4
        = MultivariateIntPolynomial::create({x, y, z}, {{{2, 2, 3}, 1_z},
                                                        {{4, 2, 1}, -2_z},
                                                        {{1, 1, 2}, 1_z},
                                                        {{1, 0, 0}, 3_z},
                                                        {{3, 0, 0}, 3_z},

                                                        {{3, 2, 3}, 1_z},
                                                        {{5, 2, 1}, -2_z},
                                                        {{2, 1, 2}, 1_z},
                                                        {{2, 0, 0}, 4_z},
                                                        {{4, 0, 0}, 2_z}});
    RCP<const MultivariateIntPolynomial> q5
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 3}, 1_z},
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
          "MultivariateIntPolynomials with a UIntPoly whose "
          "variables are not in the variable set",
          "[MultivariateIntPolynomial][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1 = MultivariateIntPolynomial::create(
        {x, y}, {{{1, 2}, 1_z}, {{2, 1}, -2_z}, {{0, 1}, 1_z}, {{0, 0}, 3_z}});
    RCP<const UIntPoly> p2 = UIntPoly::from_dict(z, {{1, 1_z}, {2, 1_z}});

    RCP<const MultivariateIntPolynomial> q1
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{2, 1, 0}, -2_z},
                                                        {{0, 1, 0}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{0, 0, 1}, 1_z},
                                                        {{0, 0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> q2
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                        {{2, 1, 0}, -2_z},
                                                        {{0, 1, 0}, 1_z},
                                                        {{0, 0, 0}, 3_z},
                                                        {{0, 0, 1}, -1_z},
                                                        {{0, 0, 2}, -1_z}});
    RCP<const MultivariateIntPolynomial> q3
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 0}, -1_z},
                                                        {{2, 1, 0}, 2_z},
                                                        {{0, 1, 0}, -1_z},
                                                        {{0, 0, 0}, -3_z},
                                                        {{0, 0, 1}, 1_z},
                                                        {{0, 0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> q4
        = MultivariateIntPolynomial::create({x, y, z}, {{{1, 2, 1}, 1_z},
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
          "UIntPolys with different variables",
          "[MultivariateIntPolynomial][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPoly> p1 = UIntPoly::from_dict(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UIntPoly> p2 = UIntPoly::from_dict(y, {{0, 1_z}, {1, 1_z}});

    RCP<const MultivariateIntPolynomial> q1 = MultivariateIntPolynomial::create(
        {x, y}, {{{1, 0}, -1_z}, {{2, 0}, 3_z}, {{0, 0}, 1_z}, {{0, 1}, 1_z}});
    RCP<const MultivariateIntPolynomial> q2 = MultivariateIntPolynomial::create(
        {x, y},
        {{{1, 0}, -1_z}, {{2, 0}, 3_z}, {{0, 0}, -1_z}, {{0, 1}, -1_z}});
    RCP<const MultivariateIntPolynomial> q3 = MultivariateIntPolynomial::create(
        {x, y}, {{{1, 0}, 1_z}, {{2, 0}, -3_z}, {{0, 0}, 1_z}, {{0, 1}, 1_z}});
    RCP<const MultivariateIntPolynomial> q4 = MultivariateIntPolynomial::create(
        {x, y}, {{{2, 1}, 3_z}, {{2, 0}, 3_z}, {{1, 1}, -1_z}, {{1, 0}, -1_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UIntPolys with the same variables",
          "[MultivariateIntPolynomial][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> p1 = UIntPoly::from_dict(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UIntPoly> p2 = UIntPoly::from_dict(x, {{0, 1_z}, {1, 1_z}});

    RCP<const MultivariateIntPolynomial> q1
        = MultivariateIntPolynomial::create({x}, {{{0}, 1_z}, {{2}, 3_z}});
    RCP<const MultivariateIntPolynomial> q2 = MultivariateIntPolynomial::create(
        {x}, {{{0}, -1_z}, {{1}, -2_z}, {{2}, 3_z}});
    RCP<const MultivariateIntPolynomial> q3 = MultivariateIntPolynomial::create(
        {x}, {{{0}, 1_z}, {{1}, 2_z}, {{2}, -3_z}});
    RCP<const MultivariateIntPolynomial> q4 = MultivariateIntPolynomial::create(
        {x}, {{{1}, -1_z}, {{2}, 2_z}, {{3}, 3_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
}

TEST_CASE("Testing addition, subtraction, and multiplication of "
          "MultivariateIntPolynomials with empty variable sets",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    vec_basic s;
    vec_uint v;
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::create(s, {{v, 2_z}});
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::create(
        {x, y}, {{{0, 0}, 5_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});

    RCP<const MultivariateIntPolynomial> q1 = MultivariateIntPolynomial::create(
        {x, y}, {{{0, 0}, 7_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> q2 = MultivariateIntPolynomial::create(
        {x, y}, {{{0, 0}, -3_z}, {{0, 1}, -1_z}, {{1, 0}, -1_z}});
    RCP<const MultivariateIntPolynomial> q3 = MultivariateIntPolynomial::create(
        {x, y}, {{{0, 0}, 3_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> q4 = MultivariateIntPolynomial::create(
        {x, y}, {{{0, 0}, 10_z}, {{0, 1}, 2_z}, {{1, 0}, 2_z}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing Precedence of MultivariateIntPolynomial",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    Precedence Prec;
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::create({x, y}, {{{0, 0}, 0_z}});
    RCP<const MultivariateIntPolynomial> p2 = MultivariateIntPolynomial::create(
        {x, y}, {{{1, 0}, 2_z}, {{0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> p3
        = MultivariateIntPolynomial::create({x, y}, {{{0, 0}, 5_z}});
    RCP<const MultivariateIntPolynomial> p4
        = MultivariateIntPolynomial::create({x, y}, {{{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> p5
        = MultivariateIntPolynomial::create({x, y}, {{{1, 1}, 4_z}});
    RCP<const MultivariateIntPolynomial> p6
        = MultivariateIntPolynomial::create({x, y}, {{{2, 0}, 1_z}});
    REQUIRE(Prec.getPrecedence(p1) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p2) == PrecedenceEnum::Add);
    REQUIRE(Prec.getPrecedence(p3) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p4) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p5) == PrecedenceEnum::Mul);
    REQUIRE(Prec.getPrecedence(p6) == PrecedenceEnum::Pow);
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
        MultivariatePolynomial::create({x, y}, {{{0, 0}, Expression(0)}}));
    MultivariateExprPolynomial p2(
        MultivariatePolynomial::create({x, y}, {{{0, 0}, expr1}}));
    REQUIRE(p1 == 0);
    REQUIRE(p2 == expr1);
}
*/