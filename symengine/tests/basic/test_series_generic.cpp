#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/series_generic.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UnivariatePolynomial;
using SymEngine::UnivariateExprPolynomial;
using SymEngine::UnivariateSeries;
using SymEngine::univariate_series;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_int_Expr;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::integer_class;
using SymEngine::vec_basic_eq_perm;
using SymEngine::Expression;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    UnivariateExprPolynomial apoly_(univariate_polynomial(x, 2, std::move(adict_)));
    RCP<const UnivariateSeries> P = univariate_series(x, 2, apoly_);
    REQUIRE(P->__str__() == "x**2 + 2*x + 1 + O(x**2)");

    map_int_Expr bdict_ = {{0, 1}, {1, 0}, {2, 2}, {3, 1}};
    UnivariateExprPolynomial bpoly_(UnivariatePolynomial::from_dict(x, std::move(bdict_)));
    RCP<const UnivariateSeries> Q = UnivariateSeries::create(x, 5, bpoly_);
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1 + O(x**5)");
}

TEST_CASE("Adding two UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    UnivariateExprPolynomial apoly_(univariate_polynomial(x, 2, std::move(adict_)));
    map_int_Expr bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    UnivariateExprPolynomial bpoly_(univariate_polynomial(x, 2, std::move(bdict_)));
    map_int_Expr ddict_ = {{0, 3}, {1, 5}, {2, 5}};
    UnivariateExprPolynomial dpoly_(univariate_polynomial(x, 2, std::move(ddict_)));

    RCP<const UnivariateSeries> a = UnivariateSeries::create(x, 5, apoly_);
    RCP<const UnivariateSeries> b = UnivariateSeries::create(x, 4, bpoly_);
    RCP<const Basic> c = add(a, b);
    RCP<const UnivariateSeries> d = UnivariateSeries::create(x, 4, dpoly_);
    REQUIRE(c->__cmp__(*d));

    RCP<const UnivariateSeries> e = UnivariateSeries::create(x, 2, bpoly_);
    RCP<const Basic> f = add(a, e);
    RCP<const Basic> g = UnivariateSeries::create(x, 2, dpoly_);
    REQUIRE(f->__cmp__(*g));
}

TEST_CASE("Negative of a UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    UnivariateExprPolynomial apoly_(univariate_polynomial(x, 2, std::move(adict_)));
    map_int_Expr bdict_ = {{0, -1}, {1, -2}, {2, -1}};
    UnivariateExprPolynomial bpoly_(univariate_polynomial(x, 2, std::move(bdict_)));

    RCP<const UnivariateSeries> a = UnivariateSeries::create(x, 5, apoly_);
    RCP<const Basic> b = neg(a);
    RCP<const UnivariateSeries> c = UnivariateSeries::create(x, 5, bpoly_);
    REQUIRE(b->__cmp__(*c));
}

TEST_CASE("Subtracting two UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    UnivariateExprPolynomial apoly_(univariate_polynomial(x, 2, std::move(adict_)));
    map_int_Expr bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    UnivariateExprPolynomial bpoly_(univariate_polynomial(x, 2, std::move(bdict_)));
    map_int_Expr fdict_ = {{0, -1}, {1, -1}, {2, -3}};
    UnivariateExprPolynomial fpoly_(univariate_polynomial(x, 2, std::move(fdict_)));
    map_int_Expr gdict_ = {{0, -1}, {1, -1}};
    UnivariateExprPolynomial gpoly_(univariate_polynomial(x, 1, std::move(gdict_)));

    RCP<const UnivariateSeries> a = UnivariateSeries::create(x, 5, apoly_);
    RCP<const UnivariateSeries> b = UnivariateSeries::create(x, 4, bpoly_);
    RCP<const Basic> c = sub(a, b);
    RCP<const UnivariateSeries> f = UnivariateSeries::create(x, 4, fpoly_);
    REQUIRE(c->__cmp__(*f));

    RCP<const UnivariateSeries> d = UnivariateSeries::create(x, 2, bpoly_);
    RCP<const Basic> e = sub(a, d);
    RCP<const UnivariateSeries> g = UnivariateSeries::create(x, 2, gpoly_);
    REQUIRE(e->__cmp__(*f));
}

TEST_CASE("Multiplication of two UnivariateExprPolynomial with precision", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    UnivariateExprPolynomial a(univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}}));
    UnivariateExprPolynomial b(univariate_polynomial(x, 2, {{0, -1}, {1, -2}, {2, -1}}));
    UnivariateExprPolynomial c(univariate_polynomial(x, 3, {{0, 1}, {1, 4}, {2, 6}, {3, 4}}));
    UnivariateExprPolynomial d(univariate_polynomial(x, 4, {{0, -1}, {1, -4}, {2, -6}, {3, -4}, {4, -1}}));
    
    UnivariateExprPolynomial e = UnivariateSeries::mul(a, a, 4);
    UnivariateExprPolynomial f = UnivariateSeries::mul(a, b, 5);

    REQUIRE(e == c);
    REQUIRE(f == d);
}

TEST_CASE("Exponentiation of UnivariateExprPolynomial with precision", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    UnivariateExprPolynomial a(univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}}));
    UnivariateExprPolynomial b(univariate_polynomial(x, 2, {{0, -1}, {1, -2}, {2, -1}}));
    UnivariateExprPolynomial c(univariate_polynomial(x, 3, {{0, 1}, {1, 4}, {2, 6}, {3, 4}}));
    UnivariateExprPolynomial d(univariate_polynomial(x, 4, {{0, -1}, {1, -6}, {2, -15}, {3, -20}, {4, -15}}));
    
    UnivariateExprPolynomial e = UnivariateSeries::pow(a, 2, 4);
    UnivariateExprPolynomial f = UnivariateSeries::pow(b, 3, 5);

    REQUIRE(e == c);
    REQUIRE(f == d);
}

TEST_CASE("Differentiation of UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    UnivariateExprPolynomial a(univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 1}}));
    UnivariateExprPolynomial b(univariate_polynomial(x, 1, {{0, 2}, {1, 2}}));
    UnivariateExprPolynomial c(univariate_polynomial(x, 1, {{0, 2}, {1, 2}}));
    REQUIRE(UnivariateSeries::diff(a, b) == c);
}

TEST_CASE("Integration of UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    UnivariateExprPolynomial a(univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 3}}));
    UnivariateExprPolynomial b(univariate_polynomial(x, 1, {{0, 2}, {1, 2}}));
    UnivariateExprPolynomial c(univariate_polynomial(x, 3, {{1, 1}, {2, 1}, {3, 1}}));
    REQUIRE(UnivariateSeries::integrate(a, b) == c);
}
