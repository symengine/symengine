#ifndef CSYMPY_NUMBER_H
#define CSYMPY_NUMBER_H

#include "basic.h"

namespace CSymPy {

class Number : public Basic {
public:
    virtual bool is_zero() const = 0;
    virtual bool is_one() const = 0;

    virtual Teuchos::RCP<Number> add(const Number &other) const = 0;
    virtual Teuchos::RCP<Number> sub(const Number &other) const = 0;
    virtual Teuchos::RCP<Number> mul(const Number &other) const = 0;
    virtual Teuchos::RCP<Number> div(const Number &other) const = 0;
    virtual Teuchos::RCP<Number> pow(const Number &other) const = 0;
};

inline Teuchos::RCP<Number> addint(const Teuchos::RCP<Number> &self,
    const Teuchos::RCP<Number> &other)
{
    return self->add(*other);
}

inline Teuchos::RCP<Number> mulint(const Teuchos::RCP<Number> &self,
    const Teuchos::RCP<Number> &other)
{
    return self->mul(*other);
}

inline Teuchos::RCP<Number> powint(const Teuchos::RCP<Number> &self,
    const Teuchos::RCP<Number> &other)
{
    return self->pow(*other);
}

inline void iaddint(const Teuchos::Ptr<Teuchos::RCP<Number>> &self,
    const Teuchos::RCP<Number> &other)
{
    *self = addint(*self, other);
}

inline void imulint(const Teuchos::Ptr<Teuchos::RCP<Number>> &self,
    const Teuchos::RCP<Number> &other)
{
    *self = mulint(*self, other);
}


} // CSymPy

#endif
