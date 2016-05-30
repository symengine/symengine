#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/pow.h>

namespace SymEngine
{

UIntPoly::UIntPoly(const RCP<const Symbol> &var, UIntDict &&dict)
    : UPolyBase(var, std::move(dict))
{

    SYMENGINE_ASSERT(is_canonical(poly_))
}

UIntPoly::UIntPoly(const RCP<const Symbol> &var,
                   const std::vector<integer_class> &v)
    : UPolyBase(var, std::move(v))
{
    poly_.dict_ = {};
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            poly_.dict_[i] = v[i];
        }
    }
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
    std::hash<std::string> hash_string;
    std::size_t seed = UINTPOLY;

    seed += hash_string(this->var_->get_name());
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

RCP<const UIntPoly> UIntPoly::from_dict(const RCP<const Symbol> &var,
                                        UIntDict &&d)
{
    auto iter = d.dict_.begin();
    while (iter != d.dict_.end()) {
        if (iter->second == 0) {
            auto toErase = iter;
            iter++;
            d.dict_.erase(toErase);
        } else {
            iter++;
        }
    }
    return make_rcp<const UIntPoly>(var, std::move(d));
}

RCP<const UIntPoly> UIntPoly::from_vec(const RCP<const Symbol> &var,
                                       const std::vector<integer_class> &v)
{
    return make_rcp<const UIntPoly>(var, std::move(v));
}

vec_basic UIntPoly::get_args() const
{
    vec_basic args;
    for (const auto &p : poly_.dict_) {
        if (p.first == 0) {
            args.push_back(integer(p.second));
        } else if (p.first == 1) {
            if (p.second == 1) {
                args.push_back(var_);
            } else {
                args.push_back(
                    Mul::from_dict(integer(p.second), {{var_, one}}));
            }
        } else {
            if (p.second == 1) {
                args.push_back(pow(var_, integer(p.first)));
            } else {
                args.push_back(Mul::from_dict(integer(p.second),
                                              {{var_, integer(p.first)}}));
            }
        }
    }
    if (poly_.dict_.empty())
        args.push_back(zero);
    return args;
}

integer_class UIntPoly::max_abs_coef() const
{
    return poly_.max_abs_coef();
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

} // SymEngine
