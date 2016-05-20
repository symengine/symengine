#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/polynomial_multivariate.h>
#include <symengine/pow.h>
#include <symengine/printer.h>

using SymEngine::Expression;
using SymEngine::UnivariateIntPolynomial;
using SymEngine::UnivariatePolynomial;
using SymEngine::univariate_int_polynomial;
using SymEngine::univariate_polynomial;
using SymEngine::UnivariateExprPolynomial;
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
using SymEngine::MultivariateIntPolynomial;
using SymEngine::MultivariatePolynomial;
using SymEngine::RCPSymbolCompare;
using SymEngine::Integer;
using SymEngine::Precedence;
using SymEngine::PrecedenceEnum;
using SymEngine::set_sym;
using SymEngine::vec_sym;
using SymEngine::vec_uint;

using namespace SymEngine::literals;

TEST_CASE("Constructing MultivariateIntPolynomial",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const MultivariateIntPolynomial> P
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{1, 2}, 1_z},
                     {{1, 1}, 2_z},
                     {{0, 1}, 2_z},
                     {{1, 0}, 3_z},
                     {{0, 0}, 0_z}});
    REQUIRE(P->__str__() == "x y**2 + 2*x y + 3*x + 2*y");

    RCP<const MultivariateIntPolynomial> Pprime
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {y, x}, {{{1, 2}, 1_z},
                     {{1, 1}, 2_z},
                     {{0, 1}, 2_z},
                     {{1, 0}, 3_z},
                     {{0, 0}, 0_z}});
    REQUIRE(Pprime->__str__() == "x**2 y + 2*x y + 2*x + 3*y");

    RCP<const MultivariateIntPolynomial> P2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{0, 0}, 0_z}});
    REQUIRE(P2->__str__() == "0");

    vec_sym s;
    vec_uint v;
    RCP<const MultivariateIntPolynomial> P3
        = MultivariateIntPolynomial::multivariate_int_polynomial(s, {{v, 0_z}});
    REQUIRE(P3->__str__() == "0");

    RCP<const MultivariateIntPolynomial> P4
        = MultivariateIntPolynomial::multivariate_int_polynomial(s, {{v, 5_z}});
    REQUIRE(P4->__str__() == "5");
}

TEST_CASE("Testing MultivariateIntPolynomial::__hash__() and compare",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> a = symbol("a");
    RCP<const Symbol> b = symbol("b");
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const MultivariateIntPolynomial> p1

        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p3
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    RCP<const MultivariateIntPolynomial> p4
        = MultivariateIntPolynomial::multivariate_int_polynomial(
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
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 1_z}, {{1, 1}, 1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 1_z}, {{1, 1}, -1_z}, {{0, 2}, 1_z}});
    RCP<const MultivariateIntPolynomial> p3
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 2_z}, {{0, 2}, 2_z}});
    RCP<const MultivariateIntPolynomial> p4
        = MultivariateIntPolynomial::multivariate_int_polynomial({x},
                                                                 {{{0}, 5_z}});
    RCP<const MultivariateIntPolynomial> p5
        = MultivariateIntPolynomial::multivariate_int_polynomial({y},
                                                                 {{{0}, 5_z}});
    RCP<const MultivariateIntPolynomial> p6
        = MultivariateIntPolynomial::multivariate_int_polynomial({x},
                                                                 {{{0}, 0_z}});
    RCP<const MultivariateIntPolynomial> p7
        = MultivariateIntPolynomial::multivariate_int_polynomial({y},
                                                                 {{{0}, 0_z}});

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
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{2, 0, 0}, 1_z},
                        {{0, 2, 0}, 2_z},
                        {{0, 0, 2}, 3_z},
                        {{1, 1, 1}, 4_z},
                        {{1, 1, 0}, 1_z},
                        {{0, 1, 1}, 2_z},
                        {{1, 0, 0}, 1_z},
                        {{0, 1, 0}, 2_z},
                        {{0, 0, 1}, 3_z},
                        {{0, 0, 0}, 5_z}});
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m1
        = {{x, 1_z}, {y, 2_z}, {z, 5_z}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m2
        = {{x, 0_z}, {y, 0_z}, {z, 0_z}};
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> m3
        = {{x, -1_z}, {y, -2_z}, {z, -5_z}};

    REQUIRE(171_z == p->eval(m1));
    REQUIRE(5_z == p->eval(m2));
    REQUIRE(51_z == p->eval(m3));
}

TEST_CASE("Testing MultivariateIntPolynomial neg_mult_poly",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{1, 0, 0}, 1_z}, {{0, 1, 0}, -2_z}, {{0, 0, 1}, 3_z}});
    RCP<const MultivariateIntPolynomial> p2 = neg_mult_poly(*p);
    REQUIRE(p2->__str__() == "- x + 2*y - 3*z");
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomials with the same set of variables",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{1, 2, 3}, 1_z},
                        {{3, 2, 1}, 2_z},
                        {{4, 1, 0}, 3_z},
                        {{0, 0, 0}, 4_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{1, 2, 3}, 1_z},
                        {{3, 2, 1}, -2_z},
                        {{0, 1, 2}, 1_z},
                        {{0, 0, 0}, 3_z}});

    REQUIRE(add_mult_poly(*p1, *p2)->__str__()
            == "2*x y**2 z**3 + 3*x**4 y + y z**2 + 7");
    REQUIRE(add_mult_poly(*p2, *p1)->__str__()
            == "2*x y**2 z**3 + 3*x**4 y + y z**2 + 7");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__()
            == "4*x**3 y**2 z + 3*x**4 y - y z**2 + 1");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__()
            == "- 4*x**6 y**4 z**2 + x**2 y**4 z**6 - 6*x**7 y**3 z + 3*x**5 "
               "y**3 z**3 + 2*x**3 y**3 z**3 + x y**3 z**5 + 3*x**4 y**2 z**2 "
               "- 2*x**3 y**2 z + 7*x y**2 z**3 + 9*x**4 y + 4*y z**2 + 12");
    REQUIRE(mul_mult_poly(*p2, *p1)->__str__()
            == "- 4*x**6 y**4 z**2 + x**2 y**4 z**6 - 6*x**7 y**3 z + 3*x**5 "
               "y**3 z**3 + 2*x**3 y**3 z**3 + x y**3 z**5 + 3*x**4 y**2 z**2 "
               "- 2*x**3 y**2 z + 7*x y**2 z**3 + 9*x**4 y + 4*y z**2 + 12");
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
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {a, b, c}, {{{1, 2, 3}, 1_z},
                        {{3, 2, 1}, 2_z},
                        {{4, 1, 0}, 3_z},
                        {{0, 0, 0}, 4_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{1, 2, 3}, 1_z},
                        {{3, 2, 1}, -2_z},
                        {{0, 1, 2}, 1_z},
                        {{0, 0, 0}, 3_z}});

    REQUIRE(add_mult_poly(*p1, *p2)->__str__()
            == "2*a**3 b**2 c + a b**2 c**3 - 2*x**3 y**2 z + x y**2 z**3 + "
               "3*a**4 b + y z**2 + 7");
    REQUIRE(add_mult_poly(*p2, *p1)->__str__()
            == "2*a**3 b**2 c + a b**2 c**3 - 2*x**3 y**2 z + x y**2 z**3 + "
               "3*a**4 b + y z**2 + 7");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__()
            == "2*a**3 b**2 c + a b**2 c**3 + 2*x**3 y**2 z - x y**2 z**3 + "
               "3*a**4 b - y z**2 + 1");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__()
            == "- 2*a**3 b**2 c - a b**2 c**3 - 2*x**3 y**2 z + x y**2 z**3 - "
               "3*a**4 b + y z**2 - 1");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__()
            == "- 4*a**3 b**2 c x**3 y**2 z + 2*a**3 b**2 c x y**2 z**3 - 2*a "
               "b**2 c**3 x**3 y**2 z + a b**2 c**3 x y**2 z**3 - 6*a**4 b "
               "x**3 y**2 z + 3*a**4 b x y**2 z**3 + 2*a**3 b**2 c y z**2 + a "
               "b**2 c**3 y z**2 + 3*a**4 b y z**2 + 6*a**3 b**2 c + 3*a b**2 "
               "c**3 - 8*x**3 y**2 z + 4*x y**2 z**3 + 9*a**4 b + 4*y z**2 + "
               "12");
    REQUIRE(mul_mult_poly(*p2, *p1)->__str__()
            == "- 4*a**3 b**2 c x**3 y**2 z + 2*a**3 b**2 c x y**2 z**3 - 2*a "
               "b**2 c**3 x**3 y**2 z + a b**2 c**3 x y**2 z**3 - 6*a**4 b "
               "x**3 y**2 z + 3*a**4 b x y**2 z**3 + 2*a**3 b**2 c y z**2 + a "
               "b**2 c**3 y z**2 + 3*a**4 b y z**2 + 6*a**3 b**2 c + 3*a b**2 "
               "c**3 - 8*x**3 y**2 z + 4*x y**2 z**3 + 9*a**4 b + 4*y z**2 + "
               "12");
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomials with an overlapping set of variables",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{1, 2}, 1_z}, {{4, 0}, 3_z}, {{0, 3}, 4_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {y, z}, {{{2, 1}, -2_z}, {{0, 2}, 1_z}, {{1, 0}, 3_z}});

    REQUIRE(add_mult_poly(*p1, *p2)->__str__()
            == "3*x**4 + x y**2 + 4*y**3 - 2*y**2 z + z**2 + 3*y");
    REQUIRE(add_mult_poly(*p2, *p1)->__str__()
            == "3*x**4 + x y**2 + 4*y**3 - 2*y**2 z + z**2 + 3*y");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__()
            == "3*x**4 + x y**2 + 4*y**3 + 2*y**2 z - z**2 - 3*y");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__()
            == "- 3*x**4 - x y**2 - 4*y**3 - 2*y**2 z + z**2 + 3*y");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__()
            == "- 6*x**4 y**2 z + 3*x**4 z**2 - 2*x y**4 z - 8*y**5 z + 9*x**4 "
               "y + x y**2 z**2 + 4*y**3 z**2 + 3*x y**3 + 12*y**4");
    REQUIRE(mul_mult_poly(*p2, *p1)->__str__()
            == "- 6*x**4 y**2 z + 3*x**4 z**2 - 2*x y**4 z - 8*y**5 z + 9*x**4 "
               "y + x y**2 z**2 + 4*y**3 z**2 + 3*x y**3 + 12*y**4");
}

TEST_CASE("Testing derivative of MultivariateIntPolynomial",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 1}, 3_z},
                     {{1, 2}, 2_z},
                     {{2, 0}, 3_z},
                     {{0, 2}, 2_z},
                     {{1, 0}, 3_z},
                     {{0, 1}, 2_z},
                     {{0, 0}, 5_z}});
    REQUIRE(p->diff(x)->__str__() == "6*x y + 2*y**2 + 6*x + 3");
    REQUIRE(p->diff(y)->__str__() == "3*x**2 + 4*x y + 4*y + 2");
    REQUIRE(p->diff(z)->__str__() == "0");
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomials with a UnivariateIntPolynomial whose "
          "variable are in the variable set",
          "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{1, 2, 3}, 1_z},
                        {{3, 2, 1}, -2_z},
                        {{0, 1, 2}, 1_z},
                        {{0, 0, 0}, 3_z},
                        {{2, 0, 0}, 2_z},
                        {{1, 0, 0}, 1_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(x, {{1, 1_z}, {2, 1_z}});
    RCP<const UnivariateIntPolynomial> p3
        = univariate_int_polynomial(y, {{1, 1_z}, {2, 1_z}});
    REQUIRE(add_mult_poly(*p1, *p2)->__str__()
            == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + 3*x**2 + 2*x + 3");
    REQUIRE(add_mult_poly(*p2, *p1)->__str__()
            == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + 3*x**2 + 2*x + 3");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__()
            == "- 2*x**3 y**2 z + x y**2 z**3 + y z**2 + x**2 + 3");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__()
            == "2*x**3 y**2 z - x y**2 z**3 - y z**2 - x**2 - 3");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__()
            == "- 2*x**5 y**2 z + x**3 y**2 z**3 - 2*x**4 y**2 z + x**2 y**2 "
               "z**3 + x**2 y z**2 + 2*x**4 + x y z**2 + 3*x**3 + 4*x**2 + "
               "3*x");
    REQUIRE(mul_mult_poly(*p2, *p1)->__str__()
            == "- 2*x**5 y**2 z + x**3 y**2 z**3 - 2*x**4 y**2 z + x**2 y**2 "
               "z**3 + x**2 y z**2 + 2*x**4 + x y z**2 + 3*x**3 + 4*x**2 + "
               "3*x");

    REQUIRE(add_mult_poly(*p1, *p3)->__str__() == "- 2*x**3 y**2 z + x y**2 "
                                                  "z**3 + y z**2 + 2*x**2 + "
                                                  "y**2 + x + y + 3");
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariateIntPolynomials with a UnivariateIntPolynomial whose "
          "variables are not in the variable set",
          "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y},
            {{{1, 2}, 1_z}, {{2, 1}, -2_z}, {{0, 1}, 1_z}, {{0, 0}, 3_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(z, {{1, 1_z}, {2, 1_z}});
    REQUIRE(add_mult_poly(*p1, *p2)->__str__()
            == "- 2*x**2 y + x y**2 + z**2 + y + z + 3");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__()
            == "- 2*x**2 y + x y**2 - z**2 + y - z + 3");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__()
            == "2*x**2 y - x y**2 + z**2 - y + z - 3");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__()
            == "- 2*x**2 y z**2 + x y**2 z**2 - 2*x**2 y z + x y**2 z + y z**2 "
               "+ y z + 3*z**2 + 3*z");
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UnivariateIntPolynomials with different variables",
          "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const UnivariateIntPolynomial> p1
        = univariate_int_polynomial(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(y, {{0, 1_z}, {1, 1_z}});
    REQUIRE(add_mult_poly(*p1, *p2)->__str__() == "3*x**2 - x + y + 1");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__() == "3*x**2 - x - y - 1");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__() == "- 3*x**2 + x + y + 1");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__()
            == "3*x**2 y + 3*x**2 - x y - x");
    REQUIRE(mul_mult_poly(*p2, *p1)->__str__()
            == "3*x**2 y + 3*x**2 - x y - x");
}

TEST_CASE("Testing addition, subtraction, multiplication of two "
          "UnivariateIntPolynomials with the same variables",
          "[MultivariateIntPolynomial][UnivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UnivariateIntPolynomial> p1
        = univariate_int_polynomial(x, {{1, -1_z}, {2, 3_z}, {0, 0_z}});
    RCP<const UnivariateIntPolynomial> p2
        = univariate_int_polynomial(x, {{0, 1_z}, {1, 1_z}});
    REQUIRE(add_mult_poly(*p1, *p2)->__str__() == "3*x**2 + 1");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__() == "3*x**2 - 2*x - 1");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__() == "- 3*x**2 + 2*x + 1");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__() == "3*x**3 + 2*x**2 - x");
}

TEST_CASE("Testing addition, subtraction, and multiplication of "
          "MultivariateIntPolynomials with empty variable sets",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    vec_sym s;
    vec_uint v;
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(s, {{v, 2_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{0, 0}, 5_z}, {{0, 1}, 1_z}, {{1, 0}, 1_z}});
    REQUIRE(add_mult_poly(*p1, *p2)->__str__() == "x + y + 7");
    REQUIRE(add_mult_poly(*p2, *p1)->__str__() == "x + y + 7");
    REQUIRE(sub_mult_poly(*p1, *p2)->__str__() == "- x - y - 3");
    REQUIRE(sub_mult_poly(*p2, *p1)->__str__() == "x + y + 3");
    REQUIRE(mul_mult_poly(*p1, *p2)->__str__() == "2*x + 2*y + 10");
    REQUIRE(mul_mult_poly(*p2, *p1)->__str__() == "2*x + 2*y + 10");
}

TEST_CASE("Testing Precedence of MultivariateIntPolynomial",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> a = symbol("a");
    Precedence Prec;
    RCP<const MultivariateIntPolynomial> p1
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{0, 0}, 0_z}});
    RCP<const MultivariateIntPolynomial> p2
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{1, 0}, 2_z}, {{0, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> p3
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{0, 0}, 5_z}});
    RCP<const MultivariateIntPolynomial> p4
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{1, 0}, 1_z}});
    RCP<const MultivariateIntPolynomial> p5
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{1, 1}, 4_z}});
    RCP<const MultivariateIntPolynomial> p6
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y}, {{{2, 0}, 1_z}});
    REQUIRE(Prec.getPrecedence(p1) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p2) == PrecedenceEnum::Add);
    REQUIRE(Prec.getPrecedence(p3) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p4) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p5) == PrecedenceEnum::Mul);
    REQUIRE(Prec.getPrecedence(p6) == PrecedenceEnum::Pow);
}

TEST_CASE("Testing MultivariateIntPolynomial::get_args()",
          "[MultivariateIntPolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const MultivariateIntPolynomial> p
        = MultivariateIntPolynomial::multivariate_int_polynomial(
            {x, y, z}, {{{0, 0, 0}, 1_z}, {{1, 1, 1}, 2_z}, {{0, 0, 2}, 1_z}});
    REQUIRE(
        vec_basic_eq_perm(p->get_args(), {mul(integer(2), mul(x, mul(y, z))),
                                          pow(z, integer(2)), one}));
}

TEST_CASE("Constructing MultivariatePolynomial", "[MultivariatePolynomial]")
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
    vec_sym s;
    vec_uint v;

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomial> pprime
        = MultivariatePolynomial::multivariate_polynomial(
            {y, x},
            {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{1, 0}, comp1},
                                                           {{0, 0}, comp2},
                                                           {{2, 2}, comp3},
                                                           {{3, 4}, comp4}});
    RCP<const MultivariatePolynomial> p3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(integer(0))}});
    RCP<const MultivariatePolynomial> p4
        = MultivariatePolynomial::multivariate_polynomial(s,
                                                          {{v, Expression(0)}});
    RCP<const MultivariatePolynomial> p5
        = MultivariatePolynomial::multivariate_polynomial(s, {{v, comp1}});

    REQUIRE(p1->__str__() == "2*x**2 y - b*x y**2 + a*x y - 3*y");
    REQUIRE(pprime->__str__() == "- b*x**2 y + 2*x y**2 + a*x y - 3*x");
    REQUIRE(p2->__str__()
            == "(-4 - f)*x**3 y**4 + (-3 + e)*x**2 y**2 + (1 + c)*x + (2 - d)");
    REQUIRE(p3->__str__() == "0");
    REQUIRE(p4->__str__() == "0");
    REQUIRE(p5->__str__() == "(1 + c)");
}

TEST_CASE("Testing MultivariatePolynomial::__eq__(), __hash__, and compare",
          "[MultivariatePolynomial]")
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
    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 0}, sum}, {{1, 1}, Expression(a)}, {{0, 2}, sum}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{2, 0}, sum}, {{1, 1}, Expression(a) * -1}, {{0, 2}, sum}});
    RCP<const MultivariatePolynomial> p3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 0}, sum + sum}, {{0, 2}, sum + sum}});
    RCP<const MultivariatePolynomial> p4
        = MultivariatePolynomial::multivariate_polynomial(
            {a, b}, {{{2, 0}, sum + sum}, {{0, 2}, sum + sum}});
    vec_sym s;
    vec_uint v;
    RCP<const MultivariatePolynomial> p5
        = MultivariatePolynomial::multivariate_polynomial(s,
                                                          {{v, Expression(0)}});
    RCP<const MultivariatePolynomial> p6
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(0)}});
    RCP<const MultivariatePolynomial> p7
        = MultivariatePolynomial::multivariate_polynomial(s, {{v, sum}});
    RCP<const MultivariatePolynomial> p8
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{0, 0}, sum}});

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

TEST_CASE("Testing MultivariatePolynomial::eval", "[MultivariatePolynomial]")
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

    RCP<const MultivariatePolynomial> p
        = MultivariatePolynomial::multivariate_polynomial({x, y, z},
                                                          {{{2, 0, 0}, expr1},
                                                           {{0, 2, 0}, expr2},
                                                           {{0, 0, 2}, expr3},
                                                           {{1, 1, 1}, expr4},
                                                           {{1, 1, 0}, expr1},
                                                           {{0, 1, 1}, expr2},
                                                           {{1, 0, 0}, expr1},
                                                           {{0, 1, 0}, expr2},
                                                           {{0, 0, 1}, expr3},
                                                           {{0, 0, 0}, expr4}});
    std::map<RCP<const Symbol>, Expression, RCPSymbolCompare> m1
        = {{x, Expression(0)}, {y, Expression(0)}, {z, Expression(0)}};
    std::map<RCP<const Symbol>, Expression, RCPSymbolCompare> m2
        = {{x, ex}, {y, why}, {z, zee}};
    REQUIRE(p->eval(m1) == expr4);
    REQUIRE(p->eval(m2)
            == ex * ex * expr1 + why * why * expr2 + zee * zee * expr3
                   + ex * why * zee * expr4 + ex * why * expr1
                   + why * zee * expr2 + ex * expr1 + why * expr2 + zee * expr3
                   + expr4);
}

TEST_CASE("Testing derivative of MultivariatePolynomial",
          "[MultivariatePolynomial]")
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
    RCP<const MultivariatePolynomial> p
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{2, 1}, expr1},
                                                           {{1, 2}, expr2},
                                                           {{2, 0}, expr3},
                                                           {{0, 2}, expr4},
                                                           {{1, 0}, expr1},
                                                           {{0, 1}, expr2},
                                                           {{0, 0}, expr3}});

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {
                        {{1, 1}, expr1 * 2},
                        {{0, 2}, expr2},
                        {{1, 0}, expr3 * 2},
                        {{0, 0}, expr1},
                    });
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{2, 0}, expr1},
                                                           {{1, 1}, expr2 * 2},
                                                           {{0, 1}, expr4 * 2},
                                                           {{0, 0}, expr2}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(0)}});

    REQUIRE(eq(*(p->diff(x)), *q1));
    REQUIRE(eq(*(p->diff(y)), *q2));
    REQUIRE(eq(*(p->diff(z)), *q3));
}

TEST_CASE("Testing MultivariatePolynomial::get_args()",
          "[MultivariatePolynomial]")
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
    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{0, 0, 0}, Expression(1)},
                        {{1, 1, 1}, Expression(2)},
                        {{0, 0, 2}, Expression(1)}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial({x, y, z},
                                                          {{{0, 0, 0}, expr1},
                                                           {{1, 1, 1}, expr2},
                                                           {{0, 0, 2}, expr3},
                                                           {{0, 2, 0}, expr4}});
    REQUIRE(
        vec_basic_eq_perm(p1->get_args(), {mul(integer(2), mul(x, mul(y, z))),
                                           pow(z, integer(2)), one}));
    REQUIRE(vec_basic_eq_perm(p2->get_args(),
                              {mul(expr2.get_basic(), mul(x, mul(y, z))),
                               mul(expr4.get_basic(), pow(y, integer(2))),
                               mul(expr3.get_basic(), pow(z, integer(2))),
                               expr1.get_basic()}));
}

TEST_CASE("Testing MultivariatePolynomial neg_mult_poly",
          "[MultivariatePolynomial]")
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

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{1, 1}, a}, {{1, 2}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{1, 0}, comp1},
                                                           {{0, 0}, comp2},
                                                           {{2, 2}, comp3},
                                                           {{3, 4}, comp4}});
    RCP<const MultivariatePolynomial> p3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(integer(0))}});

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a * -1},
                     {{1, 2}, negB * -1},
                     {{2, 1}, num1 * -1},
                     {{0, 1}, negNum * -1}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, comp1 * -1},
                     {{0, 0}, comp2 * -1},
                     {{2, 2}, comp3 * -1},
                     {{3, 4}, comp4 * -1}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(integer(0))}});

    REQUIRE(eq(*neg_mult_poly(*p1), *q1));
    REQUIRE(eq(*neg_mult_poly(*p2), *q2));
    REQUIRE(eq(*neg_mult_poly(*p3), *q3));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomials with the same set of variables",
          "[MultivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression num1(integer(2));                             // 2
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{1, 1}, a}, {{1, 0}, negB}, {{2, 1}, num1}, {{0, 1}, negNum}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, comp1}, {{0, 0}, comp4}});
    RCP<const MultivariatePolynomial> p3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(integer(0))}});

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 1}, num1},
                     {{1, 1}, a},
                     {{1, 0}, comp1 + negB},
                     {{0, 1}, negNum},
                     {{0, 0}, comp4}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 1}, num1},
                     {{1, 1}, a},
                     {{1, 0}, (-1 * comp1) + negB},
                     {{0, 1}, negNum},
                     {{0, 0}, comp4 * -1}});
    RCP<const MultivariatePolynomial> q22
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 1}, -1 * num1},
                     {{1, 1}, -1 * a},
                     {{1, 0}, (comp1)-negB},
                     {{0, 1}, -1 * negNum},
                     {{0, 0}, comp4}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{3, 1}, 2 * comp1},
                     {{
                          2, 1,
                      },
                      2 * comp4 + a * comp1},
                     {{2, 0}, negB * comp1},
                     {{1, 1}, a * comp4 + negNum * comp1},
                     {{
                          1, 0,
                      },
                      negB * comp4},
                     {{0, 1}, negNum * comp4}});

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
          "[MultivariatePolynomial]")
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

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial(
            {m, n}, {{{1, 0}, comp1}, {{2, 1}, comp4}});

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {m, n, x, y}, {{{0, 0, 1, 1}, a},
                           {{0, 0, 1, 0}, negB},
                           {{0, 0, 0, 0}, negNum},
                           {{1, 0, 0, 0}, comp1},
                           {{2, 1, 0, 0}, comp4}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {m, n, x, y}, {{{0, 0, 1, 1}, a},
                           {{0, 0, 1, 0}, negB},
                           {{0, 0, 0, 0}, negNum},
                           {{1, 0, 0, 0}, comp1 * -1},
                           {{2, 1, 0, 0}, comp4 * -1}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {m, n, x, y}, {{{0, 0, 1, 1}, a * -1},
                           {{0, 0, 1, 0}, negB * -1},
                           {{0, 0, 0, 0}, negNum * -1},
                           {{1, 0, 0, 0}, comp1},
                           {{2, 1, 0, 0}, comp4}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
            {m, n, x, y}, {{{2, 1, 1, 1}, a * comp4},
                           {{2, 1, 1, 0}, negB * comp4},
                           {{2, 1, 0, 0}, negNum * comp4},
                           {{1, 0, 1, 1}, a * comp1},
                           {{1, 0, 1, 0}, negB * comp1},
                           {{1, 0, 0, 0}, negNum * comp1}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "MultivariatePolynomials with an overlapping set of variables",
          "[MultivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial(
            {y, z}, {{{1, 0}, comp1}, {{2, 1}, comp4}});

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial({x, y, z},
                                                          {{{1, 1, 0}, a},
                                                           {{1, 0, 0}, negB},
                                                           {{0, 0, 0}, negNum},
                                                           {{0, 1, 0}, comp1},
                                                           {{0, 2, 1}, comp4}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{1, 1, 0}, a},
                        {{1, 0, 0}, negB},
                        {{0, 0, 0}, negNum},
                        {{0, 1, 0}, comp1 * -1},
                        {{0, 2, 1}, comp4 * -1}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{1, 1, 0}, a * -1},
                        {{1, 0, 0}, negB * -1},
                        {{0, 0, 0}, negNum * -1},
                        {{0, 1, 0}, comp1},
                        {{0, 2, 1}, comp4}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{1, 3, 1}, a * comp4},
                        {{1, 2, 1}, negB * comp4},
                        {{1, 2, 0}, a * comp1},
                        {{0, 2, 1}, negNum * comp4},
                        {{1, 1, 0}, negB * comp1},
                        {{0, 1, 0}, negNum * comp1}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q2));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q3));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing addition, subtraction, multiplication of "
          "UnivariatePolynomials with the same variable",
          "[MultivariatePolynomial][UnivariatePolynomial]")
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

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x}, {{{1}, expr1 + expr1}, {{0}, expr4 + expr3}, {{2}, expr2}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x}, {{{0}, expr3 - expr4}, {{1}, expr1 - expr1}, {{2}, expr2}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x},
            {{{0}, expr4 - expr3}, {{1}, expr1 - expr1}, {{2}, expr2 * -1}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
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
          "[MultivariatePolynomial][UnivariatePolynomial]")
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

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, expr1},
                     {{2, 0}, expr2},
                     {{0, 0}, expr3 + expr4},
                     {{0, 0}, expr4},
                     {{0, 1}, expr5}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, expr1},
                     {{2, 0}, expr2},
                     {{0, 0}, expr3 - expr4},
                     {{0, 1}, expr5 * -1}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, expr1 * -1},
                     {{2, 0}, expr2 * -1},
                     {{0, 0}, expr4 - expr3},
                     {{0, 1}, expr5}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 1}, expr2 * expr5},
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
          "MultivariatePolynomials with a UnivariatePolynomial whose variable "
          "are in the variable set",
          "[MultivariatePolynomial][UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const UnivariatePolynomial> p2 = univariate_polynomial(
        y, UnivariateExprPolynomial(
               {{0, comp4}, {1, Expression(integer(2))}, {2, comp1}}));

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a},
                     {{0, 2}, comp1},
                     {{1, 0}, negB},
                     {{0, 1}, Expression(2)},
                     {{0, 0}, comp4 + negNum}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a},
                     {{0, 2}, comp1 * -1},
                     {{1, 0}, negB},
                     {{0, 1}, Expression(-2)},
                     {{0, 0}, -1 * comp4 + negNum}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a * -1},
                     {{0, 2}, comp1},
                     {{1, 0}, negB * -1},
                     {{0, 1}, Expression(2)},
                     {{0, 0}, comp4 - negNum}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
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
          "MultivariatePolynomials with a UnivariatePolynomial whose variables "
          "are not in the variable set",
          "[MultivariatePolynomial][UnivariatePolynomial]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    Expression a(symbol("a"));                               // a
    Expression negB(-Expression(symbol("b")));               //-b
    Expression negNum(integer(-3));                          //-3
    Expression comp1(integer(1) + Expression(symbol("c")));  //(1+c)
    Expression comp4(integer(-4) - Expression(symbol("f"))); //(-4 - f)

    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 1}, a}, {{1, 0}, negB}, {{0, 0}, negNum}});
    RCP<const UnivariatePolynomial> p2 = univariate_polynomial(
        z, UnivariateExprPolynomial(
               {{0, comp4}, {1, Expression(integer(2))}, {2, comp1}}));

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{1, 1, 0}, a},
                        {{0, 0, 2}, comp1},
                        {{1, 0, 0}, negB},
                        {{0, 0, 1}, Expression(2)},
                        {{0, 0, 0}, negNum + comp4}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{1, 1, 0}, a},
                        {{0, 0, 2}, comp1 * -1},
                        {{1, 0, 0}, negB},
                        {{0, 0, 1}, Expression(-2)},
                        {{0, 0, 0}, negNum - comp4}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y, z}, {{{1, 1, 0}, a * -1},
                        {{0, 0, 2}, comp1},
                        {{1, 0, 0}, negB * -1},
                        {{0, 0, 1}, Expression(2)},
                        {{0, 0, 0}, -1 * negNum + comp4}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
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
          "MultivariatePolynomial with empty set of variables ",
          "[MultivariatePolynomial]")
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

    vec_sym s;
    vec_uint v;
    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(s, {{v, expr1}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, expr2}, {{0, 1}, expr3}, {{1, 0}, expr4}});

    RCP<const MultivariatePolynomial> q1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{0, 0}, expr2 + expr1}, {{0, 1}, expr3}, {{1, 0}, expr4}});
    RCP<const MultivariatePolynomial> q2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y},
            {{{0, 0}, expr2 - expr1}, {{0, 1}, expr3}, {{1, 0}, expr4}});
    RCP<const MultivariatePolynomial> q3
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, expr1 - expr2},
                     {{0, 1}, -1 * expr3},
                     {{1, 0}, -1 * expr4}});
    RCP<const MultivariatePolynomial> q4
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, expr2 * expr1},
                     {{0, 1}, expr3 * expr1},
                     {{1, 0}, expr4 * expr1}});

    REQUIRE(eq(*add_mult_poly(*p1, *p2), *q1));
    REQUIRE(eq(*add_mult_poly(*p2, *p1), *q1));
    REQUIRE(eq(*sub_mult_poly(*p1, *p2), *q3));
    REQUIRE(eq(*sub_mult_poly(*p2, *p1), *q2));
    REQUIRE(eq(*mul_mult_poly(*p1, *p2), *q4));
    REQUIRE(eq(*mul_mult_poly(*p2, *p1), *q4));
}

TEST_CASE("Testing Precedence of MultivariatePolynomial",
          "[MultivariatePolynomial]")
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
    RCP<const MultivariatePolynomial> p1
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{0, 0}, Expression(0)}});
    RCP<const MultivariatePolynomial> p2
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, expr1}, {{0, 0}, expr2}});
    RCP<const MultivariatePolynomial> p3
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{0, 0}, expr5}});
    RCP<const MultivariatePolynomial> p4
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{1, 0}, Expression(1)}});
    RCP<const MultivariatePolynomial> p5
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{1, 1}, expr4}});
    RCP<const MultivariatePolynomial> p6
        = MultivariatePolynomial::multivariate_polynomial(
            {x, y}, {{{2, 0}, Expression(1)}});
    RCP<const MultivariatePolynomial> p7
        = MultivariatePolynomial::multivariate_polynomial({x, y},
                                                          {{{1, 0}, expr1}});
    REQUIRE(Prec.getPrecedence(p1) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p2) == PrecedenceEnum::Add);
    REQUIRE(Prec.getPrecedence(p3) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p4) == PrecedenceEnum::Atom);
    REQUIRE(Prec.getPrecedence(p5) == PrecedenceEnum::Mul);
    REQUIRE(Prec.getPrecedence(p6) == PrecedenceEnum::Pow);
    REQUIRE(Prec.getPrecedence(p7) == PrecedenceEnum::Mul);
}
