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
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::integer_class;
using SymEngine::vec_basic_eq_perm;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UnivariateSeries", "[UnivariateSeries")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 0_z}};
    RCP<const UnivariateSeries> P = univariate_series(x, 2, adict_);
    REQUIRE(P->__str__() == "1 + 2*x + O(x**2)");

    RCP<const UnivariateSeries> Q = UnivariateSeries::create(x, 5, {0_z, 1_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "1 + 2*x**2 + x**3 + O(x**5)");
}

TEST_CASE("Adding two UnivariateSeries", "[UnivariateSeries")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    map_uint_mpz bdict_ = {{0, 2_z}, {1, 3_z}, {2, 4_z}};
    map_uint_mpz ddict_ = {{0, 3_z}, {1, 5_z}, {2, 5_z}};

    const UnivariateSeries a(x, 5, std::move(adict_));
    const UnivariateSeries b(x, 4, std::move(bdict_));
    RCP<const UnivariateSeries> c = add_uni_series(a, b);
    const UnivariateSeries d(x, 4, std::move(ddict_));
    REQUIRE(*c == d);

    const UnivariateSeries e(x, 2, std::move(bdict_));
    RCP<const UnivariateSeries> f = add_uni_series(a, e);
    const UnivariateSeries g(x, 2, std::move(ddict_));
    REQUIRE(*f == g);
}

TEST_CASE("Negative of a UnivariateSeries", "[UnivariateSeries")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    map_uint_mpz bdict_ = {{0, -1_z}, {1, -2_z}, {2, -1_z}};

    const UnivariateSeries a(x, 5, std::move(adict_));
    RCP<const UnivariateSeries> b = neg_uni_series(a);
    const UnivariateSeries c(x, 5, std::move(bdict_));
    REQUIRE(*b == c);
}

TEST_CASE("Subtracting two UnivariateSeries", "[UnivariateSeries]")
{
    RCP<const Symbol> x  = symbol("x");
    map_uint_mpz adict_ = {{0, 1_z}, {1, 2_z}, {2, 1_z}};
    map_uint_mpz bdict_ = {{0, 2_z}, {1, 3_z}, {2, 4_z}};
    map_uint_mpz fdict_ = {{0, -1_z}, {1, -1_z}, {2, -3_z}};
    map_uint_mpz gdict_ = {{0, -1_z}, {1, -1_z}};

    const UnivariateSeries a(x, 5, std::move(adict_));
    const UnivariateSeries b(x, 4, std::move(bdict_));
    RCP<const UnivariateSeries> c = sub_uni_series(a, b);
    const UnivariateSeries f(x, 4, std::move(fdict_));
    REQUIRE(*c == f);

    const UnivariateSeries d(x, 2, std::move(bdict_));
    RCP<const UnivariateSeries> e = sub_uni_series(a, d);
    const UnivariateSeries g(x, 2, std::move(gdict_));
    REQUIRE(*e == g);
}

TEST_CASE("Multiplication of two UnivariateSeries", "[UnivariateSeries")
{
    RCP<const Symbol> x  = symbol("x");
    RCP<const UnivariateSeries> a = univariate_series(x, 5, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UnivariateSeries> b = univariate_series(x, 3, {{0, -1_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UnivariateSeries> e = univariate_series(x, 5, {{0, 1_z}, {1, 4_z}, {2, 6_z}, {3, 4_z}, {4, 1_z}});
    RCP<const UnivariateSeries> f = univariate_series(x, 3, {{0, -1_z}, {1, -4_z}, {2, -6_z}});

    RCP<const UnivariateSeries> c = mul_uni_series(*a, *a);
    RCP<const UnivariateSeries> d = mul_uni_series(*a, *b);

    REQUIRE(*c == *e);
    REQUIRE(*d == *f);
}
