#include <symengine/symbol.h>
#include <symengine/integer.h>
#include <symengine/constants.h>

namespace SymEngine {

std::map<int, std::string> Symbol::global_symbols;
std::map<int, std::size_t> Symbol::global_symbols_hashes;

std::size_t Symbol::__hash__() const
{
    return global_symbols_hashes[name_counter_];
}

bool Symbol::__eq__(const Basic &o) const
{
    if (is_a<Symbol>(o))
        return name_counter_ == static_cast<const Symbol &>(o).name_counter_;
    return false;
}

int Symbol::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Symbol>(o))
    const Symbol &s = static_cast<const Symbol &>(o);
    if (name_counter_ == s.name_counter_) return 0;
    return name_counter_ < s.name_counter_ ? -1 : 1;
}

RCP<const Basic> Symbol::diff(const RCP<const Symbol> &x) const
{
    if (x->name_counter_ == this->name_counter_)
        return one;
    else
        return zero;
}

} // SymEngine
