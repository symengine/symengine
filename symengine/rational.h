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

namespace SymEngine
{
//! Rational Class
class Rational : public Number
{
public:
    //! `i` : object of `rational_class`
    rational_class i;

public:
    IMPLEMENT_TYPEID(RATIONAL)
    //! Constructor of Rational class
    explicit Rational(rational_class i);
    /*! \param `i` must already be in rational_class canonical form
    *   \return Integer or Rational depending on denumerator.
    * */
    static RCP<const Number> from_mpq(rational_class i);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return true if canonical
    bool is_canonical(const rational_class &i) const;

    /*! Constructs Rational as n/d, where n, d can be any Integers. If n/d is an
    *   Integer, it will return an Integer instead.
    * */
    static RCP<const Number> from_two_ints(const Integer &n, const Integer &d);
    static RCP<const Number> from_two_ints(const long n, const long d);
    //! Convert to `rational_class`.
    inline rational_class as_mpq() const
    {
        return this->i;
    }
    //! \return `true` if `0`
    virtual bool is_zero() const
    {
        return this->i == 0;
    }
    //! \return `true` if `1`
    virtual bool is_one() const
    {
        return this->i == 1;
    }
    //! \return `true` if `-1`
    virtual bool is_minus_one() const
    {
        return this->i == -1;
    }
    //! \return `true` if denominator is `1`
    inline bool is_int() const
    {
        return this->i == 1;
    }
    //! \return `true` if positive
    inline virtual bool is_positive() const
    {
        return i > 0;
    }
    //! \return `true` if negative
    inline virtual bool is_negative() const
    {
        return i < 0;
    }

    //! \return negative of `this`
    inline RCP<const Rational> neg() const
    {
        return make_rcp<const Rational>(-i);
    }

    virtual bool is_perfect_power(bool is_expected = false) const;
    // \return true if there is a exact nth root of self.
    virtual bool nth_root(const Ptr<RCP<const Number>> &,
                          unsigned long n) const;

    /*! Add Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> addrat(const Rational &other) const
    {
        return from_mpq(std::move(this->i + other.i));
    }
    /*! Add Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> addrat(const Integer &other) const
    {
        return from_mpq(std::move(this->i + other.i));
    }
    /*! Subtract Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> subrat(const Rational &other) const
    {
        return from_mpq(std::move(this->i - other.i));
    }
    /*! Subtract Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> subrat(const Integer &other) const
    {
        return from_mpq(std::move(this->i - other.i));
    }
    inline RCP<const Number> rsubrat(const Integer &other) const
    {
        return from_mpq(std::move(other.i - this->i));
    }
    /*! Multiply Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> mulrat(const Rational &other) const
    {
        return from_mpq(std::move(this->i * other.i));
    }
    /*! Multiply Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> mulrat(const Integer &other) const
    {
        return from_mpq(std::move(this->i * other.i));
    }
    /*! Divide Rationals
     * \param other of type Rational
     * */
    inline RCP<const Number> divrat(const Rational &other) const
    {
        if (other.i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(std::move(this->i / other.i));
        }
    }
    /*! Divide Rationals
     * \param other of type Integer
     * */
    inline RCP<const Number> divrat(const Integer &other) const
    {
        if (other.i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(std::move(this->i / other.i));
        }
    }
    inline RCP<const Number> rdivrat(const Integer &other) const
    {
        if (this->i == 0) {
            throw std::runtime_error("Division by zero");
        } else {
            return from_mpq(std::move(other.i / this->i));
        }
    }
    /*! Raise Rationals to power `other`
     * \param other power to be raised
     * */
    inline RCP<const Number> powrat(const Integer &other) const
    {
        bool neg = other.is_negative();
        integer_class exp_ = other.i;
        if (neg)
            exp_ = -exp_;
        if (not mp_fits_ulong_p(exp_))
            throw std::runtime_error("powrat: 'exp' does not fit ulong.");
        unsigned long exp = mp_get_ui(exp_);
        rational_class val;
        mp_pow_ui(SymEngine::get_num(val), SymEngine::get_num(i), exp);
        mp_pow_ui(SymEngine::get_den(val), SymEngine::get_den(i), exp);

        // Since 'this' is in canonical form, so is this**other, so we simply
        // pass val into the constructor directly without canonicalizing:
        if (not neg) {
            return Rational::from_mpq(std::move(val));
        } else {
            return Rational::from_mpq(std::move(1 / val));
        }
    }
    /*! Raise Rationals to power `other`
     * \param other power to be raised
     * */
    RCP<const Basic> powrat(const Rational &other) const;
    /*! Raise Integer to power Rational
     * \param other power to be raised
     * */
    RCP<const Basic> rpowrat(const Integer &other) const;

    //! Converts the param `other` appropriately and then calls `addrat`
    virtual RCP<const Number> add(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return addrat(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return addrat(static_cast<const Integer &>(other));
        } else {
            return other.add(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `subrat`
    virtual RCP<const Number> sub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return subrat(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return subrat(static_cast<const Integer &>(other));
        } else {
            return other.rsub(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `rsubrat`
    virtual RCP<const Number> rsub(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return rsubrat(static_cast<const Integer &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
    //! Converts the param `other` appropriately and then calls `mulrat`
    virtual RCP<const Number> mul(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return mulrat(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return mulrat(static_cast<const Integer &>(other));
        } else {
            return other.mul(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `divrat`
    virtual RCP<const Number> div(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return divrat(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return divrat(static_cast<const Integer &>(other));
        } else {
            return other.rdiv(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `rdivrat`
    virtual RCP<const Number> rdiv(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return rdivrat(static_cast<const Integer &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
    //! Converts the param `other` appropriately and then calls `powrat`
    virtual RCP<const Number> pow(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return powrat(static_cast<const Integer &>(other));
        } else {
            return other.rpow(*this);
        }
    };

    virtual RCP<const Number> rpow(const Number &other) const
    {
        throw std::runtime_error("Not implemented.");
    };

    RCP<const Integer> get_num() const
    {
        return integer(SymEngine::get_num(i));
    }

    RCP<const Integer> get_den() const
    {
        return integer(SymEngine::get_den(i));
    }
};

//! returns the `num` and `den` of rational `rat` as `RCP<const Integer>`
void get_num_den(const Rational &rat, const Ptr<RCP<const Integer>> &num,
                 const Ptr<RCP<const Integer>> &den);

//! convenience creator from two longs
inline RCP<const Number> rational(long n, long d)
{
    return Rational::from_two_ints(n, d);
}
} // SymEngine

#endif
