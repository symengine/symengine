#include <symengine/polys/basic_to_poly.h>
#include <symengine/pow.h>

namespace SymEngine
{

UIntPoly::UIntPoly(const RCP<const Basic> &var, UIntDict &&dict)
    : UIntPolyBase(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

bool UIntPoly::is_canonical(const UIntDict &dict) const
{
    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict.dict_)
        if (iter.second == 0)
            return false;
    return true;
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

int UIntPoly::compare(const Basic &o) const
{
    const UIntPoly &s = static_cast<const UIntPoly &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = unified_compare(var_, s.var_);
    if (cmp != 0)
        return cmp;

    return unified_compare(poly_.dict_, s.poly_.dict_);
}

RCP<const UIntPoly> UIntPoly::from_dict(const RCP<const Basic> &var,
                                        map_uint_mpz &&d)
{
    UIntDict x(d);
    return make_rcp<const UIntPoly>(var, std::move(x));
}

RCP<const UIntPoly> UIntPoly::from_vec(const RCP<const Basic> &var,
                                       const vec_integer_class &v)
{
    return make_rcp<const UIntPoly>(var, UIntDict::from_vec(v));
}

RCP<const UIntPoly> UIntPoly::from_basic(const RCP<const Basic> &basic,
                                         const RCP<const Basic> &gen, bool e)
{
    RCP<const Basic> exp = basic;
    if (e)
        exp = expand(basic);
    return UIntPoly::from_container(gen, _b_to_upoly<UIntDict>(exp, gen));
}

RCP<const UIntPoly> UIntPoly::from_basic(const RCP<const Basic> &basic, bool e)
{
    RCP<const Basic> exp = basic;
    if (e)
        exp = expand(basic);

    umap_basic_num tmp = _find_gens_poly(exp);

    if (tmp.size() != 1)
        throw std::runtime_error("Did not find exactly 1 generator");

    RCP<const Basic> gen = pow(tmp.begin()->first, tmp.begin()->second);
    return UIntPoly::from_container(gen, _b_to_upoly<UIntDict>(exp, gen));
}

integer_class UIntPoly::eval(const integer_class &x) const
{
    unsigned int last_deg = poly_.dict_.rbegin()->first;
    integer_class result(0), x_pow;

    for (auto it = poly_.dict_.rbegin(); it != poly_.dict_.rend(); ++it) {

        mp_pow_ui(x_pow, x, last_deg - (*it).first);
        last_deg = (*it).first;
        result = (*it).second + x_pow * result;
    }
    mp_pow_ui(x_pow, x, last_deg);
    result *= x_pow;

    return result;
}

vec_integer_class UIntPoly::multieval(const vec_integer_class &v) const
{
    // this is not the optimal algorithm
    vec_integer_class res(v.size());
    for (unsigned int i = 0; i < v.size(); ++i)
        res[i] = eval(v[i]);
    return res;
}

bool UIntPoly::is_zero() const
{
    return poly_.empty();
}

bool UIntPoly::is_one() const
{
    return poly_.size() == 1 and poly_.dict_.begin()->second == 1
           and poly_.dict_.begin()->first == 0;
}

bool UIntPoly::is_minus_one() const
{
    return poly_.size() == 1 and poly_.dict_.begin()->second == -1
           and poly_.dict_.begin()->first == 0;
}

bool UIntPoly::is_integer() const
{
    if (poly_.empty())
        return true;
    if (poly_.size() == 1 and poly_.dict_.begin()->first == 0)
        return true;
    return false;
}

bool UIntPoly::is_symbol() const
{
    if (poly_.size() == 1 and poly_.dict_.begin()->first == 1
        and poly_.dict_.begin()->second == 1)
        return true;
    return false;
}

bool UIntPoly::is_mul() const
{
    if (poly_.size() == 1 and poly_.dict_.begin()->first != 0
        and poly_.dict_.begin()->second != 1
        and poly_.dict_.begin()->second != 0)
        return true;
    return false;
}

bool UIntPoly::is_pow() const
{
    if (poly_.size() == 1 and poly_.dict_.begin()->second == 1
        and poly_.dict_.begin()->first != 1 and poly_.dict_.begin()->first != 0)
        return true;
    return false;
}

bool divides_upoly(const UIntPoly &a, const UIntPoly &b,
                   const Ptr<RCP<const UIntPoly>> &out)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto a_poly = a.get_poly();
    auto b_poly = b.get_poly();
    if (a_poly.size() == 0)
        return false;

    map_uint_mpz res;
    UIntDict tmp;
    integer_class q, r;
    unsigned int a_deg, b_deg;

    while (b_poly.size() >= a_poly.size()) {
        a_deg = a_poly.degree();
        b_deg = b_poly.degree();

        mp_tdiv_qr(q, r, b_poly.get_lc(), a_poly.get_lc());
        if (r != 0)
            return false;

        res[b_deg - a_deg] = q;
        UIntDict tmp = UIntDict({{b_deg - a_deg, q}});
        b_poly -= (a_poly * tmp);
    }

    if (b_poly.empty()) {
        *out = UIntPoly::from_dict(a.get_var(), std::move(res));
        return true;
    } else {
        return false;
    }
}

} // SymEngine
