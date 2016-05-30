#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/printer.h>
#include <symengine/polys/uintpoly.h>

using SymEngine::Expression;
using SymEngine::UIntPoly;
using SymEngine::uint_poly;
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

using namespace SymEngine::literals;

TEST_CASE("Constructing MultivariateIntPolynomialExpr",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    MultivariateIntPolynomialExpr P
        = MultivariateIntPolynomialExpr::create({x, y}, {{{1, 2}, 1_z},
                                                         {{1, 1}, 2_z},
                                                         {{0, 1}, 2_z},
                                                         {{1, 0}, 3_z},
                                                         {{0, 0}, 0_z}});

    REQUIRE(vec_basic_eq_perm(
        P.get_args(), {mul(x, pow(y, integer(2))), mul(integer(2), mul(x, y)),
                       mul(integer(3), x), mul(integer(2), y)}));

    MultivariateIntPolynomialExpr Pprime
        = MultivariateIntPolynomialExpr::create({y, x}, {{{1, 2}, 1_z},
                                                         {{1, 1}, 2_z},
                                                         {{0, 1}, 2_z},
                                                         {{1, 0}, 3_z},
                                                         {{0, 0}, 0_z}});

    REQUIRE(vec_basic_eq_perm(Pprime.get_args(),
                              {mul(pow(x, integer(2)), y),
                               mul(integer(2), mul(x, y)), mul(integer(2), x),
                               mul(integer(3), y)}));

    MultivariateIntPolynomialExpr P2
        = MultivariateIntPolynomialExpr::create({x, y}, {{{0, 0}, 0_z}});

    vec_basic s;
    vec_uint v;
    MultivariateIntPolynomialExpr P3
        = MultivariateIntPolynomialExpr::create(s, {{v, 0_z}});

    REQUIRE(vec_basic_eq_perm(P2.get_args(), s));
    REQUIRE(vec_basic_eq_perm(P3.get_args(), s));

    MultivariateIntPolynomialExpr P4
        = MultivariateIntPolynomialExpr::create(s, {{v, 5_z}});
    REQUIRE(vec_basic_eq_perm(P4.get_args(), {integer(5)}));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr::__hash__() and compare",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    MultivariateIntPolynomialExpr p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    MultivariateIntPolynomialExpr p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    MultivariateIntPolynomialExpr p3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    MultivariateIntPolynomialExpr p4 = MultivariateIntPolynomialExpr::create(
        {a, b}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});

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

TEST_CASE("Testing MultivariateIntPolynomialExpr::__eq__(const Basic &o)",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    MultivariateIntPolynomialExpr p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    MultivariateIntPolynomialExpr p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    MultivariateIntPolynomialExpr p3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    MultivariateIntPolynomialExpr p4
        = MultivariateIntPolynomialExpr::create({x}, {{{0}, 5_z}});
    MultivariateIntPolynomialExpr p5
        = MultivariateIntPolynomialExpr::create({y}, {{{0}, 5_z}});
    MultivariateIntPolynomialExpr p6
        = MultivariateIntPolynomialExpr::create({x}, {{{0}, 0_z}});
    MultivariateIntPolynomialExpr p7
        = MultivariateIntPolynomialExpr::create({y}, {{{0}, 0_z}});

    REQUIRE(p1.__eq__(p1));
    REQUIRE(!(p2.__eq__(p1)));
    REQUIRE(p3.__eq__(add_mult_poly(p1, p2)));
    REQUIRE(p4.__eq__(p5));
    REQUIRE(p6.__eq__(p7));
    REQUIRE(!p5.__eq__(p6));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr::eval((std::map<RCP<const "
          "Symbol>, integer_class, RCPSymbolCompare> &vals)",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p
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
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> m2
        = {{x, 0_z}, {y, 0_z}, {z, 0_z}};
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> m3
        = {{x, -1_z}, {y, -2_z}, {z, -5_z}};

    REQUIRE(171_z == p.eval(m1));
    REQUIRE(5_z == p.eval(m2));
    REQUIRE(51_z == p.eval(m3));
}

TEST_CASE("Testing MultivariateIntPolynomialExpr negation",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {{{1, 0, 0}, 1_z}, {{0, 1, 0}, -2_z}, {{0, 0, 1}, 3_z}});
    MultivariateIntPolynomialExpr p2 = p.neg();

    MultivariateIntPolynomialExpr q = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {{{1, 0, 0}, -1_z}, {{0, 1, 0}, 2_z}, {{0, 0, 1}, -3_z}});

    REQUIRE(p2.__eq__(q));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with the same set of variables",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, 2_z},
                                                            {{4, 1, 0}, 3_z},
                                                            {{0, 0, 0}, 4_z}});
    MultivariateIntPolynomialExpr p2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 3_z}});

    MultivariateIntPolynomialExpr q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 2_z},
                                                            {{4, 1, 0}, 3_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 7_z}});
    MultivariateIntPolynomialExpr q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{3, 2, 1}, 4_z},
                                                            {{4, 1, 0}, 3_z},
                                                            {{0, 1, 2}, -1_z},
                                                            {{0, 0, 0}, 1_z}});
    MultivariateIntPolynomialExpr q3 = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {
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

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q3));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q3));
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
    MultivariateIntPolynomialExpr p1
        = MultivariateIntPolynomialExpr::create({a, b, c}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, 2_z},
                                                            {{4, 1, 0}, 3_z},
                                                            {{0, 0, 0}, 4_z}});
    MultivariateIntPolynomialExpr p2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 3_z}});

    MultivariateIntPolynomialExpr q1 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, 1_z},
                             {{3, 2, 1, 0, 0, 0}, 2_z},
                             {{4, 1, 0, 0, 0, 0}, 3_z},
                             {{0, 0, 0, 0, 0, 0}, 7_z},
                             {{0, 0, 0, 1, 2, 3}, 1_z},
                             {{0, 0, 0, 3, 2, 1}, -2_z},
                             {{0, 0, 0, 0, 1, 2}, 1_z}});
    MultivariateIntPolynomialExpr q2 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, 1_z},
                             {{3, 2, 1, 0, 0, 0}, 2_z},
                             {{4, 1, 0, 0, 0, 0}, 3_z},
                             {{0, 0, 0, 0, 0, 0}, 1_z},
                             {{0, 0, 0, 1, 2, 3}, -1_z},
                             {{0, 0, 0, 3, 2, 1}, 2_z},
                             {{0, 0, 0, 0, 1, 2}, -1_z}});
    MultivariateIntPolynomialExpr q3 = MultivariateIntPolynomialExpr::create(
        {a, b, c, x, y, z}, {{{1, 2, 3, 0, 0, 0}, -1_z},
                             {{3, 2, 1, 0, 0, 0}, -2_z},
                             {{4, 1, 0, 0, 0, 0}, -3_z},
                             {{0, 0, 0, 0, 0, 0}, -1_z},
                             {{0, 0, 0, 1, 2, 3}, 1_z},
                             {{0, 0, 0, 3, 2, 1}, -2_z},
                             {{0, 0, 0, 0, 1, 2}, 1_z}});
    MultivariateIntPolynomialExpr q4 = MultivariateIntPolynomialExpr::create(
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

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q4));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with an overlapping set of variables",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 2}, 1_z}, {{4, 0}, 3_z}, {{0, 3}, 4_z}});
    MultivariateIntPolynomialExpr p2 = MultivariateIntPolynomialExpr::create(
        {y, z}, {{{2, 1}, -2_z}, {{0, 2}, 1_z}, {{1, 0}, 3_z}});

    MultivariateIntPolynomialExpr q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                            {{4, 0, 0}, 3_z},
                                                            {{0, 3, 0}, 4_z},
                                                            {{0, 2, 1}, -2_z},
                                                            {{0, 0, 2}, 1_z},
                                                            {{0, 1, 0}, 3_z}});

    MultivariateIntPolynomialExpr q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                            {{4, 0, 0}, 3_z},
                                                            {{0, 3, 0}, 4_z},
                                                            {{0, 2, 1}, 2_z},
                                                            {{0, 0, 2}, -1_z},
                                                            {{0, 1, 0}, -3_z}});

    MultivariateIntPolynomialExpr q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, -1_z},
                                                            {{4, 0, 0}, -3_z},
                                                            {{0, 3, 0}, -4_z},
                                                            {{0, 2, 1}, -2_z},
                                                            {{0, 0, 2}, 1_z},
                                                            {{0, 1, 0}, 3_z}});

    MultivariateIntPolynomialExpr q4
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 4, 1}, -2_z},
                                                            {{4, 2, 1}, -6_z},
                                                            {{0, 5, 1}, -8_z},

                                                            {{1, 2, 2}, 1_z},
                                                            {{4, 0, 2}, 3_z},
                                                            {{0, 3, 2}, 4_z},

                                                            {{1, 3, 0}, 3_z},
                                                            {{4, 1, 0}, 9_z},
                                                            {{0, 4, 0}, 12_z}});

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q4));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q4));
}

TEST_CASE("Testing derivative of MultivariateIntPolynomialExpr",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p
        = MultivariateIntPolynomialExpr::create({x, y}, {{{2, 1}, 3_z},
                                                         {{1, 2}, 2_z},
                                                         {{2, 0}, 3_z},
                                                         {{0, 2}, 2_z},
                                                         {{1, 0}, 3_z},
                                                         {{0, 1}, 2_z},
                                                         {{0, 0}, 5_z}});

    MultivariateIntPolynomialExpr q1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 1}, 6_z}, {{0, 2}, 2_z}, {{1, 0}, 6_z}, {{0, 0}, 3_z}});
    MultivariateIntPolynomialExpr q2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 0}, 3_z}, {{1, 1}, 4_z}, {{0, 1}, 4_z}, {{0, 0}, 2_z}});
    MultivariateIntPolynomialExpr q3
        = MultivariateIntPolynomialExpr::create({x, y}, {{{0, 0}, 0_z}});
    REQUIRE(MultivariateIntPolynomialExpr::diff(p, x).__eq__(q1));
    REQUIRE(MultivariateIntPolynomialExpr::diff(p, y).__eq__(q2));
    REQUIRE(MultivariateIntPolynomialExpr::diff(p, z).__eq__(q3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with a UIntPoly whose "
          "variable are in the variable set",
          "[MultivariateIntPolynomialExpr][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 3_z},
                                                            {{2, 0, 0}, 2_z},
                                                            {{1, 0, 0}, 1_z}});
    RCP<const UIntPoly> p2 = uint_poly(x, {{1, 1_z}, {2, 1_z}});
    RCP<const UIntPoly> p3 = uint_poly(y, {{1, 1_z}, {2, 1_z}});

    MultivariateIntPolynomialExpr q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 3_z},
                                                            {{2, 0, 0}, 3_z},
                                                            {{1, 0, 0}, 2_z}});
    MultivariateIntPolynomialExpr q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 3_z},
                                                            {{2, 0, 0}, 1_z}});
    MultivariateIntPolynomialExpr q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, -1_z},
                                                            {{3, 2, 1}, 2_z},
                                                            {{0, 1, 2}, -1_z},
                                                            {{0, 0, 0}, -3_z},
                                                            {{2, 0, 0}, -1_z}});
    MultivariateIntPolynomialExpr q4
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
    MultivariateIntPolynomialExpr q5
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 3}, 1_z},
                                                            {{3, 2, 1}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 0}, 3_z},
                                                            {{2, 0, 0}, 2_z},
                                                            {{1, 0, 0}, 1_z},
                                                            {{0, 1, 0}, 1_z},
                                                            {{0, 2, 0}, 1_z}});

    REQUIRE(add_mult_poly(p1, *p2).__eq__(q1));
    REQUIRE(add_mult_poly(*p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, *p2).__eq__(q4));
    REQUIRE(mul_mult_poly(*p2, p1).__eq__(q4));

    REQUIRE(add_mult_poly(p1, *p3).__eq__(q5));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomialExprs with a UIntPoly whose "
          "variables are not in the variable set",
          "[MultivariateIntPolynomialExpr][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    MultivariateIntPolynomialExpr p1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 2}, 1_z}, {{2, 1}, -2_z}, {{0, 1}, 1_z}, {{0, 0}, 3_z}});
    RCP<const UIntPoly> p2 = uint_poly(z, {{1, 1_z}, {2, 1_z}});

    MultivariateIntPolynomialExpr q1
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                            {{2, 1, 0}, -2_z},
                                                            {{0, 1, 0}, 1_z},
                                                            {{0, 0, 0}, 3_z},
                                                            {{0, 0, 1}, 1_z},
                                                            {{0, 0, 2}, 1_z}});
    MultivariateIntPolynomialExpr q2
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, 1_z},
                                                            {{2, 1, 0}, -2_z},
                                                            {{0, 1, 0}, 1_z},
                                                            {{0, 0, 0}, 3_z},
                                                            {{0, 0, 1}, -1_z},
                                                            {{0, 0, 2}, -1_z}});
    MultivariateIntPolynomialExpr q3
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 0}, -1_z},
                                                            {{2, 1, 0}, 2_z},
                                                            {{0, 1, 0}, -1_z},
                                                            {{0, 0, 0}, -3_z},
                                                            {{0, 0, 1}, 1_z},
                                                            {{0, 0, 2}, 1_z}});
    MultivariateIntPolynomialExpr q4
        = MultivariateIntPolynomialExpr::create({x, y, z}, {{{1, 2, 1}, 1_z},
                                                            {{2, 1, 1}, -2_z},
                                                            {{0, 1, 1}, 1_z},
                                                            {{0, 0, 1}, 3_z},

                                                            {{1, 2, 2}, 1_z},
                                                            {{2, 1, 2}, -2_z},
                                                            {{0, 1, 2}, 1_z},
                                                            {{0, 0, 2}, 3_z}});

    REQUIRE(add_mult_poly(p1, *p2).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, *p2).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UIntPolys with different variables",
          "[MultivariateIntPolynomialExpr][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UIntPoly> p1 = uint_poly(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UIntPoly> p2 = uint_poly(y, {{0, 1_z}, {1, 1_z}});

    MultivariateIntPolynomialExpr q1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 0}, -1_z}, {{2, 0}, 3_z}, {{0, 0}, 1_z}, {{0, 1}, 1_z}});
    MultivariateIntPolynomialExpr q2 = MultivariateIntPolynomialExpr::create(
        {x, y},
        {{{1, 0}, -1_z}, {{2, 0}, 3_z}, {{0, 0}, -1_z}, {{0, 1}, -1_z}});
    MultivariateIntPolynomialExpr q3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{1, 0}, 1_z}, {{2, 0}, -3_z}, {{0, 0}, 1_z}, {{0, 1}, 1_z}});
    MultivariateIntPolynomialExpr q4 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{2, 1}, 3_z}, {{2, 0}, 3_z}, {{1, 1}, -1_z}, {{1, 0}, -1_z}});

    REQUIRE(add_mult_poly(*p1, *p2).__eq__(q1));
    REQUIRE(sub_mult_poly(*p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, *p1).__eq__(q3));
    REQUIRE(mul_mult_poly(*p1, *p2).__eq__(q4));
    REQUIRE(mul_mult_poly(*p2, *p1).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UIntPolys with the same variables",
          "[MultivariateIntPolynomialExpr][UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPoly> p1 = uint_poly(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UIntPoly> p2 = uint_poly(x, {{0, 1_z}, {1, 1_z}});

    MultivariateIntPolynomialExpr q1
        = MultivariateIntPolynomialExpr::create({x}, {{{0}, 1_z}, {{2}, 3_z}});
    MultivariateIntPolynomialExpr q2 = MultivariateIntPolynomialExpr::create(
        {x}, {{{0}, -1_z}, {{1}, -2_z}, {{2}, 3_z}});
    MultivariateIntPolynomialExpr q3 = MultivariateIntPolynomialExpr::create(
        {x}, {{{0}, 1_z}, {{1}, 2_z}, {{2}, -3_z}});
    MultivariateIntPolynomialExpr q4 = MultivariateIntPolynomialExpr::create(
        {x}, {{{1}, -1_z}, {{2}, 2_z}, {{3}, 3_z}});

    REQUIRE(add_mult_poly(*p1, *p2).__eq__(q1));
    REQUIRE(sub_mult_poly(*p1, *p2).__eq__(q2));
    REQUIRE(sub_mult_poly(*p2, *p1).__eq__(q3));
    REQUIRE(mul_mult_poly(*p1, *p2).__eq__(q4));
}

TEST_CASE("Testing addition, subtraction, and multiplication of "
          "MultivariateIntPolynomialExprs with empty variable sets",
          "[MultivariateIntPolynomialExpr]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    vec_basic s;
    vec_uint v;
    MultivariateIntPolynomialExpr p1
        = MultivariateIntPolynomialExpr::create(s, {{v, 2_z}});
    MultivariateIntPolynomialExpr p2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 5_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});

    MultivariateIntPolynomialExpr q1 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 7_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    MultivariateIntPolynomialExpr q2 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, -3_z}, {{0, 1}, -1_z}, {{1, 0}, -1_z}});
    MultivariateIntPolynomialExpr q3 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 3_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    MultivariateIntPolynomialExpr q4 = MultivariateIntPolynomialExpr::create(
        {x, y}, {{{0, 0}, 10_z}, {{0, 1}, 2_z}, {{1, 0}, 2_z}});

    REQUIRE(add_mult_poly(p1, p2).__eq__(q1));
    REQUIRE(add_mult_poly(p2, p1).__eq__(q1));
    REQUIRE(sub_mult_poly(p1, p2).__eq__(q2));
    REQUIRE(sub_mult_poly(p2, p1).__eq__(q3));
    REQUIRE(mul_mult_poly(p1, p2).__eq__(q4));
    REQUIRE(mul_mult_poly(p2, p1).__eq__(q4));
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
    RCP<const MultivariateIntPolynomialExpr> p2 =
MultivariateIntPolynomialExpr::create(
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
    MultivariateIntPolynomialExpr p = MultivariateIntPolynomialExpr::create(
        {x, y, z}, {{{0, 0, 0}, 1_z}, {{1, 1, 1}, 2_z}, {{0, 0, 2}, 1_z}});
    REQUIRE(vec_basic_eq_perm(p.get_args(), {mul(integer(2), mul(x, mul(y, z))),
                                             pow(z, integer(2)), one}));
}
