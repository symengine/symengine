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
    // 'i' must already be in mpq_class canonical form, this function returns
    // Integer or Rational depending on denumerator.
    static Teuchos::RCP<Number> from_mpq(const mpq_class i);
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

    inline Teuchos::RCP<Number> addrat(const Rational &other) const {
        return from_mpq(this->i + other.i);
    }

    inline Teuchos::RCP<Number> subrat(const Rational &other) const {
        return from_mpq(this->i - other.i);
    }

    inline Teuchos::RCP<Number> mulrat(const Rational &other) const {
        return from_mpq(this->i * other.i);
    }

    inline Teuchos::RCP<Number> divrat(const Rational &other) const {
        return from_mpq(this->i / other.i);
    }

    inline Teuchos::RCP<Number> powrat(const Integer &other) const {
        if (!(other.i.fits_ulong_p()))
            throw std::runtime_error("powrat: 'exp' does not fit unsigned int.");
        unsigned long exp = other.i.get_ui();
        mpz_class num;
        mpz_pow_ui(num.get_mpz_t(), this->i.get_num().get_mpz_t(), exp);

        mpz_class den;
        mpz_pow_ui(den.get_mpz_t(), this->i.get_den().get_mpz_t(), exp);

        // Since 'this' is in canonical form, so is this**other, so we simply
        // pass num/den into the constructor directly:
        return Teuchos::rcp(new Rational(mpq_class(num, den)));
    }


    virtual Teuchos::RCP<Number> add(const Number &other) const {
        if (is_a<Rational>(other)) {
            return addrat(static_cast<const Rational&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> sub(const Number &other) const {
        if (is_a<Rational>(other)) {
            return subrat(static_cast<const Rational&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> mul(const Number &other) const {
        if (is_a<Rational>(other)) {
            return mulrat(static_cast<const Rational&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> div(const Number &other) const {
        if (is_a<Rational>(other)) {
            return divrat(static_cast<const Rational&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual Teuchos::RCP<Number> pow(const Number &other) const {
        if (is_a<Integer>(other)) {
            return powrat(static_cast<const Integer&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
};

} // CSymPy

#endif
