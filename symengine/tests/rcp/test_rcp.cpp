#include <cassert>

#include <symengine/symengine_rcp.h>

using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Ptr;
using SymEngine::null;
using SymEngine::EnableRCPFromThis;

class Mesh : public EnableRCPFromThis {
    int x, y;
};

int main(int argc, char* argv[])
{

    RCP<Mesh> m = make_rcp<Mesh>();
    Ptr<Mesh> p = m.ptr();
    if (m == null) return 1;
    if (p->use_count() != 1) return 1;
    RCP<Mesh> m2 = m;
    if (p->use_count() != 2) return 1;
    RCP<Mesh> m3 = m2;
    if (p->use_count() != 3) return 1;

    return 0;
}
