#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/series_generic.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UnivariateSeries;
using SymEngine::univariate_series;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_int_Expr;
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
    map_int_Expr edict_ = {{0, 1}, {1, 2}, {2, 0}};
    RCP<const UnivariateSeries> P = univariate_series(x, 2, edict_);
    REQUIRE(P->__str__() == "1 + 2*x + O(x**2)");

    std::vector<Expression> v = {1, 0, 2, 1};
    RCP<const UnivariateSeries> Q = UnivariateSeries::create(x, 5, v);
    REQUIRE(Q->__str__() == "1 + 2*x**2 + x**3 + O(x**5)");
}

TEST_CASE("Adding two UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_int_Expr bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    map_int_Expr ddict_ = {{0, 3}, {1, 5}, {2, 5}};

    const UnivariateSeries a(x, 5, adict_);
    const UnivariateSeries b(x, 4, bdict_);
    RCP<const UnivariateSeries> c = add_uni_series(a, b);
    const UnivariateSeries d(x, 4, ddict_);
    REQUIRE(*c == d);

    const UnivariateSeries e(x, 4, bdict_);

    RCP<const UnivariateSeries> f = add_uni_series(a, e);
    const UnivariateSeries g(x, 2, ddict_);
    REQUIRE(*f == g);
}

TEST_CASE("Negative of a UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_int_Expr bdict_ = {{0, -1}, {1, -2}, {2, -1}};

    const UnivariateSeries a(x, 5, adict_);
    RCP<const UnivariateSeries> b = neg_uni_series(a);
    const UnivariateSeries c(x, 5, bdict_);
    REQUIRE(*b == c);
}

TEST_CASE("Subtracting two UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_int_Expr adict_ = {{0, 1}, {1, 2}, {2, 1}};
    map_int_Expr bdict_ = {{0, 2}, {1, 3}, {2, 4}};
    map_int_Expr fdict_ = {{0, -1}, {1, -1}, {2, -3}};
    map_int_Expr gdict_ = {{0, -1}, {1, -1}};

    UnivariateSeries a(x, 5, adict_);
    UnivariateSeries b(x, 2, bdict_);
    RCP<const UnivariateSeries> c = sub_uni_series(a, b);
    UnivariateSeries f(x, 4, fdict_);
    REQUIRE(*c == f);
    UnivariateSeries d(x, 3, bdict_);

    RCP<const UnivariateSeries> e = sub_uni_series(a, d);
    UnivariateSeries g(x, 2, gdict_);
    REQUIRE(*e == g);
}

TEST_CASE("Multiplication of two UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateSeries> a = univariate_series(x, 5, {{0, 1}, {1, 2}, {2, 1}});
    RCP<const UnivariateSeries> b = univariate_series(x, 3, {{0, -1}, {1, -2}, {2, -1}});
    RCP<const UnivariateSeries> e = univariate_series(x, 5, {{0, 1}, {1, 4}, {2, 6}, {3, 4}, {4, 1}});
    RCP<const UnivariateSeries> f = univariate_series(x, 3, {{0, -1}, {1, -4}, {2, -6}});

    RCP<const UnivariateSeries> c = mul_uni_series(*a, *a);
    RCP<const UnivariateSeries> d = mul_uni_series(*a, *b);

    REQUIRE(*c == *e);
    REQUIRE(*d == *f);
}

//TEST_CASE("Differentiation of UnivariateSeries", "[UnivariateSeries]")
//{
  //  RCP<const Symbol> x  = symbol("x");
    //RCP<const UnivariateSeries> var = univariate_series(x, 1, {{0, 1}});
    
