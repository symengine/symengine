#include <cassert>

#include "symengine_rcp.h"

using SymEngine::RCP;
using SymEngine::rcp;
using SymEngine::Ptr;
using SymEngine::null;

class Mesh {
public:
    mutable unsigned int refcount_;
    Mesh() : refcount_(0) {}

    int x, y;
};

int main(int argc, char* argv[])
{

    RCP<Mesh> m = rcp(new Mesh());
    Ptr<Mesh> p = m.ptr();
    if (m == null) return 1;
    if (p->refcount_ != 1) return 1;

    return 0;
}
