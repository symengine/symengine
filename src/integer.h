#ifndef CSYMPY_INTEGER_H
#define CSYMPY_INTEGER_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Integer : public Basic {
public:
    mpz_class i;

public:
    Integer(int i);
    Integer(mpz_class i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    // Convert to "int", raise an exception if it does not fit
    signed long int as_int();
    inline mpz_class as_mpz() { return this->i; }
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
    if (!(other->i.fits_ulong_p()))
        throw std::runtime_error("powint: 'exp' does not fit unsigned int.");
    mpz_class tmp;
    mpz_pow_ui(tmp.get_mpz_t(), self->i.get_mpz_t(), other->i.get_ui());
    return Teuchos::rcp(new CSymPy::Integer(tmp));
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


// Integers -1, 0 and 1 are created only once in integer.cpp and reused
// everywhere (faster than creating them all the time):
extern Teuchos::RCP<Integer> zero;
extern Teuchos::RCP<Integer> one;
extern Teuchos::RCP<Integer> minus_one;

} // CSymPy

#endif
