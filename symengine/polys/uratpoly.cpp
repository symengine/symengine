#include <symengine/polys/uratpoly.h>

namespace SymEngine
{

URatPoly::URatPoly(const RCP<const Basic> &var, URatDict &&dict)
    : USymEnginePoly(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

std::size_t URatPoly::__hash__() const
{
    std::size_t seed = URATPOLY;

    seed += var_->hash();
    for (const auto &it : poly_.dict_) {
        std::size_t temp = URATPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(get_num(it.second)));
        hash_combine<long long int>(temp, mp_get_si(get_den(it.second)));
        seed += temp;
    }
    return seed;
}

} // SymEngine
