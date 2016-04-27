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
    for (unsigned int i = 0; i < v.size(); i++)
        if (v[i] != 0)
            dict_add_term(dict_, v[i], i);
    degree_ = v.size() - 1;
    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

bool UnivariateIntPolynomial::is_canonical(const unsigned int &degree_,
                                           const map_uint_mpz &dict) const
{
    if (var_->get_name() == "")
        if (!(dict.empty() or (dict.size() == 1 and dict.begin()->first == 0)))
            return false;

    if (dict.size() != 0) {
        unsigned int prev_degree = (--dict.end())->first;
        if (prev_degree != degree_) {
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
           and map_uint_mpz_eq(
                   dict_,
                   static_cast<const UnivariateIntPolynomial &>(o).dict_);
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
        if (integer_class(0) == iter->second) {
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

void UnivariateIntPolynomial::dict_add_term(map_uint_mpz &d,
                                            const integer_class &coef,
                                            const unsigned int &n)
{
    auto it = d.find(n);
    if (it == d.end())
        d[n] = coef;
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

integer_class UnivariateIntPolynomial::eval_bit(const int &x) const
{
    unsigned int last_deg = dict_.rbegin()->first;
    integer_class result(0);

    for (auto it = dict_.rbegin(); it != dict_.rend(); ++it) {

        result = (*it).second + (result << x * (last_deg - (*it).first));
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

    bool neg = false;

    if ((--(a.get_dict().end()))->second < 0)
        neg = not neg;
    if ((--(b.get_dict().end()))->second < 0)
        neg = not neg;

    unsigned int N
        = bit_length(std::min(a.get_degree() + 1, b.get_degree() + 1))
          + bit_length(a.max_abs_coef()) + bit_length(b.max_abs_coef());

    integer_class a1(1), b1;
    a1 <<= N;
    integer_class a2 = a1 / 2;
    integer_class mask = a1 - 1;
    integer_class a_val(a.eval_bit(N)), b_val(b.eval_bit(N));
    integer_class s_val(a_val * b_val);
    integer_class r = mp_abs(s_val);

    std::vector<integer_class> v;
    integer_class carry(0);

    while (r != 0 or carry != 0) {
        mp_and(b1, r, mask);
        if (b1 < a2) {
            v.push_back(b1 + carry);
            carry = 0;
        } else {
            v.push_back(b1 - a1 + carry);
            carry = 1;
        }
        r >>= N;
    }
    if (neg)
        return neg_poly(*UnivariateIntPolynomial::from_vec(var, v));
    else
        return UnivariateIntPolynomial::from_vec(var, v);
}

UnivariatePolynomial::UnivariatePolynomial(const RCP<const Symbol> &var,
                                           const int &degree,
                                           const map_int_Expr &&dict)
    : degree_{degree}, var_{var}, dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

UnivariatePolynomial::UnivariatePolynomial(const RCP<const Symbol> &var,
                                           const std::vector<Expression> &v)
    : var_{var}
{
    for (unsigned int i = 0; i < v.size(); i++)
        if (v[i] != 0)
            dict_add_term(dict_, v[i], i);
    degree_ = v.size() - 1;
    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

bool UnivariatePolynomial::is_canonical(const int &degree_,
                                        const map_int_Expr &dict) const
{
    if (var_->get_name() == "")
        if (!(dict.empty() or (dict.size() == 1 and dict.begin()->first == 0)))
            return false;

    if (dict.size() != 0) {
        int prev_degree = (--dict.end())->first;
        if (prev_degree != degree_) {
            return false;
        }
    } else if (degree_ != 0)
        return false;

    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict)
        if (iter.first != 0 and iter.second == 0)
            return false;
    return true;
}

std::size_t UnivariatePolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEPOLYNOMIAL;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : this->dict_) {
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
           and map_int_Expr_eq(
                   dict_, static_cast<const UnivariatePolynomial &>(o).dict_);
}

int UnivariatePolynomial::compare(const Basic &o) const
{
    const UnivariatePolynomial &s
        = static_cast<const UnivariatePolynomial &>(o);

    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_int_Expr_compare(dict_, s.dict_);
}

RCP<const UnivariatePolynomial>
UnivariatePolynomial::from_vec(const RCP<const Symbol> &var,
                               const std::vector<Expression> &v)
{
    return make_rcp<const UnivariatePolynomial>(var, std::move(v));
}

RCP<const UnivariatePolynomial>
UnivariatePolynomial::from_dict(const RCP<const Symbol> &var, map_int_Expr &&d)
{
    auto iter = d.begin();
    while (iter != d.end()) {
        if (Expression(0) == iter->second) {
            auto toErase = iter;
            iter++;
            d.erase(toErase);
        } else
            iter++;
    }
    int degree = 0;
    if (!d.empty())
        degree = (--(d.end()))->first;
    return make_rcp<const UnivariatePolynomial>(var, degree, std::move(d));
}

void UnivariatePolynomial::dict_add_term(map_int_Expr &d,
                                         const Expression &coef, const int &n)
{
    auto it = d.find(n);
    if (it == d.end())
        d[n] = coef;
}

vec_basic UnivariatePolynomial::get_args() const
{
    vec_basic args;
    for (const auto &p : dict_) {
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
    if (dict_.empty())
        args.push_back(Expression(0).get_basic());
    return args;
}

Expression UnivariatePolynomial::max_coef() const
{
    Expression curr = dict_.begin()->second;
    for (const auto &it : dict_)
        if (curr.get_basic()->__cmp__(*it.second.get_basic()))
            curr = it.second;
    return curr;
}

Expression UnivariatePolynomial::eval(const Expression &x) const
{
    Expression ans = 0;
    for (const auto &p : dict_) {
        Expression temp;
        temp = pow_ex(x, Expression(p.first));
        ans += p.second * temp;
    }
    return ans;
}

bool UnivariatePolynomial::is_zero() const
{
    return dict_.empty();
}

bool UnivariatePolynomial::is_one() const
{
    return dict_.size() == 1 and dict_.begin()->second == 1
           and dict_.begin()->first == 0;
}

bool UnivariatePolynomial::is_minus_one() const
{
    return dict_.size() == 1 and dict_.begin()->second == -1
           and dict_.begin()->first == 0;
}

bool UnivariatePolynomial::is_integer() const
{
    if (dict_.empty())
        return true;
    return dict_.size() == 1 and dict_.begin()->first == 0;
}

bool UnivariatePolynomial::is_symbol() const
{
    return dict_.size() == 1 and dict_.begin()->first == 1
           and dict_.begin()->second == 1;
}

bool UnivariatePolynomial::is_mul() const
{
    return dict_.size() == 1 and dict_.begin()->first != 0
           and dict_.begin()->second != 1 and dict_.begin()->second != 0;
}

bool UnivariatePolynomial::is_pow() const
{
    return dict_.size() == 1 and dict_.begin()->second == 1
           and dict_.begin()->first != 1 and dict_.begin()->first != 0;
}

RCP<const UnivariatePolynomial> add_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b)
{
    map_int_Expr dict;
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
    return univariate_polynomial(var, std::move(dict));
}

RCP<const UnivariatePolynomial> neg_uni_poly(const UnivariatePolynomial &a)
{
    map_int_Expr dict;
    for (const auto &it : a.get_dict())
        dict[it.first] = -1 * it.second;
    return univariate_polynomial(a.get_var(), std::move(dict));
}

RCP<const UnivariatePolynomial> sub_uni_poly(const UnivariatePolynomial &a,
                                             const UnivariatePolynomial &b)
{
    map_int_Expr dict;
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
    return univariate_polynomial(var, std::move(dict));
}

RCP<const UnivariatePolynomial> mul_uni_poly(RCP<const UnivariatePolynomial> a,
                                             RCP<const UnivariatePolynomial> b)
{
    map_int_Expr dict;
    RCP<const Symbol> var = symbol("");
    if (a->get_var()->get_name() == "") {
        var = b->get_var();
    } else if (b->get_var()->get_name() == "") {
        var = a->get_var();
    } else if (!(a->get_var()->__eq__(*b->get_var()))) {
        throw std::runtime_error("Error: variables must agree.");
    } else {
        var = a->get_var();
    }

    if (a->get_dict().empty() and b->get_dict().empty())
        return univariate_polynomial(var, {{0, 0}});

    for (const auto &i1 : a->get_dict())
        for (const auto &i2 : b->get_dict())
            dict[i1.first + i2.first] += i1.second * i2.second;
    return univariate_polynomial(var, std::move(dict));
}

/// Multivariate Polynomial///

MultivariateIntPolynomial::MultivariateIntPolynomial(const set_sym &vars,
                                                     umap_sym_uint &degrees,
                                                     umap_uvec_mpz &dict)
    : vars_{std::move(vars)}, degrees_{std::move(degrees)},
      dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(vars_, degrees_, dict_))
}

RCP<const MultivariateIntPolynomial>
MultivariateIntPolynomial::from_dict(const set_sym &s, umap_uvec_mpz &&d)
{
    umap_sym_uint degs;
    auto iter = d.begin();
    while (iter != d.end()) {
        if (integer_class(0) == iter->second) {
            auto toErase = iter;
            iter++;
            d.erase(toErase);
        } else {
            iter++;
        }
    }

    int whichvar = 0;
    for (auto sym : s) {
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(sym, 0));
        for (auto bucket : d) {
            if (bucket.first[whichvar] > degs.find(sym)->second)
                degs.find(sym)->second = bucket.first[whichvar];
        }
        whichvar++;
    }
    return make_rcp<const MultivariateIntPolynomial>(s, degs, d);
}

vec_basic MultivariateIntPolynomial::get_args() const
{
    vec_basic args;
    umap_uvec_mpz d;
    // To change the ordering in which the terms appear in the vector, change
    // vec_uint_compare in dict.h
    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(dict_);
    for (const auto &p : v) {
        map_basic_basic b;
        int whichvar = 0;
        for (auto sym : vars_) {
            if (integer_class(0) != p[whichvar])
                b.insert(std::pair<RCP<const Basic>, RCP<const Basic>>(
                    sym, make_rcp<Integer>(integer_class(p[whichvar]))));
            whichvar++;
        }
        args.push_back(Mul::from_dict(
            make_rcp<const Integer>(dict_.find(p)->second), std::move(b)));
    }
    return args;
}

bool MultivariateIntPolynomial::is_canonical(const set_sym &vars,
                                             const umap_sym_uint &degrees,
                                             const umap_uvec_mpz &dict)
{
    // checks that the dictionary does not contain terms with coefficient 0
    for (auto bucket : dict) {
        if (integer_class(0) == bucket.second)
            return false;
    }

    // checks that the maximum degree of any variable is correct according to
    // the dictionary
    unsigned int whichvar
        = 0; // keeps track of the index of the variable we are checking
    for (auto var : vars) {
        unsigned int maxdegree = 0;
        for (auto bucket : dict) {
            if (bucket.first[whichvar] > degrees.find(var)->second)
                return false;
            else if (maxdegree < bucket.first[whichvar])
                maxdegree = bucket.first[whichvar];
        }
        if (maxdegree != degrees.find(var)->second)
            return false;
        whichvar++;
    }
    return true;
}

std::size_t MultivariateIntPolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = MULTIVARIATEINTPOLYNOMIAL;
    for (auto var : vars_)
        // boost's method for combining hashes
        seed ^= hash_string(var->get_name()) + 0x9e3779b + (seed << 6)
                + (seed >> 2);

    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(dict_);

    for (auto vec : v) {
        seed ^= vec_uint_hash()(dict_.find(vec)->first) + 0x9e3779b
                + (seed << 6) + (seed >> 2);
        seed ^= mpz_hash(dict_.find(vec)->second) + 0x9e3779b + (seed << 6)
                + (seed >> 2);
    }
    return seed;
}

bool MultivariateIntPolynomial::__eq__(const Basic &o) const
{
    return (
        set_eq<set_sym>(vars_,
                        static_cast<const MultivariateIntPolynomial &>(o).vars_)
        && umap_uvec_mpz_eq(
               dict_, static_cast<const MultivariateIntPolynomial &>(o).dict_));
}

int MultivariateIntPolynomial::compare(const Basic &o) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    const MultivariateIntPolynomial &s
        = static_cast<const MultivariateIntPolynomial &>(o);

    int cmp = set_compare<set_sym>(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return umap_uvec_mpz_compare(dict_, s.dict_);
}

integer_class MultivariateIntPolynomial::eval(
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> &vals) const
{
    integer_class ans(0);
    for (auto bucket : dict_) {
        integer_class term = bucket.second;
        unsigned int whichvar = 0;
        for (auto sym : vars_) {
            integer_class temp;
            mp_pow_ui(temp, vals.find(sym)->second, bucket.first[whichvar]);
            term *= temp;
            whichvar++;
        }
        ans += term;
    }
    return ans;
}

std::string MultivariateIntPolynomial::toString() const
{
    std::ostringstream s;
    bool first = true; // is this the first term being printed out?
    // To change the ordering in which the terms will print out, change
    // vec_uint_compare in dict.h
    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(dict_);

    for (vec_uint exps : v) {
        integer_class c = dict_.find(exps)->second;
        if (c != 0) {
            if (c > 0 && !first) {
                s << "+ ";
            } else if (c < 0) {
                s << "- ";
            }
            unsigned int i = 0;
            std::ostringstream expr;
            for (auto it : vars_) {
                if (dict_.find(exps)->first[i] != 0) {
                    expr << it->get_name();
                    if (dict_.find(exps)->first[i] > 1)
                        expr << "**" << dict_.find(exps)->first[i];
                    expr << " ";
                }
                i++;
            }
            if (mp_abs(c) != 1 || expr.str().empty())
                s << mp_abs(c) << "*";
            s << expr.str();
            first = false;
        }
    }

    if (s.str().empty())
        s << "0 ";
    std::string final(s.str());
    final.pop_back();
    return final;
}

unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_sym &s,
                       const set_sym &s1, const set_sym &s2)
{
    auto a1 = s1.begin();
    auto a2 = s2.begin();
    unsigned int poscount = 0;
    while (a1 != s1.end() && a2 != s2.end()) {
        if (0 == (*a1)->compare(**a2) && (a1 != s1.end() && a2 != s2.end())) {
            v1.insert(v1.end(), poscount);
            v2.insert(v2.end(), poscount);
            s.insert(*a1);
            a1++;
            a2++;
        } else if (-1 == (*a1)->compare(**a2)) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
        } else if (1 == (*a1)->compare(**a2)) {
            v2.insert(v2.end(), poscount);
            s.insert(*a2);
            a2++;
        }
        poscount++;
    }
    if (a1 == s1.end()) {
        while (a2 != s2.end()) {
            v2.insert(v2.end(), poscount);
            s.insert(*a2);
            a2++;
            poscount++;
        }
    } else if (a2 == s2.end()) {
        while (a1 != s1.end()) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
            poscount++;
        }
    }
    return poscount; // return size of the new vectors
}

unsigned int reconcile(vec_uint &v1, unsigned int &v2, set_sym &s,
                       const set_sym &s1, const RCP<const Symbol> s2)
{
    auto a1 = s1.begin();
    unsigned int poscount = 0;
    bool inserted = false;
    while (a1 != s1.end() && !inserted) {
        if (0 == (*a1)->compare(*s2)) {
            v1.insert(v1.end(), poscount);
            v2 = poscount;
            s.insert(*a1);
            a1++;
            inserted = true;
        } else if (-1 == (*a1)->compare(*s2)) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
        } else if (1 == (*a1)->compare(*s2)) {
            v2 = poscount;
            s.insert(s2);
            inserted = true;
        }
        poscount++;
    }
    if (a1 == s1.end() && !inserted) {
        v2 = poscount;
        s.insert(s2);
        poscount++;
    } else if (inserted) {
        while (a1 != s1.end()) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
            poscount++;
        }
    }
    return poscount;
}

vec_uint translate(vec_uint original, vec_uint translator, unsigned int size)
{
    vec_uint changed;
    changed.resize(size, 0);
    for (unsigned int i = 0; i < original.size(); i++) {
        changed[translator[i]] = original[i];
    }
    return changed;
}

vec_uint translate(unsigned int original, unsigned int translator,
                   unsigned int size)
{
    vec_uint changed;
    changed.resize(size, 0);
    changed[translator] = original;
    return changed;
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.vars_);
    for (auto bucket : a.dict_) {
        dict.insert(std::pair<vec_uint, integer_class>(
            translate(bucket.first, v1, size), bucket.second));
    }
    for (auto bucket : b.dict_) {
        auto target = dict.find(translate(bucket.first, v2, size));
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, integer_class>(
                translate(bucket.first, v2, size), bucket.second));
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

// translates two vec_uints to the desired format and adds them together
// componentwise
vec_uint uint_vec_translate_and_add(const vec_uint &v1, const vec_uint &v2,
                                    const vec_uint &translator1,
                                    const vec_uint &translator2,
                                    const unsigned int size)
{
    vec_uint result;
    for (unsigned int i = 0; i < size; i++) {
        result.insert(result.end(), 0);
    }
    for (unsigned int i = 0; i < translator1.size(); i++) {
        result[translator1[i]] += v1[i];
    }
    for (unsigned int i = 0; i < translator2.size(); i++) {
        result[translator2[i]] += v2[i];
    }
    return result;
}

vec_uint uint_vec_translate_and_add(const vec_uint &v1, const unsigned int v2,
                                    const vec_uint &translator1,
                                    const unsigned int &translator2,
                                    const unsigned int size)
{
    vec_uint result;
    for (unsigned int i = 0; i < size; i++) {
        result.insert(result.end(), 0);
    }
    for (unsigned int i = 0; i < translator1.size(); i++) {
        result[translator1[i]] += v1[i];
    }
    result[translator2] += v2;
    return result;
}

RCP<const MultivariateIntPolynomial>
neg_mult_poly(const MultivariateIntPolynomial &a)
{
    umap_uvec_mpz dict;
    set_sym s = a.vars_;
    for (auto bucket : a.dict_) {
        dict.insert(
            std::pair<vec_uint, integer_class>(bucket.first, -bucket.second));
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return (add_mult_poly(a, *neg_mult_poly(b)));
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_mpz dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.vars_);
    for (auto a_bucket : a.dict_) {
        for (auto b_bucket : b.dict_) {
            vec_uint target = uint_vec_translate_and_add(
                a_bucket.first, b_bucket.first, v1, v2, size);
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, integer_class>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.get_var());
    for (auto bucket : a.dict_) {
        dict.insert(std::pair<vec_uint, integer_class>(
            translate(bucket.first, v1, size), bucket.second));
    }
    for (auto bucket : b.get_dict()) {
        auto target = dict.find(translate(bucket.first, v2, size));
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, integer_class>(
                translate(bucket.first, v2, size), bucket.second));
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return add_mult_poly(b, a);
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    return add_mult_poly(a, *neg_poly(b));
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return add_mult_poly(*neg_mult_poly(b), a);
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.get_var());
    for (auto a_bucket : a.dict_) {
        for (auto b_bucket : b.get_dict()) {
            vec_uint target = uint_vec_translate_and_add(
                a_bucket.first, b_bucket.first, v1, v2, size);
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, integer_class>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return mul_mult_poly(b, a);
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    bool same = false; // are the variables of a and b the same?
    if (0 == a.get_var()->compare(*b.get_var())) {
        v1 = 0;
        v2 = 0;
        s.insert(a.get_var());
        same = true;
    } else {
        if (-1 == a.get_var()->compare(*b.get_var())) {
            v1 = 0;
            v2 = 1;
        } else {
            v2 = 0;
            v1 = 1;
        }
        s.insert(a.get_var());
        s.insert(b.get_var());
    }

    for (auto bucket : a.get_dict()) {
        vec_uint v;
        v.insert(v.end(), 0);
        if (!same)
            v.insert(v.end(), 0);
        v[v1] = bucket.first;
        dict.insert(std::pair<vec_uint, integer_class>(v, bucket.second));
    }
    for (auto bucket : b.get_dict()) {
        vec_uint v;
        v.insert(v.end(), 0);
        if (!same)
            v.insert(v.end(), 0);
        v[v2] = bucket.first;
        auto target = dict.find(v);
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, integer_class>(v, bucket.second));
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    return add_mult_poly(a, *neg_poly(b));
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    bool same = false; // are the variables of a and b the same?
    if (0 == a.get_var()->compare(*b.get_var())) {
        v1 = 0;
        v2 = 0;
        s.insert(a.get_var());
        same = true;
    } else {
        if (-1 == a.get_var()->compare(*b.get_var())) {
            v1 = 0;
            v2 = 1;
        } else {
            v2 = 0;
            v1 = 1;
        }
        s.insert(a.get_var());
        s.insert(b.get_var());
    }
    for (auto a_bucket : a.get_dict()) {
        for (auto b_bucket : b.get_dict()) {
            vec_uint target;
            target.insert(target.end(), 0);
            if (!same)
                target.insert(target.end(), 0);
            target[v1] += a_bucket.first;
            target[v2] += b_bucket.first;
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, integer_class>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

} // SymEngine
