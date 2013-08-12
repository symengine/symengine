#ifndef CSYMPY_RATIONAL_H
#define CSYMPY_RATIONAL_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Rational : public Basic {
public:
    mpz_class i;

public:
    Rational(int i);
    Rational(mpz_class i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    // Convert to "int", raise an exception if it does not fit
    signed long int as_int();
    inline mpz_class as_mpz() { return this->i; }
    inline bool is_zero() { return this->i == 0; }
    inline bool is_one() { return this->i == 1; }
};

inline Teuchos::RCP<Rational> addint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i + other->i));
}

inline Teuchos::RCP<Rational> subint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i - other->i));
}

inline Teuchos::RCP<Rational> mulint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i * other->i));
}

inline Teuchos::RCP<Rational> divint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i / other->i));
}

inline Teuchos::RCP<Rational> powint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (!(other->i.fits_ulong_p()))
        throw std::runtime_error("powint: 'exp' does not fit unsigned int.");
    mpz_class tmp;
    mpz_pow_ui(tmp.get_mpz_t(), self->i.get_mpz_t(), other->i.get_ui());
    return Teuchos::rcp(new CSymPy::Rational(tmp));
}

inline void iaddint(const Teuchos::Ptr<Teuchos::RCP<Rational>> &self,
    const Teuchos::RCP<Rational> &other)
{
    *self = addint(*self, other);
}

inline void isubint(const Teuchos::Ptr<Teuchos::RCP<Rational>> &self,
    const Teuchos::RCP<Rational> &other)
{
    *self = subint(*self, other);
}

inline void imulint(const Teuchos::Ptr<Teuchos::RCP<Rational>> &self,
    const Teuchos::RCP<Rational> &other)
{
    *self = mulint(*self, other);
}

inline void idivint(const Teuchos::Ptr<Teuchos::RCP<Rational>> &self,
    const Teuchos::RCP<Rational> &other)
{
    *self = divint(*self, other);
}

inline void ipowint(const Teuchos::Ptr<Teuchos::RCP<Rational>> &self,
    const Teuchos::RCP<Rational> &other)
{
    *self = powint(*self, other);
}

inline Teuchos::RCP<Rational> negint(const Teuchos::RCP<Rational> &self)
{
    return Teuchos::rcp(new CSymPy::Rational(-(self->i)));
}


// Rationals -1, 0 and 1 are created only once in integer.cpp and reused
// everywhere (faster than creating them all the time):
extern Teuchos::RCP<Rational> zero;
extern Teuchos::RCP<Rational> one;
extern Teuchos::RCP<Rational> minus_one;

} // CSymPy

#endif
