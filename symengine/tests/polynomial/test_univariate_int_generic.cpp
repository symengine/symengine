#include "catch.hpp"
#include <iostream>

#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/dict.h>
#include <symengine/univariate_int_generic.h>

using SymEngine::univariate_int;
using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;
using SymEngine::UnivariateInt;
using namespace SymEngine::literals;

TEST_CASE("Constructors of UnivariateInt", "[UnivariateInt]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");

    // univariate_int
    RCP<const UnivariateInt> P, Q, T, U, V;

    P = univariate_int(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    Q = univariate_int(x, {{1_z, 0_z, 2_z, 1_z, 0_z}});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    // constructors
    UnivariateInt R(x, {{1_z, 0_z, 2_z, 1_z}});
    REQUIRE(R.__str__() == "x**3 + 2*x**2 + 1");

    UnivariateInt S(x, 2, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(S.__str__() == "x**2 + 2*x + 1");

    // for dict with just a single pair, type must be explicitly stated
    // see the two examples below for the difference
    T = univariate_int(x, map_uint_mpz{{0, 2_z}});
    REQUIRE(T->__str__() == "2");

    U = univariate_int(x, std::vector<integer_class>{{0_z, 2_z}});
    REQUIRE(U->__str__() == "2*x");

    CHECK_THROWS_AS(V = univariate_int(none, {{0, 1_z}, {1, 2_z}}),
                    std::runtime_error);
}

TEST_CASE("Adding two UnivariateInt", "[UnivariateInt]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    RCP<const Symbol> y = symbol("y");

    RCP<const UnivariateInt> a, b, c, d, e, f, g;

    a = univariate_int(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    b = univariate_int(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    c = univariate_int(none, map_uint_mpz{{0, 1_z}});

    d = add_poly(*a, *b);
    e = add_poly(*a, *c);
    f = add_poly(*c, *a);

    REQUIRE(d->__str__() == "5*x**2 + 5*x + 3");
    REQUIRE(e->__str__() == "x**2 + 2*x + 2");
    REQUIRE(f->__str__() == "x**2 + 2*x + 2");

    g = univariate_int(y, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    CHECK_THROWS_AS(add_poly(*a, *g), std::runtime_error);
}

TEST_CASE("Negative of a UnivariateInt", "[UnivariateInt]")
{
    RCP<const Symbol> x = symbol("x");

    RCP<const UnivariateInt> a, c;

    a = univariate_int(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    c = univariate_int(x, map_uint_mpz{{0, 0_z}});

    RCP<const Basic> b = neg_poly(*a);
    RCP<const Basic> d = neg_poly(*c);

    REQUIRE(b->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(d->__str__() == "0");
}

TEST_CASE("Subtracting two UnivariateInt", "[UnivariateInt]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> none = symbol("");
    RCP<const Symbol> y = symbol("y");

    RCP<const UnivariateInt> a, b, c, d, e, f;

    a = univariate_int(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    b = univariate_int(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    c = univariate_int(none, map_uint_mpz{{0, 2_z}});

    d = sub_poly(*b, *a);
    e = sub_poly(*c, *a);
    f = sub_poly(*a, *c);

    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    REQUIRE(e->__str__() == "-x**2 - 2*x + 1");
    REQUIRE(f->__str__() == "x**2 + 2*x - 1");
}