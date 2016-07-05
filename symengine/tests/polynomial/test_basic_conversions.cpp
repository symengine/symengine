#include "catch.hpp"

#include <symengine/mul.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/rational.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uexprpoly.h>
#include <symengine/polys/basic_conversions.h>

using SymEngine::symbol;
using SymEngine::Symbol;
using SymEngine::Integer;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::Rational;
using SymEngine::print_stack_on_segfault;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::minus_one;
using SymEngine::E;
using SymEngine::pi;
using SymEngine::integer;
using SymEngine::add;
using SymEngine::umap_basic_num;
using SymEngine::Number;
using SymEngine::rcp_static_cast;
using SymEngine::sin;
using SymEngine::eq;
using SymEngine::UIntPoly;
using SymEngine::UExprPoly;
using SymEngine::from_basic;

using namespace SymEngine::literals;

TEST_CASE("find_gen_poly", "[b2poly]")
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

    // 2**(x**(x+1))-> (2**(x**(x+1)))
    basic = pow(i2, pow(x, add(x, one)));
    gens = _find_gens_poly(basic);
    rgens = {{basic, one}};
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

    // 3 + (1/2) -> ()
    basic = add(i3, div(one, i2));
    gens = _find_gens_poly(basic);
    rgens = {};
    REQUIRE(unified_eq(gens, rgens));

    // x**(3/2) -> (x**(1/2))
    basic = pow(x, div(i3, i2));
    gens = _find_gens_poly(basic);
    rgens = {{x, hf}};
    REQUIRE(unified_eq(gens, rgens));

    // 2**(-x + 3) + 2**(-2x) -> (2**(-x))
    basic = add(pow(i2, add(i3, neg(x))), pow(i2, mul(neg(i2), x)));
    gens = _find_gens_poly(basic);
    rgens = {{pow(i2, neg(x)), one}};
    REQUIRE(unified_eq(gens, rgens));
}

TEST_CASE("basic_to_poly UInt", "[b2poly]")
{
    RCP<const Basic> basic, gen;
    RCP<const Integer> one = integer(1);
    RCP<const Integer> minus_one = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i6 = integer(6);
    RCP<const Basic> i9 = integer(9);
    RCP<const Basic> hf = div(one, integer(2));
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Basic> xb2 = div(x, i2);
    RCP<const Basic> twopx = pow(i2, x);
    RCP<const UIntPoly> poly1, poly2, poly3;

    // x**2 + x**(1/2)
    basic = add(pow(x, i2), pow(x, hf));
    gen = pow(x, hf);
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{0_z, 1_z, 0_z, 0_z, 1_z}});
    REQUIRE(eq(*poly1, *poly2));

    // 3x + 2
    basic = add(mul(x, i3), i2);
    gen = x;
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{2_z, 3_z}});
    REQUIRE(eq(*poly1, *poly2));

    // 2**(2x + 1)
    basic = pow(i2, add(mul(i2, x), one));
    gen = pow(i2, x);
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{0_z, 0_z, 2_z}});
    REQUIRE(eq(*poly1, *poly2));

    // 2**(-x + 3) + 2**(-2x) -> (2**(-x))
    basic = add(pow(i2, add(i3, neg(x))), pow(i2, mul(neg(i2), x)));
    gen = pow(i2, neg(x));
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{0_z, 8_z, 1_z}});
    REQUIRE(eq(*poly1, *poly2));

    // x**x + x**(x/2) + x**(x/3)
    basic = add(pow(x, x), add(pow(x, div(x, i2)), pow(x, div(x, i3))));
    gen = pow(x, div(x, i6));
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{0_z, 0_z, 1_z, 1_z, 0_z, 0_z, 1_z}});
    REQUIRE(eq(*poly1, *poly2));

    // (x**(1/2)+1)**3 + (x+2)**6
    basic = add(pow(add(pow(x, hf), one), i3), pow(add(x, i2), i6));
    gen = pow(x, hf);
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = pow_upoly(*UIntPoly::from_vec(gen, {{1_z, 1_z}}), 3);
    poly3 = pow_upoly(*UIntPoly::from_vec(gen, {{2_z, 0_z, 1_z}}), 6);
    poly2 = add_upoly(*poly2, *poly3);
    REQUIRE(eq(*poly1, *poly2));

    // (2**x)**2 * (2**(3x + 2) + 1)
    basic = mul(pow(twopx, i2), add(one, pow(i2, add(i2, mul(x, i3)))));
    gen = twopx;
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{0_z, 0_z, 1_z, 0_z, 0_z, 4_z}});
    REQUIRE(eq(*poly1, *poly2));

    // 9**(x+(1/2)) + 9**(2x +(3/2))
    basic = add(pow(i9, add(x, hf)), pow(i9, add(mul(i2, x), div(i3, i2))));
    gen = pow(i9, x);
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{0_z, 3_z, 27_z}});
    REQUIRE(eq(*poly1, *poly2));

    // 2**(2**x) + 2**(2**(x+1)) + 3
    basic = add(pow(i2, twopx), add(i3, pow(i2, pow(i2, add(x, one)))));
    gen = pow(i2, twopx);
    poly1 = from_basic<UIntPoly>(basic, gen);
    poly2 = UIntPoly::from_vec(gen, {{3_z, 1_z, 1_z}});
    REQUIRE(eq(*poly1, *poly2));

    // x + y
    basic = add(x, y);
    gen = x;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // x + 1/2
    basic = add(x, hf);
    gen = x;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // x/2 + 1
    basic = add(div(x, i2), one);
    gen = x;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // x + 1/x
    basic = add(x, div(one, x));
    gen = x;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // xy + 1
    basic = add(mul(x, y), one);
    gen = x;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // x**(1/2) + 1
    basic = add(pow(x, hf), one);
    gen = x;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // 3**x + 2**x
    basic = add(pow(i3, x), pow(i2, x));
    gen = twopx;
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // 2**(2**(2x + 1)) + 2**(2**x)
    basic = add(pow(i2, twopx), pow(i2, pow(i2, add(mul(i2, x), one))));
    gen = pow(i2, twopx);
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // 9**(x + (1/3))
    basic = pow(i9, add(div(one, i3), x));
    gen = pow(i9, x);
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic, gen), std::runtime_error);

    // x + y
    basic = add(x, y);
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic), std::runtime_error);

    // x + 1/x
    basic = add(x, div(one, x));
    CHECK_THROWS_AS(from_basic<UIntPoly>(basic), std::runtime_error);
}

TEST_CASE("basic_to_poly UExpr", "[b2poly]")
{
    RCP<const Basic> basic, gen;
    RCP<const Integer> one = integer(1);
    RCP<const Integer> minus_one = integer(-1);
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i6 = integer(6);
    RCP<const Basic> i9 = integer(9);
    RCP<const Basic> hf = div(one, integer(2));
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");
    RCP<const Symbol> z = symbol("z");
    RCP<const Basic> xb2 = div(x, i2);
    RCP<const Basic> twopx = pow(i2, x);
    RCP<const UExprPoly> poly1, poly2, poly3;

    // x + xy + (x**1/2)*z
    basic = add(x, add(mul(x, y), mul(z, pow(x, hf))));
    gen = pow(x, hf);
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{zero, z, add(one, y)}});
    REQUIRE(eq(*poly1, *poly2));

    // 3*2**x + 2**(x+y)
    basic = add(mul(i3, twopx), pow(i2, add(x, y)));
    gen = twopx;
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{zero, add(i3, pow(i2, y))}});
    REQUIRE(eq(*poly1, *poly2));

    // 2**(-x + (1/2)) + 2**(-2x)
    basic = add(pow(i2, add(neg(x), hf)), pow(i2, mul(neg(i2), x)));
    gen = pow(i2, neg(x));
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{zero, pow(i2, hf), one}});
    REQUIRE(eq(*poly1, *poly2));

    // xy + xz + yz
    basic = add(mul(x, y), add(mul(x, z), mul(y, z)));
    gen = x;
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{mul(y, z), add(z, y)}});
    REQUIRE(eq(*poly1, *poly2));

    // (x+1)**2 + 2xy
    basic = add(mul(mul(i2, x), y), pow(add(x, one), i2));
    gen = x;
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{one, add(mul(i2, y), i2), one}});
    REQUIRE(eq(*poly1, *poly2));

    // x**x + x**(2x + y)
    basic = add(pow(x, x), pow(x, add(mul(i2, x), y)));
    gen = pow(x, x);
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{zero, one, pow(x, y)}});
    REQUIRE(eq(*poly1, *poly2));

    // (1/2)*x**2 + 1/x
    basic = add(mul(hf, pow(x, i2)), div(one, x));
    gen = x;
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{div(one, x), zero, hf}});
    REQUIRE(eq(*poly1, *poly2));

    // (x/2)**2 + xz
    basic = add(pow(div(x, i2), i2), mul(z, x));
    gen = x;
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{zero, z, div(one, integer(4))}});
    REQUIRE(eq(*poly1, *poly2));

    // pi**2 + E*pi
    basic = add(pow(pi, i2), mul(pi, E));
    gen = pi;
    poly1 = from_basic<UExprPoly>(basic, gen);
    poly2 = UExprPoly::from_vec(gen, {{zero, E, one}});
    REQUIRE(eq(*poly1, *poly2));
}
