#include "polynomial.h"
#include "add.h"
#include "mul.h"
#include "pow.h"
#include "constants.h"

namespace SymEngine {

    Polynomial::Polynomial(const RCP<const Symbol> &var, map_uint_mpz&& dict) :
         var_{var}, dict_{std::move(dict)} {

        map_uint_mpz::iterator it = dict_.begin();
        uint largest = it->first;
        uint cur;
        while(it!=dict_.end()){
            cur =it->first;
            if(cur > largest)
                largest = cur;
            it++;
        }
        degree = largest;

        SYMENGINE_ASSERT(is_canonical(degree, dict_))
    }

    Polynomial::Polynomial(const RCP<const Symbol> &var, const std::vector<mpz_class> &v) :
         var_{var} {

        for (uint i = 0; i < v.size(); i++) {
            if (v[i] != 0)
                dict_add_term(dict_, v[i], i);
        }
        degree = v.size() - 1;

        SYMENGINE_ASSERT(is_canonical(degree, dict_))
    }

    bool Polynomial::is_canonical(const uint &degree, const map_uint_mpz& dict)
    {
        map_uint_mpz ordered(dict.begin(), dict.end());
        uint prev_degree = (--ordered.end())->first;
        if (prev_degree != degree)
            return false;

        return true;
    }

    std::size_t Polynomial::__hash__() const 
    {
        std::hash<std::string> hash_string;
        std::hash<uint> hash_uint;
        std::hash<long long int> hash_si;
        std::size_t seed = POLYNOMIAL;

        seed += hash_string(this->var_->get_name());
        for (auto &it : this->dict_)
        {
            seed += hash_uint(it.first) + hash_si(it.second.get_si());
        }
        return seed;
    }

    bool Polynomial::__eq__(const Basic &o) const
    {
        if (eq(var_, static_cast<const Polynomial &>(o).var_) &&
            map_uint_mpz_eq(dict_, static_cast<const Polynomial &>(o).dict_))
            return true;

        return false;
    }

    int Polynomial::compare(const Basic &o) const
    {
        const Polynomial &s = static_cast<const Polynomial &>(o);

        if (dict_.size() != s.dict_.size())
            return (dict_.size() < s.dict_.size()) ? -1 : 1;

        int cmp = var_->compare(*s.var_);
        if (cmp != 0)
            return cmp;

        return map_uint_mpz_compare(dict_, s.dict_);
    }

    RCP<const Basic> Polynomial::from_dict(const RCP<const Symbol> &var, map_uint_mpz &&d)
    {
        if (d.size() == 1) {
            if (d.begin()->first == 0) 
                return integer(d.begin()->second);
            else if (d.begin()->first == 1) {
                if (d.begin()->second == 0)
                    return zero;
                else if (d.begin()->second == 1) 
                    return var;
                else 
                    return Mul::from_dict(integer(d.begin()->second), {{var, one}});
            }
            else {
                if (d.begin()->second == 0)
                    return zero;
                else if (d.begin()->second == 1) 
                    return pow(var, integer(d.begin()->first));
                else 
                    return Mul::from_dict(integer(d.begin()->second), 
                        {{var, integer(d.begin()->first)}});
            }
        }
        else {
            return rcp(new Polynomial(var, std::move(d)));
        }
    }

    void Polynomial::dict_add_term(map_uint_mpz &d, const mpz_class &coef, const uint &n)
    {
        auto it = d.find(n);
        if (it == d.end())
            d[n] = coef;
    }

    vec_basic Polynomial::get_args() const {
        vec_basic args;
        for (auto &p: dict_) {
            args.push_back(Polynomial::from_dict(var_, {{p.first, p.second}}));
        }
        return args;
    }

    mpz_class Polynomial::max_coef() const {
        mpz_class curr = dict_.begin()->second;
        for (auto &it : dict_) {
            if (it.second > curr)
                curr = it.second;
        }
        return curr;
    }

    RCP<const Basic> Polynomial::diff(const RCP<const Symbol> &x) const
    {
        if (var_->__eq__(*x)) {
            map_uint_mpz d;
            for (auto &p : dict_) {
                d[p.first - 1] = p.second * p.first;
            }
            return rcp(new Polynomial(var_, std::move(d)));
        }
        else
            return zero;
    }

    mpz_class Polynomial::eval(const mpz_class &x) const {
        //TODO: Use Horner's Scheme
        mpz_class ans = 0;
        for (auto &p : dict_) {
            mpz_class temp;
            mpz_pow_ui(temp.get_mpz_t(), x.get_mpz_t(), p.first);
            ans += p.second * temp;
        }
        return ans;
    }

    mpz_class Polynomial::eval_bit(const int &x) const {
        //TODO: Use Horner's Scheme
        mpz_class ans = 0;
        for (auto &p : dict_) {
            mpz_class temp = 1;
            temp <<= x * p.first;
            ans += p.second * temp;
        }
        return ans;
    }

    bool Polynomial::is_zero() const {
        if (dict_.size() == 1 && dict_.begin()->second == 0)
            return true;
        return false;
    }

    bool Polynomial::is_one() const {
        if (dict_.size() == 1 && dict_.begin()->second == 1 &&
                dict_.begin()->first == 0)
            return true;
        return false;
    }

    bool Polynomial::is_minus_one() const {
        if (dict_.size() == 1 && dict_.begin()->second == -1 &&
                dict_.begin()->first == 0)
            return true;
        return false;
    }

    bool Polynomial::is_integer() const {
        if (dict_.size() == 1 && dict_.begin()->first == 0)
            return true;
        return false;
    }

    bool Polynomial::is_symbol() const {
        if (dict_.size() == 1 && dict_.begin()->first == 1 &&
                dict_.begin()->second == 1)
            return true;
        return false;
    }

    bool Polynomial::is_mul() const {
        if (dict_.size() == 1 && dict_.begin()->first != 0 &&
                dict_.begin()->second != 1 && dict_.begin()->second != 0)
            return true;
        return false;
    }

    bool Polynomial::is_pow() const {
        if (dict_.size() == 1 && dict_.begin()->second == 1 &&
                dict_.begin()->first != 1 && dict_.begin()->first != 0)
            return true;
        return false;
    }

    RCP<const Polynomial> add_poly(const Polynomial &a, const Polynomial &b) {
        map_uint_mpz dict;
        for(auto &it : a.dict_)
            dict[it.first] = it.second;
        for(auto &it : b.dict_)
            dict[it.first] += it.second;

        RCP<const Polynomial> c = polynomial(a.var_, std::move(dict));
        return c;
    }

    RCP<const Polynomial> neg_poly(const Polynomial &a) {
        map_uint_mpz dict;
        for(auto &it : a.dict_)
            dict[it.first] = -1 * it.second;

        RCP<const Polynomial> c = polynomial(a.var_, std::move(dict));
        return c;
    }

    RCP<const Polynomial> sub_poly(const Polynomial &a, const Polynomial &b) {
        map_uint_mpz dict;
        for(auto &it : a.dict_)
            dict[it.first] = it.second;
        for(auto &it : b.dict_)
            dict[it.first] -= it.second;

        RCP<const Polynomial> c = polynomial(a.var_, std::move(dict));
        return c;
    }

    //Calculates bit length of number, used in mul_poly() only
    template <typename T>
    uint bit_length(T t){
        uint count = 0;
        while (t > 0) {
        count++;
        t = t >> 1;
        }
        return count;
    }

    RCP<const Polynomial> mul_poly(RCP<const Polynomial> a, RCP<const Polynomial> b) {
        //TODO: Use `const RCP<const Polynomial> &a` for input arguments,
        //      even better is use `const Polynomial &a` 
        uint da = a->degree;
        uint db = b->degree;

        int sign = 1;
        if ((--(a->dict_.end()))->second < 0) {
            a = neg_poly(*a);
            sign = -1 * sign;
        }
        if ((--(b->dict_.end()))->second < 0) {
            b = neg_poly(*b);
            sign = -1 * sign;
        }

        mpz_class p = std::max(a->max_coef(), b->max_coef());
        uint N = bit_length(std::min(da + 1, db + 1)) + bit_length(p) + 1;

        mpz_class a1 = 1 << N;
        mpz_class a2 = a1 / 2;
        mpz_class mask = a1 - 1;
        mpz_class sa = a->eval_bit(N);
        mpz_class sb = b->eval_bit(N);
        mpz_class r = sa*sb;

        std::vector<mpz_class> v;
        mpz_class carry = 0;

        while (r != 0 || carry != 0) {
            mpz_class b;
            mpz_and(b.get_mpz_t(), r.get_mpz_t(), mask.get_mpz_t());
            if (b < a2) {
                v.push_back(b + carry);
                carry = 0;
            }
            else {
                v.push_back(b - a1 + carry);
                carry = 1;
            }
            r >>= N;
        }

        if (sign == -1)
            return neg_poly(*rcp(new Polynomial(a->var_, v)));
        else
            return rcp(new Polynomial(a->var_, v));
    }
}
