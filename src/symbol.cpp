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

bool Symbol::__eq__(const Basic &o) const
{
    if (is_a<Symbol>(o)) {
        const Symbol &s = static_cast<const Symbol &>(o);
        return this->name == s.name;
    }
    return false;
}

} // CSymPy
