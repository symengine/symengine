#ifndef CSYMPY_RATIONAL_H
#define CSYMPY_RATIONAL_H

#include <gmpxx.h>

#include "basic.h"

namespace CSymPy {

class Rational : public Basic {
public:
    mpz_class iz;
    mpq_class iq;
    bool is_integer; // if true use iz, otherwise use iq. Note: If 'false',
                     // then iq is *not* an integer (iq.get_den() != 1).

public:
    Rational(int i);
    Rational(mpz_class i);
//    Rational(mpq_class i);
//    Rational(mpz_class num, mpz_class den);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;

    // Convert to "int", raise an exception if it does not fit
    signed long int as_int();
    inline mpz_class as_mpz() {
        if (!(is_integer)) throw std::runtime_error("Not an integer.");
        return this->iz;
    }
    inline bool is_zero() { return is_integer && this->iz == 0; }
    inline bool is_one() { return is_integer && this->iz == 1; }
    inline bool is_int() { return is_integer; }
};

inline Teuchos::RCP<Rational> addint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (self->is_integer && other->is_integer)
        return Teuchos::rcp(new CSymPy::Rational(self->iz + other->iz));
    else
        throw std::runtime_error("Not implemented.");
}

inline Teuchos::RCP<Rational> subint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (self->is_integer && other->is_integer)
        return Teuchos::rcp(new CSymPy::Rational(self->iz - other->iz));
    else
        throw std::runtime_error("Not implemented.");
}

inline Teuchos::RCP<Rational> mulint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (self->is_integer && other->is_integer)
        return Teuchos::rcp(new CSymPy::Rational(self->iz * other->iz));
    else
        throw std::runtime_error("Not implemented.");
}

inline Teuchos::RCP<Rational> divint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (self->is_integer && other->is_integer)
        return Teuchos::rcp(new CSymPy::Rational(self->iz / other->iz));
    else
        throw std::runtime_error("Not implemented.");
}

inline Teuchos::RCP<Rational> powint(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (other->is_integer) {
        if (!(other->iz.fits_ulong_p()))
            throw std::runtime_error("powint: 'exp' does not fit unsigned int.");
        mpz_class tmp;
        if (self->is_integer) {
            mpz_pow_ui(tmp.get_mpz_t(), self->iz.get_mpz_t(),
                    other->iz.get_ui());
            return Teuchos::rcp(new CSymPy::Rational(tmp));
        } else {
            throw std::runtime_error("powint: non-integer 'self' not implemented.");
        }
    } else {
        throw std::runtime_error("powint: 'exp' must be an integer.");
    }
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
    if (self->is_integer)
        return Teuchos::rcp(new CSymPy::Rational(-(self->iz)));
    else
        throw std::runtime_error("Not implemented.");
}


// Rationals -1, 0 and 1 are created only once in integer.cpp and reused
// everywhere (faster than creating them all the time):
extern Teuchos::RCP<Rational> zero;
extern Teuchos::RCP<Rational> one;
extern Teuchos::RCP<Rational> minus_one;

} // CSymPy

#endif
