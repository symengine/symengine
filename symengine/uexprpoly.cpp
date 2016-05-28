#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/uexprpoly.h>
#include <symengine/pow.h>

namespace SymEngine
{

UExprPolyO::UExprPolyO(const RCP<const Symbol> &var, UExprODict &&dict)
    : UPolyBase(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

UExprPolyO::UExprPolyO(const RCP<const Symbol> &var,
                       const std::vector<Expression> &v)
    : UPolyBase(var, std::move(v))
{
    poly_.dict_ = {};
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            poly_.dict_[i] = v[i];
        }
    }
}

bool UExprPolyO::is_canonical(const UExprODict &dict) const
{
    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict.get_dict())
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t UExprPolyO::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UEXPRPOLYO;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : poly_.dict_) {
        std::size_t temp = UEXPRPOLYO;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<Basic>(temp, *(it.second.get_basic()));
        seed += temp;
    }
    return seed;
}

int UExprPolyO::compare(const Basic &o) const
{
    const UExprPolyO &s = static_cast<const UExprPolyO &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_int_Expr_compare(poly_.get_dict(), s.poly_.get_dict());
}

RCP<const UExprPolyO> UExprPolyO::from_vec(const RCP<const Symbol> &var,
                                           const std::vector<Expression> &v)
{
    return make_rcp<const UExprPolyO>(var, std::move(v));
}

RCP<const UExprPolyO> UExprPolyO::from_dict(const RCP<const Symbol> &var,
                                            UExprODict &&d)
{
    return make_rcp<const UExprPolyO>(var, std::move(d));
}

vec_basic UExprPolyO::get_args() const
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

Expression UExprPolyO::max_coef() const
{
    Expression curr = poly_.get_dict().begin()->second;
    for (const auto &it : poly_.get_dict())
        if (curr.get_basic()->__cmp__(*it.second.get_basic()))
            curr = it.second;
    return curr;
}

Expression UExprPolyO::eval(const Expression &x) const
{
    Expression ans = 0;
    for (const auto &p : poly_.get_dict()) {
        Expression temp;
        temp = pow_ex(x, Expression(p.first));
        ans += p.second * temp;
    }
    return ans;
}

bool UExprPolyO::is_zero() const
{
    return poly_.empty();
}

bool UExprPolyO::is_one() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == 1
           and poly_.get_dict().begin()->first == 0;
}

bool UExprPolyO::is_minus_one() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == -1
           and poly_.get_dict().begin()->first == 0;
}

bool UExprPolyO::is_integer() const
{
    if (poly_.empty())
        return true;
    return poly_.size() == 1 and poly_.get_dict().begin()->first == 0;
}

bool UExprPolyO::is_symbol() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->first == 1
           and poly_.get_dict().begin()->second == 1;
}

bool UExprPolyO::is_mul() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->first != 0
           and poly_.get_dict().begin()->second != 1
           and poly_.get_dict().begin()->second != 0;
}

bool UExprPolyO::is_pow() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == 1
           and poly_.get_dict().begin()->first != 1
           and poly_.get_dict().begin()->first != 0;
}

} // SymEngine
