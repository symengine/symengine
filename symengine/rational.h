/**
 *  \file rational.h
 *  Class for Rationals built on top of Number class
 *
 **/
#ifndef SYMENGINE_RATIONAL_H
#define SYMENGINE_RATIONAL_H

#include <symengine/basic.h>
#include <symengine/number.h>
#include <symengine/integer.h>
#include <symengine/constants.h>

namespace SymEngine {
//! Rational Class
class Rational : public Number {
private:
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
    bool is_canonical(const mpq_class &i) const;

    /*! Constructs Rational as n/d, where n, d can be any Integers. If n/d is an
    *   Integer, it will return an Integer instead.
    * */
    static RCP<const Number> from_two_ints(const Integer &n,
            const Integer &d);
    static RCP<const Number> from_two_ints(const long n, const long d);
    //! Convert to `mpq_class`.
    inline mpq_class as_mpq() const { return this->i; }
    //! \return `true` if `num` is `0`
    virtual bool is_zero() const { return this->i.get_num() == 0; }
    //! \return `false` since `Rational` cannot be an `Integer`
    virtual bool is_one() const { return false; }
    //! \return `false` since `Rational` cannot be an `Integer`
    virtual bool is_minus_one() const { return false; }
    //! \return `true` if positive
    inline virtual bool is_positive() const {
        return this->i.get_num() > 0;
    }
    //! \return `true` if negative
    inline virtual bool is_negative() const {
        return this->i.get_num() < 0;
    }
    //! \return negative of self
    inline RCP<const Number> neg() const {
        return from_mpq(-this->i);
    }

    virtual bool is_perfect_power(bool is_expected=false) const;
    virtual bool nth_root(const Ptr<RCP<const Number>> &, unsigned int n) const;

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
        return from_mpq(this->i + other.get_i());
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
        return from_mpq(this->i - other.get_i());
    }
    inline RCP<const Number> rsubrat(const Integer &other) const {
        return from_mpq(other.get_i() - this->i);
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
        return from_mpq(this->i * other.get_i());
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
        if (other.get_i() == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(this->i / other.get_i());
        }
    }
    inline RCP<const Number> rdivrat(const Integer &other) const {
        if (this->i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(other.get_i() / this->i);
        }
    }
    /*! Raise Rationals to power `other`
     * \param other power to be raised
     * */
    inline RCP<const Number> powrat(const Integer &other) const {
        bool neg = other.is_negative();
        mpz_class exp_ = other.get_i();
        if (neg) exp_ = -exp_;
        if (not (exp_.fits_ulong_p()))
            throw std::runtime_error("powrat: 'exp' does not fit unsigned int.");
        unsigned long exp = exp_.get_ui();
        mpz_class num;
        mpz_pow_ui(num.get_mpz_t(), this->i.get_num().get_mpz_t(), exp);

        mpz_class den;
        mpz_pow_ui(den.get_mpz_t(), this->i.get_den().get_mpz_t(), exp);

        // Since 'this' is in canonical form, so is this**other, so we simply
        // pass num/den into the constructor directly:
        if (not neg)
            if (abs(den) == one->get_i())
                return integer(num*sgn(den));
            else
                return make_rcp<const Rational>(mpq_class(num*sgn(den), abs(den)));
        else
            if (abs(num) == one->get_i())
                return integer(den*sgn(num));
            else
                return make_rcp<const Rational>(mpq_class(den*sgn(num), abs(num)));
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
            throw std::runtime_error("Not implemented.");
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
            return other.rpow(*this);
        }
    };

    virtual RCP<const Number> rpow(const Number &other) const {
        throw std::runtime_error("Not implemented.");
    };

    virtual void accept(Visitor &v) const;

    inline const mpq_class& get_i() const { return i; };
};

//! returns the `num` and `den` of rational `rat` as `RCP<const Integer>`
void get_num_den(const Rational &rat,
        const Ptr<RCP<const Integer>> &num,
        const Ptr<RCP<const Integer>> &den);

//! convenience creator from two longs
inline RCP<const Number> rational(long n, long d) {
    return Rational::from_two_ints(n, d);
    }
} // SymEngine

#endif
