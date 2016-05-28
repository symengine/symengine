/**
 *  \file polynomial_int.h
 *  Class for Univariate Polynomial Base
 **/
#ifndef SYMENGINE_UINT_BASE_H
#define SYMENGINE_UINT_BASE_H

#include <symengine/basic.h>

namespace SymEngine
{

template <typename Container, typename Poly, typename Coeff>
class UIntPolyBase : public Basic
{
protected:
    RCP<const Symbol> var_;
    Container poly_;

public:
    UIntPolyBase(const RCP<const Symbol> &var, Container &&d)
        : var_{var}, poly_{d}
    {
    }

    UIntPolyBase(const RCP<const Symbol> &var, const std::vector<Coeff> &v)
        : var_{var}
    {
    }

    // creates a Poly in cannonical form based on the dictionary.
    static RCP<const Poly> from_dict(const RCP<const Symbol> &var,
                                     Container &&d)
    {
        return make_rcp<const Poly>(var, std::move(d));
    }
    // create a Poly from a dense vector of Coeff coefficients
    static RCP<const Poly> from_vec(const RCP<const Symbol> &var,
                                    const std::vector<Coeff> v)
    {
        return make_rcp<const Poly>(var, std::move(v));
    }

    // TODO think of something to make this purely virtual
    //! \returns the degree of the polynomial
    // virtual unsigned int get_degree() const = 0;

    //! \returns `-1`,`0` or `1` after comparing
    virtual int compare(const Basic &o) const = 0;

    //! \returns `true` if two objects are equal
    inline bool __eq__(const Basic &o) const
    {
        return eq(*var_, *(static_cast<const Poly &>(o).var_))
               and poly_ == static_cast<const Poly &>(o).poly_;
    }

    inline const RCP<const Symbol> &get_var() const
    {
        return var_;
    }

    inline const Container &get_poly() const
    {
        return poly_;
    }
};

template <typename Poly>
RCP<const Poly> add_poly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict += b.get_poly();
    return Poly::from_dict(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> neg_poly(const Poly &a)
{
    auto dict = -(a.get_poly());
    return Poly::from_dict(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> sub_poly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict -= b.get_poly();
    return Poly::from_dict(a.get_var(), std::move(dict));
}

template <typename Poly>
RCP<const Poly> mul_poly(const Poly &a, const Poly &b)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto dict = a.get_poly();
    dict *= b.get_poly();
    return Poly::from_dict(a.get_var(), std::move(dict));
}
}

#endif