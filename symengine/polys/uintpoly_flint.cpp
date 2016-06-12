#include <symengine/symbol.h>
#include <symengine/polys/uintpoly_flint.h>
#ifdef HAVE_SYMENGINE_FLINT

namespace SymEngine
{

UIntPolyFlint::UIntPolyFlint(const RCP<const Symbol> &var,
                             flint::fmpz_polyxx &&dict)
    : UIntPolyBase(var, std::move(dict))
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

    for (unsigned int i = 0; i < poly_.length(); ++i) {
        if (poly_.get_coeff(i) != s.poly_.get_coeff(i))
            return (poly_.get_coeff(i) < s.poly_.get_coeff(i)) ? -1 : 1;
    }
    return 0;
}

RCP<const UIntPolyFlint> UIntPolyFlint::from_dict(const RCP<const Symbol> &var,
                                                  map_uint_mpz &&d)
{
    // benchmark this against dict->str->fmpz_polyxx
    unsigned int deg = 0;
    if (not d.empty())
        deg = d.rbegin()->first;

    flint::fmpz_polyxx f(deg + 1);
    flint::fmpzxx r;
    for (auto const &p : d) {
        fmpz_set_mpz(r._data().inner, get_mpz_t(p.second));
        f.set_coeff(p.first, r);
    }
    return make_rcp<const UIntPolyFlint>(var, std::move(f));
}

RCP<const UIntPolyFlint>
UIntPolyFlint::from_vec(const RCP<const Symbol> &var,
                        const std::vector<integer_class> &v)
{
    // benchmark this against vec->str->fmpz_polyxx
    unsigned int deg = v.size() - 1;
    while (v[deg] == integer_class(0))
        deg--;

    flint::fmpz_polyxx f(deg + 1);
    flint::fmpzxx r;
    for (unsigned int i = 0; i <= deg; i++) {
        if (v[i] != integer_class(0)) {
            fmpz_set_mpz(r._data().inner, get_mpz_t(v[i]));
            f.set_coeff(i, r);
        }
    }
    return make_rcp<const UIntPolyFlint>(var, std::move(f));
}

integer_class UIntPolyFlint::eval(const integer_class &x) const
{
    flint::fmpzxx r;
    fmpz_set_mpz(r._data().inner, get_mpz_t(x));

    flint::fmpzxx ans(poly_(r));
    return to_integer_class(ans);
}

integer_class UIntPolyFlint::get_coeff(unsigned int x) const
{
    return to_integer_class(static_cast<flint::fmpzxx>(poly_.get_coeff(x)));
}

flint::fmpzxx_srcref UIntPolyFlint::get_coeff_ref(unsigned int x) const
{
    return static_cast<flint::fmpzxx_srcref>(poly_.get_coeff(x));
}
}
#endif