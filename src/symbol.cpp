#include "symbol.h"
#include "integer.h"

namespace CSymPy {

Symbol::Symbol(const std::string &name)
    : name_{name}
{
}

std::size_t Symbol::__hash__() const
{
    std::hash<std::string> hash_fn;
    return hash_fn(name_);
}

bool Symbol::__eq__(const Basic &o) const
{
    if (is_a<Symbol>(o)) return (this == &o);
    return false;
}

int Symbol::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Symbol>(o))
    if (this == &o) return 0;
    return this < &o ? -1 : 1;
}

std::string Symbol::__str__() const
{
    return name_;
}

RCP<const Basic> Symbol::diff(const RCP<const Symbol> &x) const
{
    if (x->name_ == this->name_)
        return one;
    else
        return zero;
}

} // CSymPy
