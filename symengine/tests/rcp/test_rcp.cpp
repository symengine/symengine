#include "catch.hpp"

#include <symengine/symengine_rcp.h>

using SymEngine::EnableRCPFromThis;
using SymEngine::make_rcp;
using SymEngine::null;
using SymEngine::Ptr;
using SymEngine::RCP;

// This is the canonical use of EnableRCPFromThis:

class Mesh : public EnableRCPFromThis<Mesh>
{
public:
    int x, y;
};

TEST_CASE("Test make_rcp", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    Ptr<Mesh> p = m.ptr();
    REQUIRE(not(m == null));
    REQUIRE(p->use_count() == 1);
    RCP<Mesh> m2 = m;
    REQUIRE(p->use_count() == 2);
    RCP<Mesh> m3 = m2;
    REQUIRE(p->use_count() == 3);
}

void f(Mesh &m)
{
    REQUIRE(m.use_count() == 1);
    // rcp_from_this() gives up non const version of RCP<Mesh> because 'm' is
    // not const
    RCP<Mesh> m2 = m.rcp_from_this();
    REQUIRE(m.use_count() == 2);
    m2->x = 6;
}

void f_const(const Mesh &m)
{
    REQUIRE(m.use_count() == 1);
    // rcp_from_this() gives up const version of RCP<Mesh> because 'm' is const
    RCP<const Mesh> m2 = m.rcp_from_this();
    REQUIRE(m.use_count() == 2);
}

TEST_CASE("Test rcp_from_this", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    m->x = 5;
    REQUIRE(m->x == 5);
    f(*m);
    REQUIRE(m->use_count() == 1);
    REQUIRE(m->x == 6);

    f_const(*m);
    REQUIRE(m->use_count() == 1);
}

TEST_CASE("Test rcp_from_this const", "[rcp]")
{
    RCP<const Mesh> m = make_rcp<const Mesh>();
    REQUIRE(m->use_count() == 1);
    f_const(*m);
    REQUIRE(m->use_count() == 1);
}

// This is not a canonical way how to use EnableRCPFromThis, since we use
// 'const Mesh2' for the internal weak pointer, so we can only get
// 'RCP<const Mesh2>' out of rcp_from_this(). But it is legitimate code, so we
// test it as well.

class Mesh2 : public EnableRCPFromThis<const Mesh2>
{
public:
    int x, y;
};

void f2_const(const Mesh2 &m)
{
    REQUIRE(m.use_count() == 1);
    // rcp_from_this() gives up const version of RCP<Mesh> because 'm' is const
    RCP<const Mesh2> m2 = m.rcp_from_this();
    REQUIRE(m.use_count() == 2);
}

void f2_hybrid(Mesh2 &m)
{
    REQUIRE(m.use_count() == 1);
    // rcp_from_this() gives up const version of RCP<Mesh> even though 'm' is
    // not const, because the internal pointer inside Mesh2 is const.
    RCP<const Mesh2> m2 = m.rcp_from_this();
    REQUIRE(m.use_count() == 2);
}

TEST_CASE("Test rcp_from_this const 2", "[rcp]")
{
    RCP<const Mesh2> m = make_rcp<const Mesh2>();
    REQUIRE(m->use_count() == 1);
    f2_const(*m);
    REQUIRE(m->use_count() == 1);

    RCP<Mesh2> m2 = make_rcp<Mesh2>();
    REQUIRE(m2->use_count() == 1);
    f2_const(*m2);
    REQUIRE(m2->use_count() == 1);
    f2_hybrid(*m2);
    REQUIRE(m2->use_count() == 1);
}

#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)
TEST_CASE("Test RCP move construct", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    RCP<Mesh> m2 = std::move(m);
    REQUIRE(m.is_null());
    REQUIRE(m2->use_count() == 1);
}

TEST_CASE("Test RCP move assign", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    RCP<Mesh> m2;
    m2 = std::move(m);
    REQUIRE(m.is_null());
    REQUIRE(m2->use_count() == 1);
}
#endif

TEST_CASE("Test RCP reset", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    m.reset();
    REQUIRE(m.is_null());

    // reset on null is safe
    m.reset();
    REQUIRE(m.is_null());
}

TEST_CASE("Test RCP copy does not affect source", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    {
        RCP<Mesh> m2 = m;
        REQUIRE(m->use_count() == 2);
        REQUIRE(m2->use_count() == 2);
    }
    REQUIRE(m->use_count() == 1);
}

TEST_CASE("Test RCP field access", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    m->x = 10;
    m->y = 20;
    REQUIRE(m->x == 10);
    REQUIRE(m->y == 20);
}

TEST_CASE("Test make_rcp const", "[rcp]")
{
    RCP<const Mesh> m = make_rcp<const Mesh>();
    REQUIRE(m->use_count() == 1);
    REQUIRE(not(m == null));
}

TEST_CASE("Test rcp_from_this increments count", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    {
        RCP<Mesh> m2 = m->rcp_from_this();
        REQUIRE(m->use_count() == 2);
        REQUIRE(m2->use_count() == 2);
    }
    REQUIRE(m->use_count() == 1);
}

TEST_CASE("Test RCP null comparison", "[rcp]")
{
    RCP<Mesh> m;
    REQUIRE(m == null);
    m = make_rcp<Mesh>();
    REQUIRE(not(m == null));
}

TEST_CASE("Test RCP copy assign to null RCP", "[rcp]")
{
    RCP<Mesh> m;
    RCP<Mesh> m2 = make_rcp<Mesh>();
    m = m2;
    REQUIRE(m->use_count() == 2);
    REQUIRE(m2->use_count() == 2);
}

TEST_CASE("Test RCP copy assign from null", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    RCP<Mesh> m2;
    m = m2;
    REQUIRE(m.is_null());
}

#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)
TEST_CASE("Test cooperative_intrusive counter queries", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->is_uniquely_owned_by_cpp());
    REQUIRE(m->is_uniquely_owned());
    REQUIRE(not m->is_external_owned());

    RCP<Mesh> m2 = m;
    REQUIRE(not m->is_uniquely_owned_by_cpp());
    REQUIRE(not m->is_uniquely_owned());
    REQUIRE(not m->is_external_owned());
}
#endif

TEST_CASE("Test backend-neutral unique ownership query", "[rcp]")
{
    RCP<Mesh> m = make_rcp<Mesh>();
#if defined(WITH_SYMENGINE_THREAD_SAFE)
    // Thread-safe legacy RCP intentionally preserves the historical no-steal
    // behavior until that optimization can be reviewed independently.
    REQUIRE(not m->is_uniquely_owned());
#else
    REQUIRE(m->is_uniquely_owned());
    RCP<Mesh> m2 = m;
    REQUIRE(not m->is_uniquely_owned());
#endif
}
