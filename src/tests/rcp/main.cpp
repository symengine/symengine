#include "Teuchos_RCP.hpp"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::rcp;
using Teuchos::null;

class Mesh {
public:
    int x, y;
};

int main(int argc, char* argv[])
{

    RCP<Mesh> m = rcp(new Mesh());
    Ptr<Mesh> p = m.ptr();
    if (m == null) return 1;
    if (p == null) return 1;

    return 0;
}
