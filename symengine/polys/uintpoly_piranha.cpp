#include <symengine/symbol.h>
#include <symengine/expression.h>
#include <symengine/polys/uintpoly_piranha.h>

#ifdef HAVE_SYMENGINE_PIRANHA

namespace SymEngine
{

UIntPolyPiranha::UIntPolyPiranha(const RCP<const Basic> &var, pintpoly &&dict)
    : UIntPolyBase(var, std::move(dict))
{
}

std::size_t UIntPolyPiranha::__hash__() const
{
    std::size_t seed = UINTPOLYPIRANHA;
    seed += poly_.hash();
    seed += var_->hash();
    return seed;
}

int UIntPolyPiranha::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<UIntPolyPiranha>(o))
    const UIntPolyPiranha &s = static_cast<const UIntPolyPiranha &>(o);
    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;
    if (poly_ == s.poly_)
        return 0;
    return (poly_.hash() < s.poly_.hash()) ? -1 : 1;
}

RCP<const UIntPolyPiranha>
UIntPolyPiranha::from_dict(const RCP<const Basic> &var, map_uint_mpz &&d)
{
    pintpoly p;
    piranha::symbol_set ss(
        {{piranha::symbol(SymEngine::detail::poly_print(var))}});
    p.set_symbol_set(ss);
    for (auto &it : d)
        p.insert(pterm{it.second, pmonomial{it.first}});

    return make_rcp<const UIntPolyPiranha>(var, std::move(p));
}

RCP<const UIntPolyPiranha>
UIntPolyPiranha::from_vec(const RCP<const Basic> &var,
                          const vec_integer_class &v)
{
    pintpoly p;
    piranha::symbol_set ss(
        {{piranha::symbol(SymEngine::detail::poly_print(var))}});
    p.set_symbol_set(ss);
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != integer_class(0)) {
            p.insert(pterm{v[i], pmonomial{i}});
        }
    }
    return make_rcp<const UIntPolyPiranha>(var, std::move(p));
}

integer_class UIntPolyPiranha::get_coeff(unsigned int x) const
{
    return poly_.find_cf(pmonomial{x});
}

const integer_class &UIntPolyPiranha::get_coeff_ref(unsigned int x) const
{
    static integer_class PZERO(0);

    pterm temp = pterm{0, pmonomial{x}};
    auto it = poly_._container().find(temp);
    if (it == poly_._container().end())
        return PZERO;
    return it->m_cf;
}

integer_class UIntPolyPiranha::eval(const integer_class &x) const
{
    return piranha::math::evaluate<integer_class>(
        poly_, {{detail::poly_print(var_), x}});
}

vec_integer_class UIntPolyPiranha::multieval(const vec_integer_class &v) const
{
    vec_integer_class res(v.size());
    for (unsigned int i = 0; i < v.size(); ++i)
        res[i] = piranha::math::evaluate<integer_class>(
            poly_, {{detail::poly_print(var_), v[i]}});
    return res;
}
}
#endif