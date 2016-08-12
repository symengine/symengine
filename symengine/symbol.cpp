#include <symengine/constants.h>

namespace SymEngine
{

Symbol::Symbol(const std::string &name) : name_{name}
{
}

std::size_t Symbol::__hash__() const
{
    return hash_string(name_);
}

bool Symbol::__eq__(const Basic &o) const
{
    if (is_a<Symbol>(o))
        return name_ == static_cast<const Symbol &>(o).name_;
    return false;
}

int Symbol::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Symbol>(o))
    const Symbol &s = static_cast<const Symbol &>(o);
    if (name_ == s.name_)
        return 0;
    return name_ < s.name_ ? -1 : 1;
}

} // SymEngine
