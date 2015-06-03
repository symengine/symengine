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

    vec_basic Polynomial::get_args() const {
        vec_basic args;
        for (auto &p: dict_) {
            args.push_back(Polynomial::from_dict(var_, {{p.first, p.second}}));
        }
        return args;
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
}
