/**
 *  \file polynomial.h
 *  Class for univariate sparse Polynomial
 *
 **/
#ifndef SYMENGINE_POLYNOMIALS_H
#define SYMENGINE_POLYNOMIALS_H

#include "monomials.h"
#include "dict.h"
#include "basic.h"
#include "integer.h"

namespace SymEngine {

    class Polynomial : public Basic{
    public:
        int degree;
        std::string var_;
        map_uint_mpz dict_;
    public:
        IMPLEMENT_TYPEID(POLYNOMIAL)
        Polynomial(){}

        Polynomial(const std::string& var, map_uint_mpz&& dict);

        std::size_t __hash__() const 
        {
            std::hash<std::string> hash_string;
            std::hash<uint> hash_uint;
            std::hash<long long int> hash_si;
            std::size_t seed = POLYNOMIAL;

            seed += hash_string(var_);
            for (auto &it : dict_)
            {
                seed += hash_uint(it.first) + hash_si(it.second.get_si());
            }
            return seed;
        }

        bool __eq__(const Basic &o) const
        {
            if ((var_ == static_cast<const Polynomial &>(o).var_) &&
                map_uint_mpz_eq(dict_, static_cast<const Polynomial &>(o).dict_))
                return true;

            return false;
        }

        int compare(const Basic &o) const
        {
            const Polynomial &s = static_cast<const Polynomial &>(o);

            if (dict_.size() != s.dict_.size())
                return (dict_.size() < s.dict_.size()) ? -1 : 1;

            int cmp = (var_ < s.var_) ? -1 : 1;
            if (cmp != 0)
                return cmp;

            return map_uint_mpz_compare(dict_, s.dict_);
        }

        virtual vec_basic get_args() const {return {};}

        virtual void accept(Visitor &v) const;

    }; //Polynomial

    RCP<const Polynomial> add_poly(const Polynomial &a, const Polynomial &b);

    RCP<const Polynomial> neg_poly(const Polynomial &a);

    RCP<const Polynomial> sub_poly(const Polynomial &a, const Polynomial &b);

    inline RCP<const Polynomial> polynomial(std::string i, map_uint_mpz&& dict)
    {
        return rcp(new Polynomial(i, std::move(dict)));
    }
}  //SymEngine

#endif
