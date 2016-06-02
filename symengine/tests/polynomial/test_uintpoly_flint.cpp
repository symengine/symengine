#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polys/uintpoly_flint.h>
#include <symengine/pow.h>
#include <symengine/dict.h>

using SymEngine::UIntPolyFlint;
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

using namespace SymEngine::literals;

TEST_CASE("Constructor of UIntPolyFlint", "[UIntPoly]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> P = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UIntPolyFlint> Q = UIntPolyFlint::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyFlint> R = UIntPolyFlint::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyFlint> S = UIntPolyFlint::from_dict(x, {{0, 2_z}});
    REQUIRE(S->__str__() == "2");

    RCP<const UIntPolyFlint> T = UIntPolyFlint::from_dict(x, {});
    REQUIRE(T->__str__() == "0");

    RCP<const UIntPolyFlint> U = UIntPolyFlint::from_dict(x, {{0, 2_z}, {1, 0_z}, {2, 0_z}});
    REQUIRE(U->__str__() == "2");
}