#ifndef CSYMPY_INTEGER_H
#define CSYMPY_INTEGER_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Integer : public Basic {
public:
    // TODO: make this private
    long long int i;
    mpz_class ii;

public:
    Integer(int i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;
};

inline Teuchos::RCP<Integer> addint(const Teuchos::RCP<Integer> &self,
    const Teuchos::RCP<Integer> &other)
{
    return Teuchos::rcp(new CSymPy::Integer(self->i + other->i));
}

inline Teuchos::RCP<Integer> subint(const Teuchos::RCP<Integer> &self,
    const Teuchos::RCP<Integer> &other)
{
    return Teuchos::rcp(new CSymPy::Integer(self->i - other->i));
}

inline Teuchos::RCP<Integer> mulint(const Teuchos::RCP<Integer> &self,
    const Teuchos::RCP<Integer> &other)
{
    return Teuchos::rcp(new CSymPy::Integer(self->i * other->i));
}

inline Teuchos::RCP<Integer> divint(const Teuchos::RCP<Integer> &self,
    const Teuchos::RCP<Integer> &other)
{
    return Teuchos::rcp(new CSymPy::Integer(self->i / other->i));
}

inline void iaddint(const Teuchos::Ptr<Teuchos::RCP<Integer>> &self,
    const Teuchos::RCP<Integer> &other)
{
    *self = addint(*self, other);
}

inline void isubint(const Teuchos::Ptr<Teuchos::RCP<Integer>> &self,
    const Teuchos::RCP<Integer> &other)
{
    *self = subint(*self, other);
}

inline void imulint(const Teuchos::Ptr<Teuchos::RCP<Integer>> &self,
    const Teuchos::RCP<Integer> &other)
{
    *self = mulint(*self, other);
}

inline void idivint(const Teuchos::Ptr<Teuchos::RCP<Integer>> &self,
    const Teuchos::RCP<Integer> &other)
{
    *self = divint(*self, other);
}


} // CSymPy

Teuchos::RCP<CSymPy::Integer> operator*(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b);

Teuchos::RCP<CSymPy::Integer> operator/(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b);

Teuchos::RCP<CSymPy::Integer> operator-(const Teuchos::RCP<CSymPy::Integer> &a);

#endif
