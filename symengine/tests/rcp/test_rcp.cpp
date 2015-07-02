#include <cassert>

#include <symengine/symengine_rcp.h>

using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::Ptr;
using SymEngine::null;

class Mesh {
public:
#if defined(WITH_SYMENGINE_RCP)
    mutable unsigned int refcount_;
    Mesh() : refcount_(0) {}
#else
    mutable RCP<const Mesh> weak_self_ptr_;
#endif

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
