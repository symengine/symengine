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
#if defined(WITH_SYMENGINE_RCP)
    if (p->refcount_ != 1) return 1;
#endif

    return 0;
}
