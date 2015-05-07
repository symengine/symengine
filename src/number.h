/**
 *  \file number.h
 *  Basic Number class.
 *
 **/
 
#ifndef CSYMPY_NUMBER_H
#define CSYMPY_NUMBER_H

#include "basic.h"

namespace SymEngine {

class Number : public Basic {
public:
    //! \return true if `0`
    virtual bool is_zero() const = 0;
    //! \return true if `1`
    virtual bool is_one() const = 0;
    //! \return true if `-1`
    virtual bool is_minus_one() const = 0;

    virtual bool is_negative() const = 0;

    virtual bool is_positive() const = 0;
    //! Addition
    virtual RCP<const Number> add(const Number &other) const = 0;
    //! Subtraction
    virtual RCP<const Number> sub(const Number &other) const = 0;
    virtual RCP<const Number> rsub(const Number &other) const = 0;
    //! Multiplication
    virtual RCP<const Number> mul(const Number &other) const = 0;
    //! Division
    virtual RCP<const Number> div(const Number &other) const = 0;
    virtual RCP<const Number> rdiv(const Number &other) const = 0;
    //! Power
    virtual RCP<const Number> pow(const Number &other) const = 0;
    //! Differentiation w.r.t Symbol `x`
    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;

    virtual vec_basic get_args() const { return {}; }
};
//! Add `self` and `other`
inline RCP<const Number> addnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->add(*other);
}
//! Subtract `self` and `other`
inline RCP<const Number> subnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->sub(*other);
}
//! Multiply `self` and `other`
inline RCP<const Number> mulnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->mul(*other);
}
//! Divide `self` and `other`
inline RCP<const Number> divnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->div(*other);
}
//! Raise `self` to power `other`
inline RCP<const Number> pownum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->pow(*other);
}

inline void iaddnum(const Ptr<RCP<const Number>> &self,
    const RCP<const Number> &other)
{
    *self = addnum(*self, other);
}

inline void imulnum(const Ptr<RCP<const Number>> &self,
    const RCP<const Number> &other)
{
    *self = mulnum(*self, other);
}

inline void idivnum(const Ptr<RCP<const Number>> &self,
    const RCP<const Number> &other)
{
    *self = divnum(*self, other);
}


} // SymEngine

#endif
