#include <symengine/symbol.h>
#include <symengine/polys/uintpoly_flint.h>

namespace SymEngine
{

UIntPolyFlint::UIntPolyFlint(const RCP<const Basic> &var, fzp_t &&dict)
    : UFlintPoly(var, std::move(dict))
{
}

std::size_t UIntPolyFlint::__hash__() const
{
    std::hash<std::string> str_hash;
    std::size_t seed = UINTPOLYFLINT;

    seed += var_->hash();
    hash_combine(seed, str_hash(poly_.to_string()));
    return seed;
}
}
