#include <symengine/polys/basic_to_poly.h>

namespace SymEngine
{

UExprPoly::UExprPoly(const RCP<const Basic> &var, UExprDict &&dict)
    : UPolyBase(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

bool UExprPoly::is_canonical(const UExprDict &dict) const
{
    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict.get_dict())
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t UExprPoly::__hash__() const
{
    std::size_t seed = UEXPRPOLY;

    seed += var_->hash();
    for (const auto &it : poly_.dict_) {
        std::size_t temp = UEXPRPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<Basic>(temp, *(it.second.get_basic()));
        seed += temp;
    }
    return seed;
}

int UExprPoly::compare(const Basic &o) const
{
    const UExprPoly &s = static_cast<const UExprPoly &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = unified_compare(var_, s.var_);
    if (cmp != 0)
        return cmp;

    return unified_compare(poly_.get_dict(), s.poly_.get_dict());
}

RCP<const UExprPoly> UExprPoly::from_dict(const RCP<const Basic> &var,
                                          map_int_Expr &&d)
{
    UExprDict x(d);
    return make_rcp<const UExprPoly>(var, std::move(x));
}

RCP<const UExprPoly> UExprPoly::from_vec(const RCP<const Basic> &var,
                                         const std::vector<Expression> &v)
{
    return make_rcp<const UExprPoly>(var, UExprDict::from_vec(v));
}

RCP<const UExprPoly> UExprPoly::from_basic(const RCP<const Basic> &basic,
                                         const RCP<const Basic> &gen)
{
    return UExprPoly::from_container(gen, _b_to_upoly<UExprDict>(basic, gen));
}

RCP<const UExprPoly> UExprPoly::from_basic(const RCP<const Basic> &basic)
{
    umap_basic_num tmp = _find_gens_poly(basic);

    if (tmp.size() != 1)
        throw std::runtime_error("Did not find exactly 1 generator");

    RCP<const Basic> gen = pow(tmp.begin()->first, tmp.begin()->second);
    return UExprPoly::from_container(gen, _b_to_upoly<UExprDict>(basic, gen));
}

vec_basic UExprPoly::get_args() const
{
    vec_basic args;
    for (const auto &p : poly_.get_dict()) {
        if (p.first == 0)
            args.push_back(p.second.get_basic());
        else if (p.first == 1) {
            if (p.second == Expression(1))
                args.push_back(var_);
            else
                args.push_back(mul(p.second.get_basic(), var_));
        } else if (p.second == 1)
            args.push_back(pow(var_, integer(p.first)));
        else
            args.push_back(
                mul(p.second.get_basic(), pow(var_, integer(p.first))));
    }
    if (poly_.empty())
        args.push_back(Expression(0).get_basic());
    return args;
}

Expression UExprPoly::max_coef() const
{
    Expression curr = poly_.get_dict().begin()->second;
    for (const auto &it : poly_.get_dict())
        if (curr.get_basic()->__cmp__(*it.second.get_basic()))
            curr = it.second;
    return curr;
}

Expression UExprPoly::eval(const Expression &x) const
{
    Expression ans = 0;
    for (const auto &p : poly_.get_dict()) {
        Expression temp;
        temp = pow_ex(x, Expression(p.first));
        ans += p.second * temp;
    }
    return ans;
}

bool UExprPoly::is_zero() const
{
    return poly_.empty();
}

bool UExprPoly::is_one() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == 1
           and poly_.get_dict().begin()->first == 0;
}

bool UExprPoly::is_minus_one() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == -1
           and poly_.get_dict().begin()->first == 0;
}

bool UExprPoly::is_integer() const
{
    if (poly_.empty())
        return true;
    return poly_.size() == 1 and poly_.get_dict().begin()->first == 0;
}

bool UExprPoly::is_symbol() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->first == 1
           and poly_.get_dict().begin()->second == 1;
}

bool UExprPoly::is_mul() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->first != 0
           and poly_.get_dict().begin()->second != 1
           and poly_.get_dict().begin()->second != 0;
}

bool UExprPoly::is_pow() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == 1
           and poly_.get_dict().begin()->first != 1
           and poly_.get_dict().begin()->first != 0;
}

} // SymEngine
