#ifndef CSYMPY_POLYNOMIALS_H
#define CSYMPY_POLYNOMIALS_H

#include "monomials.h"
#include "dict.h"
#include "basic.h"
#include "integer.h"

namespace CSymPy {

    class Polynomial : public Basic{
    public:
        int degree;
        std::string var_;
        map_uint_integer dict_;
    public:
        IMPLEMENT_TYPEID(POLYNOMIAL)
        Polynomial(){}

        Polynomial(const std::string& var, map_uint_integer&& dict);

        std::size_t __hash__() const {return 0;}
        //{
            //std::hash<long long int> hash_fn;
            //return hash_fn(this->var_);
        //}

        bool __eq__(const Basic &o) const
        {
            const Polynomial &s = static_cast<const Polynomial&>(o);
            return this->var_ == s.var_;
        }

        int compare(const Basic &o) const
        {
            const Polynomial &s = static_cast<const Polynomial&>(o);
            if (this->var_ == s.var_) return 0;
            return this->var_ < s.var_ ? -1 : 1;
        }

        virtual vec_basic get_args() const {return {};}

        virtual void accept(Visitor &v) const;

    }; //Polynomial

        RCP<const Polynomial> add_poly(const Polynomial &a, const Polynomial &b);
    inline RCP<const Polynomial> polynomial(std::string i, map_uint_integer&& dict)
    {
        return rcp(new Polynomial(i, std::move(dict)));
    }
}  //CSymPy

#endif






