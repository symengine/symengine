#include "catch.hpp"

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/basic_to_poly.cpp>

using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::add;
using SymEngine::umap_basic_num;
using SymEngine::Number;
using SymEngine::rcp_static_cast;

using namespace SymEngine::literals;

TEST_CASE("find_gen_poly", "[find_gen]")
{
    umap_basic_num gens, rgens;
    RCP<const Basic> basic;
    RCP<const Number> i2 = rcp_static_cast<const Number>(integer(2));
    RCP<const Number> i3 = rcp_static_cast<const Number>(integer(3));
    RCP<const Number> half = rcp_static_cast<const Number>(div(one, integer(2)));
    RCP<const Symbol> x = symbol("x");

    basic = add(pow(x, half), pow(x, i2));
    gens = _find_gens_poly(basic);
    rgens = {{x, half}};

    REQUIRE(unified_eq(gens, rgens));
}