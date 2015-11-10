#include "catch.hpp"
#include <iostream>
#include <chrono>

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/real_double.h>

using SymEngine::Basic;
using SymEngine::Add;
using SymEngine::Mul;
using SymEngine::Pow;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::umap_basic_num;
using SymEngine::map_vec_int;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::multinomial_coefficients;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::sin;
using SymEngine::RCP;
using SymEngine::rcp_dynamic_cast;
using SymEngine::map_basic_basic;
using SymEngine::print_stack_on_segfault;
using SymEngine::real_double;

TEST_CASE("Symbol: subs", "[subs]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = x;
    RCP<const Basic> r2 = y;
    map_basic_basic d;
    d[x] = y;
    REQUIRE(eq(*r1->subs(d), *r2));
    REQUIRE(neq(*r1->subs(d), *r1));
}

TEST_CASE("Add: subs", "[subs]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = add(x, y);
    RCP<const Basic> r2 = mul(i2, y);
    map_basic_basic d;
    d[x] = y;
    REQUIRE(eq(*r1->subs(d), *r2));

    d[x] = z;
    d[y] = w;
    r1 = add(x, y);
    r2 = add(z, w);
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[add(x, y)] = z;
    r1 = add(x, y);
    r2 = z;
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[pow(x, y)] = z;
    d[pow(x, i2)] = y;
    d[pow(i2, y)] = x;
    r1 = add(add(pow(x, y), pow(x, i2)), pow(i2, y));
    r2 = add(add(x, y), z);
    REQUIRE(eq(*r1->subs(d), *r2));

    r1 = add(add(add(add(pow(x, y), pow(x, i2)), pow(i2, y)), x), i3);
    r2 = add(add(add(mul(i2, x), y), z), i3);
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[x] = integer(5);
    r1 = add(mul(integer(12), add(integer(3), sin(x))), sin(integer(4)));
    r2 = add(mul(integer(12), add(integer(3), sin(integer(5)))),
            sin(integer(4)));
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[x] = integer(1);
    r1 = add(mul(x, add(integer(3), sin(integer(4)))), sin(integer(1)));
    r2 = add(add(integer(3), sin(integer(4))), sin(integer(1)));
    REQUIRE(eq(*r1->subs(d), *r2));

    // (2+2*x).subs({2: y}) -> y+x*y
    d.clear();
    d[i2] = y;
    r1 = add(i2, mul(i2, x));
    r2 = add(y, mul(x, y));
    REQUIRE(eq(*r1->subs(d), *r2));

    // (1+2*x*y).subs({2*x*y: z}) -> 1+z
    d.clear();
    d[mul(mul(i2, x), y)] = z;
    r1 = add(one, mul(i2, mul(x, y)));
    r2 = add(one, z);
    REQUIRE(eq(*r1->subs(d), *r2));
}

TEST_CASE("Mul: subs", "[subs]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = mul(x, y);
    RCP<const Basic> r2 = pow(y, i2);
    map_basic_basic d;
    d[x] = y;
    REQUIRE(eq(*r1->subs(d), *r2));

    d[x] = z;
    d[y] = w;
    r1 = mul(x, y);
    r2 = mul(z, w);
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[mul(x, y)] = z;
    r1 = mul(x, y);
    r2 = z;
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[pow(x, y)] = z;
    r1 = mul(i2, pow(x, y));
    r2 = mul(i2, z);
    REQUIRE(eq(*r1->subs(d), *r2));

    r1 = mul(x, y)->subs({{x, real_double(0.0)}});
    r2 = real_double(0.0);
    REQUIRE(eq(*r1, *r2));
}

TEST_CASE("Pow: subs", "[subs]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = pow(x, y);
    RCP<const Basic> r2 = pow(y, y);
    map_basic_basic d;
    d[x] = y;
    REQUIRE(eq(*r1->subs(d), *r2));

    d[x] = z;
    d[y] = w;
    r1 = pow(x, y);
    r2 = pow(z, w);
    REQUIRE(eq(*r1->subs(d), *r2));

    r1 = pow(x, i2);
    r2 = pow(z, i2);
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[pow(x, y)] = z;
    r1 = pow(x, y);
    r2 = z;
    REQUIRE(eq(*r1->subs(d), *r2));
}

TEST_CASE("Trig: subs", "[subs]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    RCP<const Basic> r1 = sin(x);
    RCP<const Basic> r2 = zero;
    map_basic_basic d;
    d[x] = zero;
    REQUIRE(eq(*r1->subs(d), *r2));

    r1 = cos(x);
    r2 = one;
    REQUIRE(eq(*r1->subs(d), *r2));

    d[x] = y;
    r1 = sin(pow(x, i2));
    r2 = sin(pow(y, i2));
    REQUIRE(eq(*r1->subs(d), *r2));

    d.clear();
    d[sin(x)] = z;
    r1 = sin(x);
    r2 = z;
    REQUIRE(eq(*r1->subs(d), *r2));

    r1 = mul(i2, sin(x));
    r2 = mul(i2, z);
    REQUIRE(eq(*r1->subs(d), *r2));
}
