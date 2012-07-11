#include <iostream>
#include "Teuchos_RCP.hpp"
#include "basic.h"
#include "add.h"
#include "symbol.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Basic;
using CSymPy::Add;
using CSymPy::Symbol;

void test_symbol_hash()
{
    RCP<Symbol> x = rcp(new Symbol("x"));
    RCP<Symbol> y = rcp(new Symbol("y"));

    std::size_t seed1 = 0;
    hash_combine<std::string>(seed1, "x");
    hash_combine<std::string>(seed1, "y");

    std::size_t seed2 = 0;
    hash_combine<Basic>(seed2, *x);
    hash_combine<Basic>(seed2, *y);

    assert(seed1 == seed2);
}

int main(int argc, char* argv[])
{
    test_symbol_hash();

    std::unordered_map<int, int> m;
    RCP<Add> a = rcp(new Add(m));

    return 0;
}
