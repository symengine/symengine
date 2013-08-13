#ifndef CSYMPY_RATIONAL_H
#define CSYMPY_RATIONAL_H

#include <gmpxx.h>

#include "basic.h"
#include "number.h"
#include "integer.h"

namespace CSymPy {

class Rational : public Number {
public:
    mpq_class i;

public:
    Rational(mpq_class i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual std::string __str__() const;
    bool is_canonical(const mpq_class &i);

    // Constructs Rational as n/d, where n, d can be any Integers. If n/d is an
    // Integer, it will return an Integer instead.
    static Teuchos::RCP<Number> from_two_ints(const Teuchos::RCP<Integer> &n,
            const Teuchos::RCP<Integer> &d);

    virtual bool is_zero() const { return this->i == 0; }
    virtual bool is_one() const { return this->i == 1; }
    inline bool is_int() { return this->i.get_den() == 1; }

    virtual Teuchos::RCP<Number> add(const Number &other) const {
        if (is_a<Rational>(other)) {
            return Teuchos::rcp(new Rational(this->i +
                        static_cast<const Rational&>(other).i));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> sub(const Number &other) const {
        if (is_a<Rational>(other)) {
            return Teuchos::rcp(new Rational(this->i -
                        static_cast<const Rational&>(other).i));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> mul(const Number &other) const {
        if (is_a<Rational>(other)) {
            return Teuchos::rcp(new Rational(this->i *
                        static_cast<const Rational&>(other).i));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> div(const Number &other) const {
        if (is_a<Rational>(other)) {
            return Teuchos::rcp(new Rational(this->i /
                        static_cast<const Rational&>(other).i));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> pow(const Number &other) const {
        if (is_a<Rational>(other)) {
            throw std::runtime_error("Not implemented.");
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
};

inline Teuchos::RCP<Rational> addrat(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i + other->i));
}

inline Teuchos::RCP<Rational> subrat(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i - other->i));
}

inline Teuchos::RCP<Rational> mulrat(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i * other->i));
}

inline Teuchos::RCP<Rational> divrat(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    return Teuchos::rcp(new CSymPy::Rational(self->i / other->i));
}

inline Teuchos::RCP<Rational> powrat(const Teuchos::RCP<Rational> &self,
    const Teuchos::RCP<Rational> &other)
{
    if (other->i.get_den() == 1) {
        if (!(other->i.get_num().fits_ulong_p()))
            throw std::runtime_error("powint: 'exp' does not fit unsigned int.");
        mpz_class tmp;
        mpz_pow_ui(tmp.get_mpz_t(), self->i.get_num().get_mpz_t(),
                other->i.get_num().get_ui());
        if (self->i.get_den() == 1) {
            return Teuchos::rcp(new CSymPy::Rational(tmp));
        } else {
            mpz_class tmp2;
            mpz_pow_ui(tmp2.get_mpz_t(), self->i.get_den().get_mpz_t(),
                    other->i.get_num().get_ui());
            mpq_class q(tmp, tmp2);
            return Teuchos::rcp(new CSymPy::Rational(q));
        }
    } else {
        throw std::runtime_error("powint: 'exp' must be an integer.");
    }
}

} // CSymPy

#endif
