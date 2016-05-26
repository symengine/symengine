/**
 *  \file polynomial_int.h
 *  Class for Univariate Polynomial Base
 **/
#ifndef SYMENGINE_UINT_BASE_H
#define SYMENGINE_UINT_BASE_H

#include <symengine/basic.h>

namespace SymEngine
{

template <typename Container, typename Poly>
class UIntPolyBase : public Basic
{
protected:
    RCP<const Symbol> var_;
    Container poly_;

public:
    UIntPolyBase(const RCP<const Symbol> &var, Container &&container)
        : var_{var}, poly_{container}
    {
    }

    // unify these two constructor? another template would be required
    // may solve some more problems, like `get_degree` virtualization
    UIntPolyBase(const RCP<const Symbol> &var,
                 const std::vector<integer_class> &v)
        : var_{var}
    {
    }

    UIntPolyBase(const RCP<const Symbol> &var, const std::vector<Expression> &v)
        : var_{var}
    {
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

    friend RCP<const Poly> add_poly(const Poly &a, const Poly &b)
    {
        if (!(a.var_->__eq__(*b.var_)))
            throw std::runtime_error("Error: variables must agree.");

        Container dict = a.poly_;
        dict += b.poly_;
        return Poly::from_dict(a.var_, std::move(dict));
    }

    friend RCP<const Poly> neg_poly(const Poly &a)
    {
        Container dict = -(a.poly_);
        return Poly::from_dict(a.var_, std::move(dict));
    }

    friend RCP<const Poly> sub_poly(const Poly &a, const Poly &b)
    {
        if (!(a.var_->__eq__(*b.var_)))
            throw std::runtime_error("Error: variables must agree.");

        Container dict = a.poly_;
        dict -= b.poly_;
        return Poly::from_dict(a.var_, std::move(dict));
    }

    friend RCP<const Poly> mul_poly(const Poly &a, const Poly &b)
    {
        if (!(a.var_->__eq__(*b.var_)))
            throw std::runtime_error("Error: variables must agree.");

        Container dict = a.get_poly();
        dict *= b.get_poly();
        return Poly::from_dict(a.var_, std::move(dict));
    }
};
}

#endif