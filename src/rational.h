/**
 *  \file rational.h
 *  Class for Rationals built on top of Number class
 *
 **/
#ifndef SYMENGINE_RATIONAL_H
#define SYMENGINE_RATIONAL_H

#include "basic.h"
#include "number.h"
#include "integer.h"
#include "constants.h"

namespace SymEngine {
//! Rational Class
class Rational : public Number {
public:
    //! `i` : object of `mpq_class`
    mpq_class i;

public:
    IMPLEMENT_TYPEID(RATIONAL)
    //! Constructor of Rational class
    Rational(mpq_class i);
    /*! \param `i` must already be in mpq_class canonical form
    *   \return Integer or Rational depending on denumerator.
    * */
    static RCP<const Number> from_mpq(const mpq_class i);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return true if canonical
    bool is_canonical(const mpq_class &i);

    /*! Constructs Rational as n/d, where n, d can be any Integers. If n/d is an
    *   Integer, it will return an Integer instead.
    * */
    static RCP<const Number> from_two_ints(const RCP<const Integer> &n,
            const RCP<const Integer> &d);
    //! \return `true` if `0`
    virtual bool is_zero() const { return this->i == 0; }
    //! \return `true` if `1`
    virtual bool is_one() const { return this->i == 1; }
    //! \return `true` if `-1`
    virtual bool is_minus_one() const { return this->i == -1; }
    //! \return `true` if denominator is `1`
    inline bool is_int() const { return this->i.get_den() == 1; }
    //! \return `true` if positive
    inline virtual bool is_positive() const {
        return ((this->i.get_den() > 0) && (this->i.get_num() > 0)) ||
                ((this->i.get_den() < 0) && (this->i.get_num() < 0)) ; }
    //! \return `true` if negative
    inline virtual bool is_negative() const {
        return ((this->i.get_den() < 0) && (this->i.get_num() > 0)) ||
                ((this->i.get_den() > 0) && (this->i.get_num() < 0)) ; }

    /*! Add Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> addrat(const Rational &other) const {
        return from_mpq(this->i + other.i);
    }
    /*! Add Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> addrat(const Integer &other) const {
        return from_mpq(this->i + other.i);
    }
    /*! Subtract Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> subrat(const Rational &other) const {
        return from_mpq(this->i - other.i);
    }
    /*! Subtract Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> subrat(const Integer &other) const {
        return from_mpq(this->i - other.i);
    }
    inline RCP<const Number> rsubrat(const Integer &other) const {
        return from_mpq(other.i - this->i);
    }
    /*! Multiply Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> mulrat(const Rational &other) const {
        return from_mpq(this->i * other.i);
    }
    /*! Multiply Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> mulrat(const Integer &other) const {
        return from_mpq(this->i * other.i);
    }
    /*! Divide Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> divrat(const Rational &other) const {
        if (other.i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(this->i / other.i);
        }
    }
    /*! Divide Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> divrat(const Integer &other) const {
        if (other.i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(this->i / other.i);
        }
    }
    inline RCP<const Number> rdivrat(const Integer &other) const {
        if (this->i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(other.i / this->i);
        }
    }
    /*! Raise Rationals to power `other`
     * \param other power to be raised
     * */
    inline RCP<const Number> powrat(const Integer &other) const {
        bool neg = other.is_negative();
        mpz_class exp_ = other.i;
        if (neg) exp_ = -exp_;
        if (!(exp_.fits_ulong_p()))
            throw std::runtime_error("powrat: 'exp' does not fit unsigned int.");
        unsigned long exp = exp_.get_ui();
        mpz_class num;
        mpz_pow_ui(num.get_mpz_t(), this->i.get_num().get_mpz_t(), exp);

        mpz_class den;
        mpz_pow_ui(den.get_mpz_t(), this->i.get_den().get_mpz_t(), exp);

        // Since 'this' is in canonical form, so is this**other, so we simply
        // pass num/den into the constructor directly:
        if (!neg)
            if (abs(den) == one->i)
                return rcp(new Integer(num*sgn(den)));
            else
                return rcp(new Rational(mpq_class(num*sgn(den), abs(den))));
        else
            if (abs(num) == one->i)
                return rcp(new Integer(den*sgn(num)));
            else
                return rcp(new Rational(mpq_class(den*sgn(num), abs(num))));
    }

    //! Converts the param `other` appropriately and then calls `addrat`
    virtual RCP<const Number> add(const Number &other) const {
        if (is_a<Rational>(other)) {
            return addrat(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return addrat(static_cast<const Integer&>(other));
        } else {
            return other.add(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `subrat`
    virtual RCP<const Number> sub(const Number &other) const {
        if (is_a<Rational>(other)) {
            return subrat(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return subrat(static_cast<const Integer&>(other));
        } else {
            return other.rsub(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `rsubrat`
    virtual RCP<const Number> rsub(const Number &other) const {
        if (is_a<Integer>(other)) {
            return rsubrat(static_cast<const Integer&>(other));
        } else {
            return other.sub(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `mulrat`
    virtual RCP<const Number> mul(const Number &other) const {
        if (is_a<Rational>(other)) {
            return mulrat(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return mulrat(static_cast<const Integer&>(other));
        } else {
            return other.mul(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `divrat`
    virtual RCP<const Number> div(const Number &other) const {
        if (is_a<Rational>(other)) {
            return divrat(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return divrat(static_cast<const Integer&>(other));
        } else {
            return other.rdiv(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `rdivrat`
    virtual RCP<const Number> rdiv(const Number &other) const {
        if (is_a<Integer>(other)) {
            return rdivrat(static_cast<const Integer&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
    //! Converts the param `other` appropriately and then calls `powrat`
    virtual RCP<const Number> pow(const Number &other) const {
        if (is_a<Integer>(other)) {
            return powrat(static_cast<const Integer&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };

    virtual void accept(Visitor &v) const;
};

//! returns the `num` and `den` of rational `rat` as `rcp Integer`
void get_num_den(const RCP<const Rational> &rat,
        const Ptr<RCP<const Integer>> &num,
        const Ptr<RCP<const Integer>> &den);

} // SymEngine

#endif
