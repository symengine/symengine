#include "Teuchos_RCP.hpp"
#include "add.h"
#include "symbol.h"

using Teuchos::RCP;
using Teuchos::rcp;

using CSymPy::Add;
using CSymPy::Symbol;

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

int main(int argc, char* argv[])
{
    RCP<Symbol> x = rcp(new Symbol("x"));
    RCP<Symbol> y = rcp(new Symbol("y"));

    std::unordered_map<int, int> m;
    RCP<Add> a = rcp(new Add(m));

    return 0;
}
