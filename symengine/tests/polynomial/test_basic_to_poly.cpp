#include "catch.hpp"

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/basic_to_poly.cpp>

using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::Integer;
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
    RCP<const Integer> one = integer(1);
    RCP<const Integer> minus_one = integer(-1);
    RCP<const Number> i2 = rcp_static_cast<const Number>(integer(2));
    RCP<const Number> i3 = rcp_static_cast<const Number>(integer(3));
    RCP<const Number> half = rcp_static_cast<const Number>(div(one, integer(2)));
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> xb2 = div(x, i2);
    RCP<const Basic> twopx = pow(i2, x);

    // x**2 + x**(1/2) -> (x**(1/2))
    basic = add(pow(x, half), pow(x, i2));
    gens = _find_gens_poly(basic);
    rgens = {{x, half}};
    REQUIRE(unified_eq(gens, rgens));

    // x**(-1/2) + x**2 + x**(-1) -> (x, x**(-1/2))
    basic = add(add(pow(x, neg(half)), pow(x, i2)), pow(x, minus_one));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {pow(x, minus_one), half}};
    REQUIRE(unified_eq(gens, rgens));

    // x/2 + 1/2 -> (x)
    basic = add(xb2, half);
    gens = _find_gens_poly(basic);
    rgens = {{x, one}};
    REQUIRE(unified_eq(gens, rgens));

    // x*y*z**2 -> (x, y, z)
    basic = mul(x, mul(y, pow(z, i2)));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {y, one}, {z, one}};
    REQUIRE(unified_eq(gens, rgens));

    // 2**(2*x + 1) -> (2**x)
    basic = pow(i2, add(mul(i2, x), one));
    gens = _find_gens_poly(basic);
    rgens = {{twopx, one}};
    REQUIRE(unified_eq(gens, rgens));

    // 2**(2**x) + 2**(2**(x+1)) -> (2**(2**x))
    basic = add(pow(i2, twopx), pow(i2, pow(i2, add(x, one))));
    gens = _find_gens_poly(basic);
    rgens = {{pow(i2, twopx), one}};
    REQUIRE(unified_eq(gens, rgens));
}