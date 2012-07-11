#include "Teuchos_RCP.hpp"
#include "add.h"
#include "symbol.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Add;
using CSymPy::Symbol;

int main(int argc, char* argv[])
{
    RCP<Symbol> x = rcp(new Symbol("x"));
    RCP<Symbol> y = rcp(new Symbol("y"));

    std::unordered_map<int, int> m;
    RCP<Add> a = rcp(new Add(m));

    return 0;
}
