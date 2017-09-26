#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/pow.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/functions.h>

using SymEngine::Basic;
using SymEngine::symbol;
using SymEngine::RCP;
using SymEngine::pow;
using SymEngine::add;
using SymEngine::sub;
using SymEngine::mul;
using SymEngine::div;
using SymEngine::neg;
using SymEngine::vec_basic;
using SymEngine::vec_pair;
using SymEngine::sqrt;
using SymEngine::sin;
using SymEngine::cse;
using SymEngine::unified_eq;
using SymEngine::integer;

TEST_CASE("CSE: simple", "[cse]")
{
    RCP<const Basic> x = symbol("x");
    RCP<const Basic> x0 = symbol("x0");
    RCP<const Basic> x1 = symbol("x1");
    RCP<const Basic> x2 = symbol("x2");
    RCP<const Basic> y = symbol("y");
    RCP<const Basic> z = symbol("z");
    RCP<const Basic> w = symbol("w");
    RCP<const Basic> i2 = integer(2);
    RCP<const Basic> i3 = integer(3);
    RCP<const Basic> i4 = integer(4);

    {
        auto e = add(pow(add(x, y), i2), sqrt(add(x, y)));
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e});
        REQUIRE(unified_eq(substs, {{x0, add(x, y)}}));
        REQUIRE(unified_eq(reduced, {add(sqrt(x0), pow(x0, i2))}));
    }
    {
        auto e = add(x, y);
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e});
        REQUIRE(unified_eq(substs, {}));
        REQUIRE(unified_eq(reduced, {e}));
    }
    {
        auto e = add(pow(add(mul(w, x), y), i2), sqrt(add(mul(w, x), y)));
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e});
        REQUIRE(unified_eq(substs, {{x0, add(mul(w, x), y)}}));
        REQUIRE(unified_eq(reduced, {add(sqrt(x0), pow(x0, i2))}));
    }
    {
        auto e1 = mul(add(x, y), z);
        auto e2 = mul(add(x, y), w);
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e1, e2});
        REQUIRE(unified_eq(substs, {{x0, add(x, y)}}));
        REQUIRE(unified_eq(reduced, {mul(x0, z), mul(x0, w)}));
    }
    {
        auto e1 = add(mul(mul(w, x), y), z);
        auto e2 = mul(y, w);
        vec_pair substs;
        vec_pair rsubsts;
        vec_basic reduced;
        vec_basic rreduced;
        cse(substs, reduced, {e1, e2});
        cse(rsubsts, rreduced, {e2, e1});
        REQUIRE(unified_eq(substs, rsubsts));
        REQUIRE(unified_eq(reduced, {add(mul(x0, x), z), x0}));
    }
    {
        auto e1 = mul(mul(w, x), y);
        auto e2 = add(mul(mul(w, x), y), z);
        auto e3 = mul(y, w);
        vec_pair substs;
        vec_pair rsubsts;
        vec_basic reduced;
        vec_basic rreduced;
        cse(substs, reduced, {e1, e2, e3});
        cse(rsubsts, rreduced, {e3, e2, e1});
        REQUIRE(unified_eq(substs, rsubsts));
        REQUIRE(unified_eq(reduced, {x1, add(x1, z), x0}));
    }
    {
        auto e2 = sub(x, z);
        auto e3 = sub(y, z);
        auto e1 = mul(e2, e3);
        vec_pair substs;
        vec_pair rsubsts;
        vec_basic reduced;
        vec_basic rreduced;
        cse(substs, reduced, {e1, e2, e3});
        cse(rsubsts, rreduced, {e3, e2, e1});
        REQUIRE(unified_eq(substs,
                           {{x0, neg(z)}, {x1, add(x, x0)}, {x2, add(x0, y)}}));
        REQUIRE(unified_eq(rsubsts,
                           {{x0, neg(z)}, {x1, add(y, x0)}, {x2, add(x0, x)}}));
        REQUIRE(unified_eq(reduced, {mul(x1, x2), x1, x2}));
    }
    {
        auto e2 = sub(x, z);
        auto e3 = sub(y, z);
        auto e1 = mul(e2, e3);
        vec_pair substs;
        vec_pair rsubsts;
        vec_basic reduced;
        vec_basic rreduced;
        cse(substs, reduced, {e1, e2, e3});
        cse(rsubsts, rreduced, {e3, e2, e1});
        REQUIRE(unified_eq(substs,
                           {{x0, neg(z)}, {x1, add(x, x0)}, {x2, add(x0, y)}}));
        REQUIRE(unified_eq(rsubsts,
                           {{x0, neg(z)}, {x1, add(y, x0)}, {x2, add(x0, x)}}));
        REQUIRE(unified_eq(reduced, {mul(x1, x2), x1, x2}));
    }
    {
        auto e1 = add(x, add(w, add(y, add(z, mul(w, y)))));
        auto e2 = mul(w, mul(x, y));
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e1, e2});
        REQUIRE(unified_eq(substs, {{x0, mul(w, y)}}));
        REQUIRE(unified_eq(reduced,
                           {add(x, add(w, add(y, add(z, x0)))), mul(x0, x)}));
    }
    {
        auto e1 = add(x, y);
        auto e2 = add(x, add(y, z));
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e1, e2});
        REQUIRE(unified_eq(substs, {{x0, add(x, y)}}));
        REQUIRE(unified_eq(reduced, {x0, add(z, x0)}));
    }
    {
        auto e1 = add(x, y);
        auto e2 = add(x, z);
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e1, e2});
        REQUIRE(unified_eq(substs, {}));
        REQUIRE(unified_eq(reduced, {e1, e2}));
    }
    {
        auto e1 = mul(x, y);
        auto e2 = add(z, mul(x, y));
        auto e3 = add(i3, mul(z, mul(x, y)));
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e1, e2, e3});
        REQUIRE(unified_eq(substs, {{x0, mul(x, y)}}));
        REQUIRE(unified_eq(reduced, {x0, add(z, x0), add(i3, mul(x0, z))}));
    }
    {
        auto e = div(sin(pow(x, x)), pow(x, x));
        vec_pair substs;
        vec_basic reduced;
        cse(substs, reduced, {e});
        REQUIRE(unified_eq(substs, {{x0, pow(x, x)}}));
        REQUIRE(unified_eq(reduced, {div(sin(x0), x0)}));
    }
}
