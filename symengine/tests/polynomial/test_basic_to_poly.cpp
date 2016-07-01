#include "catch.hpp"

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/rational.h>
#include <symengine/polys/upolybase.h>

using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::Integer;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::Rational;
using SymEngine::print_stack_on_segfault;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::minus_one;
using SymEngine::E;
using SymEngine::pi;
using SymEngine::integer;
using SymEngine::add;
using SymEngine::umap_basic_num;
using SymEngine::Number;
using SymEngine::rcp_static_cast;
using SymEngine::sin;

using namespace SymEngine::literals;

TEST_CASE("find_gen_poly", "[find_gen]")
{
    umap_basic_num gens, rgens;
    RCP<const Basic> basic;
    RCP<const Integer> one = integer(1);
    RCP<const Integer> minus_one = integer(-1);
    RCP<const Number> i2 = rcp_static_cast<const Number>(integer(2));
    RCP<const Number> i3 = rcp_static_cast<const Number>(integer(3));
    RCP<const Number> i6 = rcp_static_cast<const Number>(integer(6));
    RCP<const Number> hf = rcp_static_cast<const Number>(div(one, integer(2)));
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> xb2 = div(x, i2);
    RCP<const Basic> twopx = pow(i2, x);

    // x**2 + x**(1/2) -> (x**(1/2))
    basic = add(pow(x, hf), pow(x, i2));
    gens = _find_gens_poly(basic);
    rgens = {{x, hf}};
    REQUIRE(unified_eq(gens, rgens));

    // x**(-1/2) + x**2 + x**(-1) -> (x, x**(-1/2))
    basic = add(add(pow(x, neg(hf)), pow(x, i2)), pow(x, minus_one));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {pow(x, minus_one), hf}};
    REQUIRE(unified_eq(gens, rgens));

    // x/2 + 1/2 -> (x)
    basic = add(xb2, hf);
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

    // sin(x)*sin(y) + sin(x)**2 + sin(y) -> (sin(x), sin(y))
    basic = add(mul(sin(x), sin(y)), add(pow(sin(x), i2), sin(y)));
    gens = _find_gens_poly(basic);
    rgens = {{sin(x), one}, {sin(y), one}};
    REQUIRE(unified_eq(gens, rgens));

    // 2**x + 2**(x+y) -> (2**x, 2**y)
    basic = add(twopx, pow(i2, add(x, y)));
    gens = _find_gens_poly(basic);
    rgens = {{pow(i2, y), one}, {twopx, one}};
    REQUIRE(unified_eq(gens, rgens));

    // x**x + x**(x/2) + x**(x/3) -> (x**(x/6))
    basic = add(pow(x, x), add(pow(x, div(x, i2)), pow(x, div(x, i3))));
    gens = _find_gens_poly(basic);
    rgens = {{pow(x, x), rcp_static_cast<const Number>(div(one, i6))}};
    REQUIRE(unified_eq(gens, rgens));

    // x + (1/(x**2)) -> (x, 1/x)
    basic = add(x, div(one, pow(x, i2)));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {pow(x, minus_one), one}};
    REQUIRE(unified_eq(gens, rgens));

    // x + (1/(x**2)) -> (x, 1/x)
    basic = add(x, div(one, pow(x, i2)));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {pow(x, minus_one), one}};

    // ((x+1)**6)*(y**2) + x*y**3 -> (x, y)
    basic = add(mul(x, pow(y, i3)), mul(pow(add(x, one), i6), pow(y, i2)));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {y, one}};
    REQUIRE(unified_eq(gens, rgens));

    // ((x+1)**6)*(y**2) + x*y**3 -> (x, y)
    basic = add(mul(x, pow(y, i3)), mul(pow(add(x, one), i6), pow(y, i2)));
    gens = _find_gens_poly(basic);
    rgens = {{x, one}, {y, one}};
    REQUIRE(unified_eq(gens, rgens));

    // 2**(3x) + y/2 + z**-2 -> (2**x, y, 1/z)
    basic = add(pow(i2, mul(x, i3)), add(div(y, i2), pow(z, neg(i2))));
    gens = _find_gens_poly(basic);
    rgens = {{pow(i2, x), one}, {y, one}, {pow(z, minus_one), one}};
    REQUIRE(unified_eq(gens, rgens));

    // E**2 + E*pi -> (E, pi)
    basic = add(pow(E, i2), mul(E, pi));
    gens = _find_gens_poly(basic);
    rgens = {{E, one}, {pi, one}};
    REQUIRE(unified_eq(gens, rgens));

    // 3 +(1/2) -> ()
    basic = add(i3, div(one, i2));
    gens = _find_gens_poly(basic);
    rgens = {};
    REQUIRE(unified_eq(gens, rgens));

    // x**(3/2) -> (x**(1/2))
    basic = pow(x, div(i3, i2));
    gens = _find_gens_poly(basic);
    rgens = {{x, hf}};
    REQUIRE(unified_eq(gens, rgens));
}
