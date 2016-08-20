#include <symengine/visitor.h>

namespace SymEngine
{

UExprPoly::UExprPoly(const RCP<const Basic> &var, UExprDict &&dict)
    : USymEnginePoly(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

hash_t UExprPoly::__hash__() const
{
    hash_t seed = UEXPRPOLY;

    seed += var_->hash();
    for (const auto &it : poly_.dict_) {
        hash_t temp = UEXPRPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<Basic>(temp, *(it.second.get_basic()));
        seed += temp;
    }
    return seed;
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
