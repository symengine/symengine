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

    int as_int(); // Convert to "int", raise an exception if it is too large
    inline bool is_zero() { return this->i == 0; }
    inline bool is_one() { return this->i == 1; }
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

inline Teuchos::RCP<Integer> powint(const Teuchos::RCP<Integer> &self,
    const Teuchos::RCP<Integer> &other)
{
    return Teuchos::rcp(new CSymPy::Integer(std::pow(self->i, other->i)));
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

inline void ipowint(const Teuchos::Ptr<Teuchos::RCP<Integer>> &self,
    const Teuchos::RCP<Integer> &other)
{
    *self = powint(*self, other);
}

inline Teuchos::RCP<Integer> negint(const Teuchos::RCP<Integer> &self)
{
    return Teuchos::rcp(new CSymPy::Integer(-(self->i)));
}

} // CSymPy

#endif
