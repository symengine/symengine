#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polys/uexprpoly.h>
#include <symengine/pow.h>

namespace SymEngine
{

UExprPoly::UExprPoly(const RCP<const Symbol> &var, UExprDict &&dict)
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
    std::hash<std::string> hash_string;
    std::size_t seed = UEXPRPOLY;

    seed += hash_string(var_->get_name());
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

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_int_Expr_compare(poly_.get_dict(), s.poly_.get_dict());
}

RCP<const UExprPoly> UExprPoly::from_dict(const RCP<const Symbol> &var,
                                          UExprDict &&d)
{
    return make_rcp<const UExprPoly>(var, std::move(d));
}

RCP<const UExprPoly> UExprPoly::from_vec(const RCP<const Symbol> &var,
                                         const std::vector<Expression> &v)
{
    return make_rcp<const UExprPoly>(var, UExprDict::from_vec(v));
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
