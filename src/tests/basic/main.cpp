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

namespace std
{
    // Specialise std::hash for Basic. We just call Basic.__hash__()
    template<>
    class hash<Basic> : public unary_function<Basic, size_t>
    {
    public:
        size_t operator()(const Basic& b)
        {
            return b.__hash__();
        }
    };
}

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

    std::size_t seed1 = 0;
    hash_combine<std::string>(seed1, "x");
    hash_combine<std::string>(seed1, "y");

    std::size_t seed2 = 0;
    hash_combine<Basic>(seed2, *x);
    hash_combine<Basic>(seed2, *y);

    assert(seed1 == seed2);

    return 0;
}
