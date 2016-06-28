#include <symengine/symbol.h>
#include <symengine/polys/uintpoly_flint.h>
#ifdef HAVE_SYMENGINE_FLINT

namespace SymEngine
{

UIntPolyFlint::UIntPolyFlint(const RCP<const Symbol> &var, fp_t &&dict)
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
        if (poly_.coeff(i) != s.poly_.coeff(i))
            return (poly_.coeff(i) < s.poly_.coeff(i)) ? -1 : 1;
    }
    return 0;
}

RCP<const UIntPolyFlint> UIntPolyFlint::from_dict(const RCP<const Symbol> &var,
                                                  map_uint_mpz &&d)
{
    // benchmark this against dict->str->fmpz_polyxx
    if (d.empty())
        return UIntPolyFlint::from_dict(var, {{}});

    fp_t f;
    for (auto const &p : d) {
        fz_t r(get_mpz_t(p.second));
        f.set_coeff(p.first, r);
    }
    return make_rcp<const UIntPolyFlint>(var, std::move(f));
}

RCP<const UIntPolyFlint> UIntPolyFlint::from_vec(const RCP<const Symbol> &var,
                                                 const vec_integer_class &v)
{
    // benchmark this against vec->str->fmpz_polyxx
    unsigned int deg = v.size() - 1;
    while (v[deg] == integer_class(0))
        deg--;

    fp_t f(deg + 1);
    for (unsigned int i = 0; i <= deg; i++) {
        if (v[i] != integer_class(0)) {
            fz_t r(get_mpz_t(v[i]));
            f.set_coeff(i, r);
        }
    }
    return make_rcp<const UIntPolyFlint>(var, std::move(f));
}

integer_class UIntPolyFlint::eval(const integer_class &x) const
{
    fz_t r(get_mpz_t(x));
    return to_integer_class(poly_.eval(r));
}

vec_integer_class UIntPolyFlint::multieval(const vec_integer_class &v) const
{
    const unsigned int n = v.size();
    fmpz *fvp = _fmpz_vec_init(n);
    for (unsigned int i = 0; i < n; ++i)
        fmpz_set_mpz(fvp + i, get_mpz_t(v[i]));

    poly_.eval_vec(fvp, fvp, n);
    vec_integer_class res(n);
    for (unsigned int i = 0; i < n; ++i)
        res[i] = to_integer_class(fvp + i);
    _fmpz_vec_clear(fvp, n);
    return res;
}

integer_class UIntPolyFlint::get_coeff(unsigned int x) const
{
    return to_integer_class(poly_.coeff(x));
}

fz_t UIntPolyFlint::get_coeff_ref(unsigned int x) const
{
    if (x > poly_.degree())
        return fz_t(0);
    return poly_.coeff(x);
}
}
#endif
