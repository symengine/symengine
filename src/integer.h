#ifndef CSYMPY_INTEGER_H
#define CSYMPY_INTEGER_H

#include <gmpxx.h>

#include "basic.h"
#include "number.h"

namespace CSymPy {

class Integer : public Number {
public:
    mpz_class i;

public:
    const static TypeID type_code_id = INTEGER;
    Integer(int i);
    Integer(mpz_class i);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    // Convert to "int", raise an exception if it does not fit
    signed long int as_int();
    inline mpz_class as_mpz() { return this->i; }
    inline virtual bool is_zero() const { return this->i == 0; }
    inline virtual bool is_one() const { return this->i == 1; }
    inline virtual bool is_positive() const { return this->i > 0; }
    inline virtual bool is_negative() const { return this->i < 0; }


    /* These are very fast methods for add/sub/mul/div/pow on Integers only */

    inline Teuchos::RCP<Integer> addint(const Integer &other) const {
        return Teuchos::rcp(new Integer(this->i + other.i));
    }

    inline Teuchos::RCP<Integer> subint(const Integer &other) const {
        return Teuchos::rcp(new Integer(this->i - other.i));
    }

    inline Teuchos::RCP<Integer> mulint(const Integer &other) const {
        return Teuchos::rcp(new Integer(this->i * other.i));
    }

    Teuchos::RCP<Number> divint(const Integer &other) const;
    Teuchos::RCP<Number> pow_negint(const Integer &other) const;

    inline Teuchos::RCP<Number> powint(const Integer &other) const {
        if (!(other.i.fits_ulong_p())) {
            if (other.i > 0)
                throw std::runtime_error("powint: 'exp' does not fit unsigned int.");
            else
                return pow_negint(other);
        }
        mpz_class tmp;
        mpz_pow_ui(tmp.get_mpz_t(), this->i.get_mpz_t(), other.i.get_ui());
        return Teuchos::rcp(new Integer(tmp));
    }

    inline Teuchos::RCP<Integer> neg() const {
        return Teuchos::rcp(new Integer(-i));
    }

    /* These are general methods, overriden from the Number class, that need to
     * check types to decide what operation to do, and so are a bit slower. */

    virtual Teuchos::RCP<Number> add(const Number &other) const {
        if (is_a<Integer>(other)) {
            return addint(static_cast<const Integer&>(other));
        } else {
            return other.add(*this);
        }
    };

    virtual Teuchos::RCP<Number> sub(const Number &other) const {
        if (is_a<Integer>(other)) {
            return subint(static_cast<const Integer&>(other));
        } else {
            return other.rsub(*this);
        }
    };

    virtual Teuchos::RCP<Number> rsub(const Number &other) const {
        throw std::runtime_error("Not implemented.");
    };

    virtual Teuchos::RCP<Number> mul(const Number &other) const {
        if (is_a<Integer>(other)) {
            return mulint(static_cast<const Integer&>(other));
        } else {
            return other.mul(*this);
        }
    };

    virtual Teuchos::RCP<Number> div(const Number &other) const {
        if (is_a<Integer>(other)) {
            return divint(static_cast<const Integer&>(other));
        } else {
            return other.rdiv(*this);
        }
    };

    virtual Teuchos::RCP<Number> rdiv(const Number &other) const {
        throw std::runtime_error("Not implemented.");
    };

    virtual Teuchos::RCP<Number> pow(const Number &other) const {
        if (is_a<Integer>(other)) {
            return powint(static_cast<const Integer&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
};

inline Teuchos::RCP<Integer> integer(int i)
{
    return Teuchos::rcp(new Integer(i));
}

inline Teuchos::RCP<Integer> integer(mpz_class i)
{
    return Teuchos::rcp(new Integer(i));
}

// Integers -1, 0 and 1 are created only once in integer.cpp and reused
// everywhere (faster than creating them all the time):
extern Teuchos::RCP<Integer> zero;
extern Teuchos::RCP<Integer> one;
extern Teuchos::RCP<Integer> minus_one;

} // CSymPy

#endif
