#include "catch.hpp"

#include <symengine/symengine_rcp.h>

using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Ptr;
using SymEngine::null;
using SymEngine::EnableRCPFromThis;

class Mesh : public EnableRCPFromThis {
    int x, y;
};

TEST_CASE("Test make_rcp", "[rcp]")
{

    RCP<Mesh> m = make_rcp<Mesh>();
    Ptr<Mesh> p = m.ptr();
    REQUIRE(m != null);
    REQUIRE(p->use_count() == 1);
    RCP<Mesh> m2 = m;
    REQUIRE(p->use_count() == 2);
    RCP<Mesh> m3 = m2;
    REQUIRE(p->use_count() == 3);
}
