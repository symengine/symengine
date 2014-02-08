#ifndef CSYMPY_NUMBER_H
#define CSYMPY_NUMBER_H

#include "basic.h"

namespace CSymPy {

class Number : public Basic {
public:
    virtual bool is_zero() const = 0;
    virtual bool is_one() const = 0;

    virtual RCP<const Number> add(const Number &other) const = 0;
    virtual RCP<const Number> sub(const Number &other) const = 0;
    virtual RCP<const Number> rsub(const Number &other) const = 0;
    virtual RCP<const Number> mul(const Number &other) const = 0;
    virtual RCP<const Number> div(const Number &other) const = 0;
    virtual RCP<const Number> rdiv(const Number &other) const = 0;
    virtual RCP<const Number> pow(const Number &other) const = 0;

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
};

inline RCP<const Number> addnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->add(*other);
}

inline RCP<const Number> subnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->sub(*other);
}

inline RCP<const Number> mulnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->mul(*other);
}

inline RCP<const Number> divnum(const RCP<const Number> &self,
    const RCP<const Number> &other)
{
    return self->div(*other);
}

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


} // CSymPy

#endif
