#include <symengine/symbol.h>
#include <symengine/polys/uintpoly_flint.h>

namespace SymEngine
{

UIntPolyFlint::UIntPolyFlint(const RCP<const Symbol> &var,
                             flint::fmpz_polyxx &&dict)
    : UPolyBase(var, std::move(dict))
{
}

std::size_t UIntPolyFlint::__hash__() const
{
    std::hash<std::string> str_hash;
    std::size_t seed = UINTPOLYFLINT;

    seed += str_hash(var_->get_name());
    hash_combine(seed, str_hash(poly_.to_string()));
    return seed;
}

int UIntPolyFlint::compare(const Basic &o) const
{
    const UIntPolyFlint &s = static_cast<const UIntPolyFlint &>(o);

    if (poly_.degree() != s.poly_.degree())
        return (poly_.degree() < s.poly_.degree()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    for (unsigned int i = 0; i < poly_.degree(); ++i) {
        if (poly_.get_coeff(i) != s.poly_.get_coeff(i))
            return (poly_.get_coeff(i) < s.poly_.get_coeff(i)) ? -1 : 1;
    }
    return 0;
}

}