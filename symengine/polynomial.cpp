#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/pow.h>

namespace SymEngine
{

UnivariateIntPolynomial::UnivariateIntPolynomial(const RCP<const Symbol> &var,
                                                 const unsigned int &degree,
                                                 UIntDict &&dict)
    : degree_{degree}, var_{var}, poly_{std::move(dict)}
{

    SYMENGINE_ASSERT(is_canonical(degree_, poly_))
}

UnivariateIntPolynomial::UnivariateIntPolynomial(
    const RCP<const Symbol> &var, const std::vector<integer_class> &v)
    : var_{var}
{
    poly_.dict_ = {};
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            poly_.dict_[i] = v[i];
        }
    }
    if (poly_.dict_.empty())
        degree_ = 0;
    else
        degree_ = (--poly_.dict_.end())->first;
}

bool UnivariateIntPolynomial::is_canonical(const unsigned int &degree_,
                                           const UIntDict &dict) const
{
    if (var_->get_name() == "")
        if (!(dict.empty()
              or (dict.size() == 1 and dict.dict_.begin()->first == 0)))
            return false;

    if (dict.size() != 0) {
        unsigned int actual_degree = (--dict.dict_.end())->first;
        if (actual_degree != degree_) {
            return false;
        }
    } else if (degree_ != 0)
        return false;

    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict.dict_)
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t UnivariateIntPolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEINTPOLYNOMIAL;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : poly_.dict_) {
        std::size_t temp = UNIVARIATEPOLYNOMIAL;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

bool UnivariateIntPolynomial::__eq__(const Basic &o) const
{
    return eq(*var_, *(static_cast<const UnivariateIntPolynomial &>(o).var_))
           and poly_.dict_
                   == static_cast<const UnivariateIntPolynomial &>(o)
                          .poly_.dict_;
}

int UnivariateIntPolynomial::compare(const Basic &o) const
{
    const UnivariateIntPolynomial &s
        = static_cast<const UnivariateIntPolynomial &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_mpz_compare(poly_.dict_, s.poly_.dict_);
}

RCP<const UnivariateIntPolynomial>
UnivariateIntPolynomial::from_dict(const RCP<const Symbol> &var, UIntDict &&d)
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
    unsigned int degree = 0;
    if (!d.dict_.empty())
        degree = (--(d.dict_.end()))->first;
    return make_rcp<const UnivariateIntPolynomial>(var, degree, std::move(d));
}

RCP<const UnivariateIntPolynomial>
UnivariateIntPolynomial::from_vec(const RCP<const Symbol> &var,
                                  const std::vector<integer_class> &v)
{
    return make_rcp<const UnivariateIntPolynomial>(var, std::move(v));
}

vec_basic UnivariateIntPolynomial::get_args() const
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

integer_class UnivariateIntPolynomial::max_abs_coef() const
{
    return poly_.max_abs_coef();
}

integer_class UnivariateIntPolynomial::eval(const integer_class &x) const
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

bool UnivariateIntPolynomial::is_zero() const
{
    return poly_.empty();
}

bool UnivariateIntPolynomial::is_one() const
{
    return poly_.size() == 1 and poly_.dict_.begin()->second == 1
           and poly_.dict_.begin()->first == 0;
}

bool UnivariateIntPolynomial::is_minus_one() const
{
    return poly_.size() == 1 and poly_.dict_.begin()->second == -1
           and poly_.dict_.begin()->first == 0;
}

bool UnivariateIntPolynomial::is_integer() const
{
    if (poly_.empty())
        return true;
    if (poly_.size() == 1 and poly_.dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_symbol() const
{
    if (poly_.size() == 1 and poly_.dict_.begin()->first == 1
        and poly_.dict_.begin()->second == 1)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_mul() const
{
    if (poly_.size() == 1 and poly_.dict_.begin()->first != 0
        and poly_.dict_.begin()->second != 1
        and poly_.dict_.begin()->second != 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_pow() const
{
    if (poly_.size() == 1 and poly_.dict_.begin()->second == 1
        and poly_.dict_.begin()->first != 1 and poly_.dict_.begin()->first != 0)
        return true;
    return false;
}

RCP<const UnivariateIntPolynomial> add_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b)
{
    RCP<const Symbol> var = symbol("");
    if (a.get_var()->get_name() == "") {
        var = b.get_var();
    } else if (b.get_var()->get_name() == "") {
        var = a.get_var();
    } else if (!(a.get_var()->__eq__(*b.get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a.get_var();
    }
    UIntDict dict = a.get_int_dict();
    dict += b.get_int_dict();
    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

RCP<const UnivariateIntPolynomial> neg_poly(const UnivariateIntPolynomial &a)
{
    UIntDict dict = -(a.get_int_dict());
    return UnivariateIntPolynomial::from_dict(a.get_var(), std::move(dict));
}

RCP<const UnivariateIntPolynomial> sub_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b)
{
    RCP<const Symbol> var = symbol("");
    if (a.get_var()->get_name() == "") {
        var = b.get_var();
    } else if (b.get_var()->get_name() == "") {
        var = a.get_var();
    } else if (!(a.get_var()->__eq__(*b.get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a.get_var();
    }
    UIntDict dict = a.get_int_dict();
    dict -= b.get_int_dict();
    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

RCP<const UnivariateIntPolynomial> mul_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b)
{
    RCP<const Symbol> var = symbol("");
    if (a.get_var()->get_name() == "") {
        var = b.get_var();
    } else if (b.get_var()->get_name() == "") {
        var = a.get_var();
    } else if (!(a.get_var()->__eq__(*b.get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a.get_var();
    }

    UIntDict dict = a.get_int_dict();
    dict *= b.get_int_dict();
    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

UnivariatePolynomial::UnivariatePolynomial(
    const RCP<const Symbol> &var, const int &degree,
    const UnivariateExprPolynomial &&dict)
    : degree_{degree}, var_{var}, poly_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(degree_, poly_))
}

UnivariatePolynomial::UnivariatePolynomial(const RCP<const Symbol> &var,
                                           const std::vector<Expression> &v)
    : var_{var}
{
    poly_.dict_ = {};
    unsigned int deg = 0;
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            poly_.dict_[i] = v[i];
            deg = i;
        }
    }
    if (var->get_name() == "")
        if (!(poly_.dict_.empty()
              or (poly_.dict_.size() == 1 and poly_.dict_.begin()->first == 0)))
            throw std::runtime_error("Should only have a constant term");
    degree_ = deg;
}

bool UnivariatePolynomial::is_canonical(
    const int &degree_, const UnivariateExprPolynomial &dict) const
{
    if (var_->get_name() == "")
        if (!(dict.empty()
              or (dict.size() == 1 and dict.get_dict().begin()->first == 0)))
            return false;

    if (dict.size() != 0) {
        int actual_degree = (--dict.get_dict().end())->first;
        if (actual_degree != degree_) {
            return false;
        }
    } else if (degree_ != 0)
        return false;

    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict.get_dict())
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t UnivariatePolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEPOLYNOMIAL;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : poly_.dict_) {
        std::size_t temp = UNIVARIATEPOLYNOMIAL;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<Basic>(temp, *(it.second.get_basic()));
        seed += temp;
    }
    return seed;
}

bool UnivariatePolynomial::__eq__(const Basic &o) const
{
    return eq(*var_, *(static_cast<const UnivariatePolynomial &>(o).var_))
           and (poly_.get_dict()
                == static_cast<const UnivariatePolynomial &>(o)
                       .poly_.get_dict());
}

int UnivariatePolynomial::compare(const Basic &o) const
{
    const UnivariatePolynomial &s
        = static_cast<const UnivariatePolynomial &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_int_Expr_compare(poly_.get_dict(), s.poly_.get_dict());
}

RCP<const UnivariatePolynomial>
UnivariatePolynomial::from_vec(const RCP<const Symbol> &var,
                               const std::vector<Expression> &v)
{
    return make_rcp<const UnivariatePolynomial>(var, std::move(v));
}

RCP<const UnivariatePolynomial>
UnivariatePolynomial::from_dict(const RCP<const Symbol> &var,
                                UnivariateExprPolynomial &&d)
{
    int degree = 0;
    if (!d.get_dict().empty())
        degree = (--(d.get_dict().end()))->first;
    return make_rcp<const UnivariatePolynomial>(var, degree, std::move(d));
}

vec_basic UnivariatePolynomial::get_args() const
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

Expression UnivariatePolynomial::max_coef() const
{
    Expression curr = poly_.get_dict().begin()->second;
    for (const auto &it : poly_.get_dict())
        if (curr.get_basic()->__cmp__(*it.second.get_basic()))
            curr = it.second;
    return curr;
}

Expression UnivariatePolynomial::eval(const Expression &x) const
{
    Expression ans = 0;
    for (const auto &p : poly_.get_dict()) {
        Expression temp;
        temp = pow_ex(x, Expression(p.first));
        ans += p.second * temp;
    }
    return ans;
}

bool UnivariatePolynomial::is_zero() const
{
    return poly_.empty();
}

bool UnivariatePolynomial::is_one() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == 1
           and poly_.get_dict().begin()->first == 0;
}

bool UnivariatePolynomial::is_minus_one() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == -1
           and poly_.get_dict().begin()->first == 0;
}

bool UnivariatePolynomial::is_integer() const
{
    if (poly_.empty())
        return true;
    return poly_.size() == 1 and poly_.get_dict().begin()->first == 0;
}

bool UnivariatePolynomial::is_symbol() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->first == 1
           and poly_.get_dict().begin()->second == 1;
}

bool UnivariatePolynomial::is_mul() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->first != 0
           and poly_.get_dict().begin()->second != 1
           and poly_.get_dict().begin()->second != 0;
}

bool UnivariatePolynomial::is_pow() const
{
    return poly_.size() == 1 and poly_.get_dict().begin()->second == 1
           and poly_.get_dict().begin()->first != 1
           and poly_.get_dict().begin()->first != 0;
}

RCP<const UnivariatePolynomial> add_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b)
{
    RCP<const Symbol> var = symbol("");
    if (a.get_var()->get_name() == "") {
        var = b.get_var();
    } else if (b.get_var()->get_name() == "") {
        var = a.get_var();
    } else if (!(a.get_var()->__eq__(*b.get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a.get_var();
    }
    UnivariateExprPolynomial dict = a.get_expr_dict();
    dict += b.get_expr_dict();
    return univariate_polynomial(var, std::move(dict));
}

RCP<const UnivariatePolynomial> neg_uni_poly(const UnivariatePolynomial &a)
{
    UnivariateExprPolynomial dict = -(a.get_expr_dict());
    return univariate_polynomial(a.get_var(), std::move(dict));
}

RCP<const UnivariatePolynomial> sub_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b)
{
    RCP<const Symbol> var = symbol("");
    if (a.get_var()->get_name() == "") {
        var = b.get_var();
    } else if (b.get_var()->get_name() == "") {
        var = a.get_var();
    } else if (!(a.get_var()->__eq__(*b.get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a.get_var();
    }
    UnivariateExprPolynomial dict = a.get_expr_dict();
    dict -= b.get_expr_dict();
    return univariate_polynomial(var, std::move(dict));
}

RCP<const UnivariatePolynomial> mul_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b)
{
    RCP<const Symbol> var = symbol("");
    if (a.get_var()->get_name() == "") {
        var = b.get_var();
    } else if (b.get_var()->get_name() == "") {
        var = a.get_var();
    } else if (!(a.get_var()->__eq__(*b.get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a.get_var();
    }
    UnivariateExprPolynomial dict = a.get_expr_dict();
    dict *= b.get_expr_dict();
    return univariate_polynomial(var, std::move(dict));
}

} // SymEngine
