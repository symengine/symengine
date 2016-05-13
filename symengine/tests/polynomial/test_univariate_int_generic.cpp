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
    RCP<const UnivariateInt> P
        = univariate_int(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UnivariateInt> Q = univariate_int(x, {1_z, 0_z, 2_z, 1_z, 0_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    // constructors
    UnivariateInt R(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R.__str__() == "x**3 + 2*x**2 + 1");

    UnivariateInt T(x, 2, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(T.__str__() == "x**2 + 2*x + 1");

    // for dict with just a single pair, type must be explicitly stated
    // see the two examples below for the difference
    RCP<const UnivariateInt> U = univariate_int(x, map_uint_mpz{{0, 2_z}});
    REQUIRE(U->__str__() == "2");

    RCP<const UnivariateInt> V
        = univariate_int(x, std::vector<integer_class>{{0, 2_z}});
    REQUIRE(V->__str__() == "2*x");
}