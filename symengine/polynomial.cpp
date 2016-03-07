#include <symengine/polynomial.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/constants.h>

namespace SymEngine {

UnivariateIntPolynomial::UnivariateIntPolynomial(const RCP<const Symbol> &var, const unsigned int &degree, map_uint_mpz&& dict) :
     degree_{degree}, var_{var}, dict_{std::move(dict)} {

    SYMENGINE_ASSERT(is_canonical(degree_, dict_))
}

bool UnivariateIntPolynomial::is_canonical(const unsigned int &degree_, const map_uint_mpz& dict) const
{
    if (dict.size() != 0) {
        unsigned int prev_degree = (--dict.end())->first;
        if (prev_degree != degree_)
            return false;
    } else if (0 != degree_) {
        return false;
    }
    //Check if dictionary contains terms with coeffienct 0
    for (auto itter = dict.begin(); itter != dict.end(); itter++) {
        if (0 == itter->second)
            return false;
    }
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
    if (eq(*var_, *(static_cast<const UnivariateIntPolynomial &>(o).var_)) and
        map_uint_mpz_eq(dict_, static_cast<const UnivariateIntPolynomial &>(o).dict_))
        return true;
    return false;
}

int UnivariateIntPolynomial::compare(const Basic &o) const
{
    const UnivariateIntPolynomial &s = static_cast<const UnivariateIntPolynomial &>(o);

    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_mpz_compare(dict_, s.dict_);
}

RCP<const UnivariateIntPolynomial> UnivariateIntPolynomial::from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d)
{
    auto itter = d.begin();
    while (itter != d.end()) {
        if (integer_class(0) == itter->second) {
            auto toErase = itter;
            itter++;
            d.erase(toErase);
        } else {
            itter++;
	}
    }
    unsigned int degree = 0;
    if (!d.empty())
        degree = (--(d.end()))->first;
    return make_rcp<const UnivariateIntPolynomial>(var, degree, std::move(d));
}

RCP<const UnivariateIntPolynomial> UnivariateIntPolynomial::from_vec(const RCP<const Symbol> &var, const std::vector<integer_class> &v){
    map_uint_mpz dict;
    unsigned int degree = 0;
    for(unsigned int i = 0; i < v.size(); i++) {
        if (0 != v[i]) {
  	    dict.insert(std::pair<unsigned int, integer_class>(i, v[i]));
            degree = i;
        }
    }
    return make_rcp<const UnivariateIntPolynomial>(var, degree, std::move(dict));
}

void UnivariateIntPolynomial::dict_add_term(map_uint_mpz &d, const integer_class &coef, const unsigned int &n)
{
    auto it = d.find(n);
    if (it == d.end())
        d[n] = coef;
}

vec_basic UnivariateIntPolynomial::get_args() const {
    vec_basic args;
    map_uint_mpz d;
    for (const auto &p: dict_) {
        d = {{p.first, p.second}};
        if (d.begin()->first == 0) {
            args.push_back( integer(d.begin()->second));
        }
        else if (d.begin()->first == 1) {
            if (d.begin()->second == 1) {
                args.push_back(var_);
	    } else {
                args.push_back(Mul::from_dict(integer(d.begin()->second), {{var_, one}}));
	    }
        } else {
            if (d.begin()->second == 1) {
                args.push_back( pow(var_, integer(d.begin()->first)));
            } else {
                args.push_back( Mul::from_dict(integer(d.begin()->second),{{var_, integer(d.begin()->first)}}));
            }
        }
    }
    if (dict_.empty())
        args.push_back(zero);
    return args;
}

integer_class UnivariateIntPolynomial::max_coef() const {
    integer_class curr = dict_.begin()->second;
    for (const auto &it : dict_) {
        if (it.second > curr)
            curr = it.second;
    }
    return curr;
}

integer_class UnivariateIntPolynomial::eval(const integer_class &x) const {
    //TODO: Use Horner's Scheme
    integer_class ans(0), temp;
    for (const auto &p : dict_) {
        mp_pow_ui(temp, x, p.first);
        ans += p.second * temp;
    }
    return ans;
}

integer_class UnivariateIntPolynomial::eval_bit(const int &x) const {
    //TODO: Use Horner's Scheme
    integer_class ans(0);
    for (const auto &p : dict_) {
        integer_class t = integer_class(1) << x * p.first;
        ans += p.second * t;
    }
    return ans;
}

bool UnivariateIntPolynomial::is_zero() const {
    if (dict_.empty())
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_one() const {
    if (dict_.size() == 1 and dict_.begin()->second == 1 and
            dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_minus_one() const {
    if (dict_.size() == 1 and dict_.begin()->second == -1 and
            dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_integer() const {
    if (dict_.empty())
        return true;
    if (dict_.size() == 1 and dict_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_symbol() const {
    if (dict_.size() == 1 and dict_.begin()->first == 1 and
            dict_.begin()->second == 1)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_mul() const {
    if (dict_.size() == 1 and dict_.begin()->first != 0 and
            dict_.begin()->second != 1 and dict_.begin()->second != 0)
        return true;
    return false;
}

bool UnivariateIntPolynomial::is_pow() const {
    if (dict_.size() == 1 and dict_.begin()->second == 1 and
            dict_.begin()->first != 1 and dict_.begin()->first != 0)
        return true;
    return false;
}

RCP<const UnivariateIntPolynomial> add_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b) {
    map_uint_mpz dict;
    for (const auto &it : a.dict_)
        dict[it.first] = it.second;
    for (const auto &it : b.dict_)
        dict[it.first] += it.second;

    RCP<const UnivariateIntPolynomial> c = UnivariateIntPolynomial::from_dict(a.var_, std::move(dict));
    return c;
}

RCP<const UnivariateIntPolynomial> neg_poly(const UnivariateIntPolynomial &a) {
    map_uint_mpz dict;
    for (const auto &it : a.dict_)
        dict[it.first] = -1 * it.second;

    RCP<const UnivariateIntPolynomial> c = UnivariateIntPolynomial::from_dict(a.var_, std::move(dict));
    return c;
}

RCP<const UnivariateIntPolynomial> sub_poly(const UnivariateIntPolynomial &a, const UnivariateIntPolynomial &b) {
    map_uint_mpz dict;
    for (const auto &it : a.dict_)
        dict[it.first] = it.second;
    for (const auto &it : b.dict_)
        dict[it.first] -= it.second;

    RCP<const UnivariateIntPolynomial> c = UnivariateIntPolynomial::from_dict(a.var_, std::move(dict));
    return c;
}

//Calculates bit length of number, used in mul_poly() only
template <typename T>
unsigned int bit_length(T t){
    unsigned int count = 0;
    while (t > 0) {
        count++;
        t = t >> 1;
    }
    return count;
}

RCP<const UnivariateIntPolynomial> mul_poly(RCP<const UnivariateIntPolynomial> a, RCP<const UnivariateIntPolynomial> b) {
    //TODO: Use `const RCP<const UnivariateIntPolynomial> &a` for input arguments,
    //      even better is use `const UnivariateIntPolynomial &a`
    unsigned int da = a->degree_;
    unsigned int db = b->degree_;

    int sign = 1;
    if ((--(a->dict_.end()))->second < 0) {
        a = neg_poly(*a);
        sign = -1 * sign;
    }
    if ((--(b->dict_.end()))->second < 0) {
        b = neg_poly(*b);
        sign = -1 * sign;
    }

    integer_class p = std::max(a->max_coef(), b->max_coef());

    unsigned int N = bit_length(std::min(da + 1, db + 1)) + bit_length(p) + 1;

    integer_class a1(1);
    a1 <<= N;
    integer_class a2 = a1 / 2;
    integer_class mask = a1 - 1;
    integer_class sa = a->eval_bit(N);
    integer_class sb = b->eval_bit(N);
    integer_class r = sa*sb;

    std::vector<integer_class> v;
    integer_class carry(0);

    while (r != 0 or carry != 0) {
        integer_class b;
        //TODO:fix this
        mp_and(b, r, mask);
        if (b < a2) {
            v.push_back(b + carry);
            carry = 0;
        } else {
            v.push_back(b - a1 + carry);
            carry = 1;
        }
        r >>= N;
    }

    if (sign == -1)
        return neg_poly(*UnivariateIntPolynomial::from_vec(a->var_, v));
    else
        return UnivariateIntPolynomial::from_vec(a->var_, v);
}

} // SymEngine
