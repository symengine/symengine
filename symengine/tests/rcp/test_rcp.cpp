#include "catch.hpp"

#include <symengine/symengine_rcp.h>

using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Ptr;
using SymEngine::null;
using SymEngine::EnableRCPFromThis;

class Mesh : public EnableRCPFromThis<Mesh> {
public:
    int x, y;
};

class Mesh2 : public EnableRCPFromThis<const Mesh2> {
public:
    int x, y;
};

TEST_CASE("Test make_rcp", "[rcp]")
{

    RCP<Mesh> m = make_rcp<Mesh>();
    Ptr<Mesh> p = m.ptr();
    REQUIRE(!(m == null));
    REQUIRE(p->use_count() == 1);
    RCP<Mesh> m2 = m;
    REQUIRE(p->use_count() == 2);
    RCP<Mesh> m3 = m2;
    REQUIRE(p->use_count() == 3);
}

void f(Mesh &m)
{
    REQUIRE(m.use_count() == 1);
    RCP<Mesh> m2 = m.rcp_from_this();
    REQUIRE(m.use_count() == 2);
    m2->x = 6;
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
}

void f2(const Mesh2 &m)
{
    REQUIRE(m.use_count() == 1);
    RCP<const Mesh2> m2 = m.rcp_from_this();
    REQUIRE(m.use_count() == 2);
}

TEST_CASE("Test rcp_from_this const", "[rcp]")
{

    RCP<const Mesh2> m = make_rcp<const Mesh2>();
    REQUIRE(m->use_count() == 1);
    f2(*m);
    REQUIRE(m->use_count() == 1);
}
