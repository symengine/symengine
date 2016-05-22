#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/pow.h>

namespace SymEngine
{

UnivariateIntPolynomial::UnivariateIntPolynomial(const RCP<const Symbol> &var,
                                                 const unsigned int &degree,
                                                 map_uint_mpz &&dict)
    : degree_{degree}, var_{var}, dict_{std::move(dict)}
{

    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

UnivariateIntPolynomial::UnivariateIntPolynomial(
    const RCP<const Symbol> &var, const std::vector<integer_class> &v)
    : var_{var}
{
    dict_ = {};
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            dict_[i] = v[i];
        }
    }
    if (dict_.empty())
        degree_ = 0;
    else
        degree_ = (--dict_.end())->first;
}

bool UnivariateIntPolynomial::is_canonical(const unsigned int &degree_,
                                           const map_uint_mpz &dict) const
{
    if (var_->get_name() == "")
        if (!(dict.empty() or (dict.size() == 1 and dict.begin()->first == 0)))
            return false;

    if (dict.size() != 0) {
        unsigned int actual_degree = (--dict.end())->first;
        if (actual_degree != degree_) {
            return false;
        }
    } else if (degree_ != 0)
        return false;

    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict)
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t UnivariateIntPolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEINTPOLYNOMIAL;
    seed += hash_string(this->var_->get_name());
    for (const auto &it : this->dict_) {
        std::size_t temp = UNIVARIATEINTPOLYNOMIAL;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

bool UnivariateIntPolynomial::__eq__(const Basic &o) const
{
    return eq(*var_, *(static_cast<const UnivariateIntPolynomial &>(o).var_))
           and dict_ == static_cast<const UnivariateIntPolynomial &>(o).dict_;
}

int UnivariateIntPolynomial::compare(const Basic &o) const
{
    const UnivariateIntPolynomial &s
        = static_cast<const UnivariateIntPolynomial &>(o);

    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_mpz_compare(dict_, s.dict_);
}

RCP<const UnivariateIntPolynomial>
UnivariateIntPolynomial::from_dict(const RCP<const Symbol> &var,
                                   map_uint_mpz &&d)
{
    auto iter = d.begin();
    while (iter != d.end()) {
        if (iter->second == 0) {
            auto toErase = iter;
            iter++;
            d.erase(toErase);
        } else {
            iter++;
        }
    }
    unsigned int degree = 0;
    if (!d.empty())
        degree = (--(d.end()))->first;
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
    for (const auto &p : dict_) {
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
    if (dict_.empty())
        args.push_back(zero);
    return args;
}

integer_class UnivariateIntPolynomial::max_abs_coef() const
{
    integer_class curr(mp_abs(dict_.begin()->second));
    for (const auto &it : dict_) {
        if (mp_abs(it.second) > curr)
            curr = mp_abs(it.second);
    }
    return curr;
}

integer_class UnivariateIntPolynomial::eval(const integer_class &x) const
{
    unsigned int last_deg = dict_.rbegin()->first;
    integer_class result(0), x_pow;

    for (auto it = dict_.rbegin(); it != dict_.rend(); ++it) {

        mp_pow_ui(x_pow, x, last_deg - (*it).first);
        last_deg = (*it).first;
        result = (*it).second + x_pow * result;
    }
    mp_pow_ui(x_pow, x, last_deg);
    result *= x_pow;

    return result;
}

integer_class UnivariateIntPolynomial::eval_bit(const int &x, bool neg) const
{
    unsigned int last_deg = dict_.rbegin()->first;
    integer_class result(0);
    int mul;

    for (auto it = dict_.rbegin(); it != dict_.rend(); ++it) {

        mul = ((*it).first % 2 == 0 or not neg) ? 1 : -1;
        result = (*it).second * mul + (result << x * (last_deg - (*it).first));
        last_deg = (*it).first;
    }
    result = result << x * last_deg;

    return result;
}

bool UnivariateIntPolynomial::is_zero() const
{
    return dict_.empty();
}

bool UnivariateIntPolynomial::is_one() const
{
    return dict_.size() == 1 and dict_.begin()->second == 1
           and dict_.begin()->first == 0;
}

bool UnivariateIntPolynomial::is_minus_one() const
{
    return dict_.size() == 1 and dict_.begin()->second == -1
           and dict_.begin()->first == 0;
}

bool UnivariateIntPolynomial::is_integer() const
{
    if (dict_.empty())
        return true;
    if (dict_.size() == 1 and dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_symbol() const
{
    if (dict_.size() == 1 and dict_.begin()->first == 1
        and dict_.begin()->second == 1)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_mul() const
{
    if (dict_.size() == 1 and dict_.begin()->first != 0
        and dict_.begin()->second != 1 and dict_.begin()->second != 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_pow() const
{
    if (dict_.size() == 1 and dict_.begin()->second == 1
        and dict_.begin()->first != 1 and dict_.begin()->first != 0)
        return true;
    return false;
}

RCP<const UnivariateIntPolynomial> add_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b)
{
    map_uint_mpz dict;
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
    for (const auto &it : a.get_dict())
        dict[it.first] = it.second;
    for (const auto &it : b.get_dict())
        dict[it.first] += it.second;
    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

RCP<const UnivariateIntPolynomial> neg_poly(const UnivariateIntPolynomial &a)
{
    map_uint_mpz dict;
    for (const auto &it : a.get_dict())
        dict[it.first] = -1 * it.second;
    return UnivariateIntPolynomial::from_dict(a.get_var(), std::move(dict));
}

RCP<const UnivariateIntPolynomial> sub_poly(const UnivariateIntPolynomial &a,
                                            const UnivariateIntPolynomial &b)
{
    map_uint_mpz dict;
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
    for (const auto &it : a.get_dict())
        dict[it.first] = it.second;
    for (const auto &it : b.get_dict())
        dict[it.first] -= it.second;
    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

// Calculates bit length of number, used in mul_poly() only
template <typename T>
unsigned int bit_length(T t)
{
    unsigned int count = 0;
    while (t > 0) {
        count++;
        t = t >> 1;
    }
    return count;
}

// Reference for the KS algorithms : http://arxiv.org/pdf/0712.4046v1.pdf
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

    int mul = 1;

    unsigned int N
        = bit_length(std::min(a.get_degree() + 1, b.get_degree() + 1))
          + bit_length(a.max_abs_coef()) + bit_length(b.max_abs_coef());

    integer_class full = integer_class(1), temp, res;
    full <<= N;
    integer_class thresh = full / 2;
    integer_class mask = full - 1;
    integer_class s_val = a.eval_bit(N) * b.eval_bit(N);
    if (s_val < 0)
        mul = -1;
    s_val = mp_abs(s_val);

    unsigned int deg = 0, carry = 0;
    map_uint_mpz dict;

    while (s_val != 0 or carry != 0) {
        mp_and(temp, s_val, mask);
        if (temp < thresh) {
            res = mul * (temp + carry);
            if (res != 0)
                dict[deg] = res;
            carry = 0;
        } else {
            res = mul * (temp - full + carry);
            if (res != 0)
                dict[deg] = res;
            carry = 1;
        }
        s_val >>= N;
        deg++;
    }

    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

RCP<const UnivariateIntPolynomial>
mul_poly_ks2(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b)
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

    bool parity = true;
    int mul_o = 1, mul_e = 1;

    unsigned int N
        = (bit_length(std::min(a.get_degree() + 1, b.get_degree() + 1))
           + bit_length(a.max_abs_coef()) + bit_length(b.max_abs_coef()) + 1)
          / 2;

    integer_class full = integer_class(1), temp, res;
    full <<= (2 * N);
    integer_class thresh = full / 2;
    integer_class mask = full - 1;
    integer_class s_val_p = a.eval_bit(N) * b.eval_bit(N);
    integer_class s_val_n = a.eval_bit(N, true) * b.eval_bit(N, true);

    integer_class he = s_val_p + s_val_n;
    if (he < 0)
        mul_e = -1;
    he = mp_abs(he);
    he = he / 2;
    integer_class ho = s_val_p - s_val_n;
    if (ho < 0)
        mul_o = -1;
    ho = mp_abs(ho);
    ho = ho / 2;
    ho = ho >> N;

    unsigned int carrye = 0, carryo = 0, deg = 0;
    map_uint_mpz dict;

    while (he != 0 or carrye != 0 or ho != 0 or carryo != 0) {

        if (parity) {

            mp_and(temp, he, mask);
            if (temp < thresh) {
                res = mul_e * (temp + carrye);
                if (res != 0)
                    dict[deg] = res;
                carrye = 0;
            } else {
                res = mul_e * (temp - full + carrye);
                if (res != 0)
                    dict[deg] = res;
                carrye = 1;
            }
            he >>= (2 * N);

        } else {

            mp_and(temp, ho, mask);
            if (temp < thresh) {
                res = mul_o * (temp + carryo);
                if (res != 0)
                    dict[deg] = res;
                carryo = 0;
            } else {
                res = mul_o * (temp - full + carryo);
                if (res != 0)
                    dict[deg] = res;
                carryo = 1;
            }
            ho >>= (2 * N);
        }
        deg++;
        parity = not parity;
    }
    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

RCP<const UnivariateIntPolynomial>
mul_poly_naive(const UnivariateIntPolynomial &a,
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

    integer_class mul;
    int deg;

    map_uint_mpz dict;
    for (auto a_iter : a.get_dict()) {
        for (auto b_iter : b.get_dict()) {

            deg = a_iter.first + b_iter.first;
            mul = a_iter.second * b_iter.second;
            auto ite = dict.find(deg);

            if (ite == dict.end()) {
                dict[deg] = mul;
            } else {
                dict[deg] += mul;
            }
        }
    }

    return UnivariateIntPolynomial::from_dict(var, std::move(dict));
}

UnivariatePolynomial::UnivariatePolynomial(
    const RCP<const Symbol> &var, const int &degree,
    const UnivariateExprPolynomial &&dict)
    : degree_{degree}, var_{var}, expr_dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(degree_, expr_dict_))
}

UnivariatePolynomial::UnivariatePolynomial(const RCP<const Symbol> &var,
                                           const std::vector<Expression> &v)
    : var_{var}
{
    expr_dict_.dict_ = {};
    unsigned int deg = 0;
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            expr_dict_.dict_[i] = v[i];
            deg = i;
        }
    }
    if (var->get_name() == "")
        if (!(expr_dict_.dict_.empty()
              or (expr_dict_.dict_.size() == 1
                  and expr_dict_.dict_.begin()->first == 0)))
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
    seed += expr_dict_.__hash__();
    return seed;
}

bool UnivariatePolynomial::__eq__(const Basic &o) const
{
    return eq(*var_, *(static_cast<const UnivariatePolynomial &>(o).var_))
           and (expr_dict_.get_dict()
                == static_cast<const UnivariatePolynomial &>(o)
                       .expr_dict_.get_dict());
}

int UnivariatePolynomial::compare(const Basic &o) const
{
    const UnivariatePolynomial &s
        = static_cast<const UnivariatePolynomial &>(o);

    if (expr_dict_.size() != s.expr_dict_.size())
        return (expr_dict_.size() < s.expr_dict_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_int_Expr_compare(expr_dict_.get_dict(), s.expr_dict_.get_dict());
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
    for (const auto &p : expr_dict_.get_dict()) {
        if (p.first == 0)
            args.push_back(p.second.get_basic());
        else if (p.first == 1) {
            if (p.second == Expression(1))
                args.push_back(var_);
            else
                args.push_back(Mul::from_dict(
                    rcp_static_cast<const Integer>(p.second.get_basic()),
                    {{var_, one}}));
        } else if (p.second == 1)
            args.push_back(pow(var_, integer(p.first)));
        else
            args.push_back(Mul::from_dict(
                rcp_static_cast<const Integer>(p.second.get_basic()),
                {{var_, integer(p.first)}}));
    }
    if (expr_dict_.empty())
        args.push_back(Expression(0).get_basic());
    return args;
}

Expression UnivariatePolynomial::max_coef() const
{
    Expression curr = expr_dict_.get_dict().begin()->second;
    for (const auto &it : expr_dict_.get_dict())
        if (curr.get_basic()->__cmp__(*it.second.get_basic()))
            curr = it.second;
    return curr;
}

Expression UnivariatePolynomial::eval(const Expression &x) const
{
    Expression ans = 0;
    for (const auto &p : expr_dict_.get_dict()) {
        Expression temp;
        temp = pow_ex(x, Expression(p.first));
        ans += p.second * temp;
    }
    return ans;
}

bool UnivariatePolynomial::is_zero() const
{
    return expr_dict_.empty();
}

bool UnivariatePolynomial::is_one() const
{
    return expr_dict_.size() == 1 and expr_dict_.get_dict().begin()->second == 1
           and expr_dict_.get_dict().begin()->first == 0;
}

bool UnivariatePolynomial::is_minus_one() const
{
    return expr_dict_.size() == 1
           and expr_dict_.get_dict().begin()->second == -1
           and expr_dict_.get_dict().begin()->first == 0;
}

bool UnivariatePolynomial::is_integer() const
{
    if (expr_dict_.empty())
        return true;
    return expr_dict_.size() == 1 and expr_dict_.get_dict().begin()->first == 0;
}

bool UnivariatePolynomial::is_symbol() const
{
    return expr_dict_.size() == 1 and expr_dict_.get_dict().begin()->first == 1
           and expr_dict_.get_dict().begin()->second == 1;
}

bool UnivariatePolynomial::is_mul() const
{
    return expr_dict_.size() == 1 and expr_dict_.get_dict().begin()->first != 0
           and expr_dict_.get_dict().begin()->second != 1
           and expr_dict_.get_dict().begin()->second != 0;
}

bool UnivariatePolynomial::is_pow() const
{
    return expr_dict_.size() == 1 and expr_dict_.get_dict().begin()->second == 1
           and expr_dict_.get_dict().begin()->first != 1
           and expr_dict_.get_dict().begin()->first != 0;
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
