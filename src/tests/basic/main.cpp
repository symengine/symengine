#include "Teuchos_RCP.hpp"
#include "add.h"

using Teuchos::RCP;
using Teuchos::rcp;
using CSymPy::Add;

int main(int argc, char* argv[])
{

    std::unordered_map<int, int> m;
    RCP<Add> a = rcp(new Add(m));

    return 0;
}
