#include <symengine/symbol.h>
#include <symengine/expression.h>
#include <symengine/polys/uintpoly_piranha.h>

namespace SymEngine
{

UIntPolyPiranha::UIntPolyPiranha(const RCP<const Basic> &var, pintpoly &&dict)
    : UPiranhaPoly(var, std::move(dict))
{
}

hash_t UIntPolyPiranha::__hash__() const
{
    hash_t seed = UINTPOLYPIRANHA;
    seed += poly_.hash();
    seed += var_->hash();
    return seed;
}

URatPolyPiranha::URatPolyPiranha(const RCP<const Basic> &var, pratpoly &&dict)
    : UPiranhaPoly(var, std::move(dict))
{
}

hash_t URatPolyPiranha::__hash__() const
{
    hash_t seed = URATPOLYPIRANHA;
    seed += poly_.hash();
    seed += var_->hash();
    return seed;
}
}
