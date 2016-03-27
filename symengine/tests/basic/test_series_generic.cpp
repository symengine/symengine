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
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::integer_class;
using SymEngine::rational;
using SymEngine::vec_basic_eq_perm;
using SymEngine::Expression;
using SymEngine::umap_short_basic;

using namespace SymEngine::literals;

TEST_CASE("Create UnivariateSeries", "[UnivariateSeries]")
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

    RCP<const UnivariateSeries> a = UnivariateSeries::create(x, 3, apoly_);
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
    UnivariateExprPolynomial b(univariate_polynomial(x, 0, {{0, 1}}));
    UnivariateExprPolynomial c(univariate_polynomial(x, 1, {{0, 2}, {1, 2}}));
    REQUIRE(UnivariateSeries::diff(a, b) == c);
}

TEST_CASE("Integration of UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    UnivariateExprPolynomial a(univariate_polynomial(x, 2, {{0, 1}, {1, 2}, {2, 3}}));
    UnivariateExprPolynomial b(univariate_polynomial(x, 0, {{0, 1}}));
    UnivariateExprPolynomial c(univariate_polynomial(x, 3, {{1, 1}, {2, 1}, {3, 1}}));
    REQUIRE(UnivariateSeries::integrate(a, b) == c);
}

#define series_coeff(EX,SYM,PREC,COEFF) UnivariateSeries::series(EX,SYM->get_name(),PREC)->get_poly().find_cf({COEFF}).get_basic()
#define invseries_coeff(EX,SYM,PREC,COEFF) UnivariateSeries::series_reverse(UnivariateSeries::series(EX,SYM->get_name(),PREC)->get_poly(),p_expr(SYM->get_name()),PREC).find_cf({COEFF}).get_basic()

static bool expand_check_pairs(const RCP<const Basic> &ex, const RCP<const Symbol> &x, int prec, const umap_short_basic& pairs)
{
    auto ser = SymEngine::UnivariateSeries::series(ex, x->get_name(), prec);
    for (auto it : pairs) {
        if (not it.second->__eq__(*(ser->get_poly().find_cf({it.first}).get_basic())))
            return false;
    }
    return true;
}

TEST_CASE("Expression series expansion: Add ", "[Expansion of Add]")
{
    RCP<const Symbol> x = symbol("x"), y = symbol("y");
    auto z = add(integer(1), x);
    z = sub(z, pow(x, integer(2)));
    z = add(z, pow(x, integer(4)));
    auto vb = umap_short_basic{{0, integer(1)}, {1, integer(1)}, {2, integer(-1)}, {4, integer(1)}};
    REQUIRE(expand_check_pairs(z, x, 5, vb));
    auto vb1 = umap_short_basic{{0, integer(1)}, {1, integer(1)}, {2, integer(-1)}};
//    REQUIRE(expand_check_pairs(z, x, 4, vb1));
}

TEST_CASE("Expression series expansion: sin, cos", "[Expansion of sin, cos]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Integer> one = integer(1);
    auto z1 = sin(x);
    auto z2 = cos(x);
    auto z3 = add(sin(x), cos(x));
    auto z4 = mul(sin(x), cos(x));
    auto z5 = sin(atan(x));
    auto z6 = cos(div(x, sub(one, x)));

    REQUIRE(series_coeff(z1, x, 10, 9)->__eq__(*rational(1, 362880)));
    auto res = umap_short_basic{{0, integer(1)}, {2, rational(-1, 2)}};
    REQUIRE(expand_check_pairs(z2, x, 3, res));
    REQUIRE(series_coeff(z3, x, 9, 8)->__eq__(*rational(1, 40320)));
    REQUIRE(series_coeff(z4, x, 12, 11)->__eq__(*rational(-4, 155925)));
    REQUIRE(series_coeff(z5, x, 30, 27)->__eq__(*rational(-1300075, 8388608)));
    REQUIRE(series_coeff(z6, x, 15, 11)->__eq__(*rational(-125929, 362880)));
}
