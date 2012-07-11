#include "symbol.h"

namespace CSymPy {

Symbol::Symbol(const std::string &name)
{
    this->name = name;
}

std::size_t Symbol::__hash__() const
{
    std::hash<std::string> hash_fn;
    return hash_fn(this->name);
}

} // CSymPy
