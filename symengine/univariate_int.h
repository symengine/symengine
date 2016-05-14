/**
 *  \file polynomial_int.h
 *  Class for Univariate Int Polynomial Base
 **/
#ifndef SYMENGINE_UNIVARIATE_INT_H
#define SYMENGINE_UNIVARIATE_INT_H

#include <symengine/basic.h>

namespace SymEngine
{

template <typename Coeff, typename Container, typename Poly>
class UnivariateIntBase : public Basic
{
protected:
    unsigned int degree_;
    RCP<const Symbol> var_;
    Container container_;

public:
    UnivariateIntBase(const RCP<const Symbol> &var, const unsigned int &degree,
                      map_uint_mpz &&container)
        : degree_{degree}, var_{var}, container_{container}
    {
    }

    UnivariateIntBase(const RCP<const Symbol> &var,
                      const std::vector<integer_class> &v)
        : var_{var}
    {
    }

    //! \return the number terms in the polynomial
    virtual unsigned int get_length() const = 0;

    //! \return `true` if `0`
    virtual bool is_zero() const = 0;
    //! \return `true` if `1`
    virtual bool is_one() const = 0;
    //! \return `true` if `-1`
    virtual bool is_minus_one() const = 0;
    //! \return `true` if integer
    virtual bool is_integer() const = 0;
    //! \return `true` if symbol
    virtual bool is_symbol() const = 0;
    //! \return `true` if mul
    virtual bool is_mul() const = 0;
    //! \return `true` if pow
    virtual bool is_pow() const = 0;

    //! \return evaluated value at `x`
    virtual integer_class eval(const integer_class &x) const = 0;

    //! \returns `-1`,`0` or `1` after comparing
    virtual int compare(const Basic &o) const = 0;

    //! \returns `true` if two objects are equal
    inline bool __eq__(const Basic &o) const
    {
        return eq(*var_, *(static_cast<const Poly &>(o).var_))
               and container_ == static_cast<const Poly &>(o).container_;
    }

    inline RCP<const Symbol> get_var() const
    {
        return var_;
    }

    inline unsigned int get_degree() const
    {
        return degree_;
    }

    inline const Container &get_container() const
    {
        return container_;
    }
};
}

#endif