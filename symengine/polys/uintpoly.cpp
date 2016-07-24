#include <symengine/polys/uintpoly.h>

namespace SymEngine
{

UIntPoly::UIntPoly(const RCP<const Basic> &var, UIntDict &&dict)
    : USymEnginePoly(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

std::size_t UIntPoly::__hash__() const
{
    std::size_t seed = UINTPOLY;

    seed += var_->hash();
    for (const auto &it : poly_.dict_) {
        std::size_t temp = UINTPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

} // SymEngine
