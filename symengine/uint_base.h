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
public:
    unsigned int degree_;
    RCP<const Symbol> var_;
    Container poly_;

    UIntPolyBase(const RCP<const Symbol> &var, const unsigned int &degree,
                 Container &&container)
        : degree_{degree}, var_{var}, poly_{container}
    {
    }

    UIntPolyBase(const RCP<const Symbol> &var,
                 const std::vector<integer_class> &v)
        : var_{var}
    {
    }

    // //! \returns `-1`,`0` or `1` after comparing
    virtual int compare(const Basic &o) const = 0;

    //! \returns `true` if two objects are equal
    inline bool __eq__(const Basic &o) const
    {
        return eq(*var_, *(static_cast<const Poly &>(o).var_))
               and poly_ == static_cast<const Poly &>(o).poly_;
    }

    inline RCP<const Symbol> get_var() const
    {
        return var_;
    }

    inline unsigned int get_degree() const
    {
        return degree_;
    }

    inline const Container &get_poly() const
    {
        return poly_;
    }
};
}

#endif