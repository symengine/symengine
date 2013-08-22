#ifndef CSYMPY_NUMBER_H
#define CSYMPY_NUMBER_H

#include "basic.h"

namespace CSymPy {

class Number : public Basic {
public:
    virtual bool is_zero() const = 0;
    virtual bool is_one() const = 0;

    virtual RCP<Number> add(const Number &other) const = 0;
    virtual RCP<Number> sub(const Number &other) const = 0;
    virtual RCP<Number> rsub(const Number &other) const = 0;
    virtual RCP<Number> mul(const Number &other) const = 0;
    virtual RCP<Number> div(const Number &other) const = 0;
    virtual RCP<Number> rdiv(const Number &other) const = 0;
    virtual RCP<Number> pow(const Number &other) const = 0;

    virtual RCP<Basic> diff(const RCP<Symbol> &x) const;
};

inline RCP<Number> addnum(const RCP<Number> &self,
    const RCP<Number> &other)
{
    return self->add(*other);
}

inline RCP<Number> subnum(const RCP<Number> &self,
    const RCP<Number> &other)
{
    return self->sub(*other);
}

inline RCP<Number> mulnum(const RCP<Number> &self,
    const RCP<Number> &other)
{
    return self->mul(*other);
}

inline RCP<Number> divnum(const RCP<Number> &self,
    const RCP<Number> &other)
{
    return self->div(*other);
}

inline RCP<Number> pownum(const RCP<Number> &self,
    const RCP<Number> &other)
{
    return self->pow(*other);
}

inline void iaddnum(const Ptr<RCP<Number>> &self,
    const RCP<Number> &other)
{
    *self = addnum(*self, other);
}

inline void imulnum(const Ptr<RCP<Number>> &self,
    const RCP<Number> &other)
{
    *self = mulnum(*self, other);
}


} // CSymPy

#endif
