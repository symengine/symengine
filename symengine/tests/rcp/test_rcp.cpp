#include "catch.hpp"

#include <symengine/symengine_rcp.h>

using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Ptr;
using SymEngine::null;
using SymEngine::EnableRCPFromThis;

class Mesh : public EnableRCPFromThis {
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
    RCP<const Mesh> m2 = m.get_rcp_cast<const Mesh>();
    REQUIRE(m.use_count() == 2);
//  m2 is read only --- need to fix get_rcp_cast to allow cast away the const
//    m2->x = 6;
}

TEST_CASE("Test get_rcp_cast", "[rcp]")
{

    RCP<Mesh> m = make_rcp<Mesh>();
    REQUIRE(m->use_count() == 1);
    m->x = 5;
    f(*m);
    REQUIRE(m->use_count() == 1);
//    REQUIRE(m->x == 6);
}
