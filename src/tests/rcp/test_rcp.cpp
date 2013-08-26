#include <cassert>

#include "csympy_rcp.h"

using CSymPy::RCP;
using CSymPy::rcp;
using CSymPy::Ptr;
using CSymPy::null;

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
