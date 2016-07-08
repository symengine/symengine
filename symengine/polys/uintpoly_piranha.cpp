#include <symengine/symbol.h>
#include <symengine/expression.h>
#include <symengine/polys/uintpoly_piranha.h>

#ifdef HAVE_SYMENGINE_PIRANHA

namespace SymEngine
{

UIntPolyPiranha::UIntPolyPiranha(const RCP<const Basic> &var, pintpoly &&dict)
    : UPiranhaPoly(var, std::move(dict))
{
}

std::size_t UIntPolyPiranha::__hash__() const
{
    std::size_t seed = UINTPOLYPIRANHA;
    seed += poly_.hash();
    seed += var_->hash();
    return seed;
}
}
#endif