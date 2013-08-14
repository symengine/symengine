#include "symbol.h"
#include "integer.h"

using Teuchos::RCP;

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
    if (is_a<Symbol>(o))
        return name_ == static_cast<const Symbol &>(o).name_;
    return false;
}

std::string Symbol::__str__() const
{
    return name_;
}

RCP<Basic> Symbol::diff(const Teuchos::RCP<Symbol> &x) const
{
    if (x->name_ == this->name_)
        return one;
    else
        return zero;
}

} // CSymPy
