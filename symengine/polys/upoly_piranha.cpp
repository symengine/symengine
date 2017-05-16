#include <symengine/symbol.h>
#include <symengine/expression.h>
#include <symengine/polys/upoly_piranha.h>

namespace SymEngine
{

UIntPolyPiranha::UIntPolyPiranha(const RCP<const Basic> &var, pintpoly &&dict)
    : UPiranhaPoly(var, std::move(dict))
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t UIntPolyPiranha::__hash__() const
{
    hash_t seed = UINTPOLYPIRANHA;
    seed += get_poly().hash();
    seed += get_var()->hash();
    return seed;
}

URatPolyPiranha::URatPolyPiranha(const RCP<const Basic> &var, pratpoly &&dict)
    : UPiranhaPoly(var, std::move(dict))
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t URatPolyPiranha::__hash__() const
{
    hash_t seed = URATPOLYPIRANHA;
    seed += get_poly().hash();
    seed += get_var()->hash();
    return seed;
}
}
