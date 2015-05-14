/**
 *  \file integer.h
 *  Class to implement Integers
 *
 **/

#ifndef SYMENGINE_INTEGER_H
#define SYMENGINE_INTEGER_H

#include "basic.h"
#include "number.h"

namespace SymEngine {

//! Integer Class
class Integer : public Number {
public:
    //! `i` : object of `mpz_class`
    mpz_class i;

public:
    IMPLEMENT_TYPEID(INTEGER)
    //! Constructor of Integer using `int`
    Integer(int i);
    //! Constructor of Integer using `mpz_class`
    Integer(mpz_class i);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;

    //! Convert to `int`, raise an exception if it does not fit
    signed long int as_int() const;
    //! Convert to `mpz_class`.
    inline mpz_class as_mpz() const { return this->i; }
    //! \return `true` if `0`
    inline virtual bool is_zero() const { return this->i == 0; }
    //! \return `true` if `1`
    inline virtual bool is_one() const { return this->i == 1; }
    //! \return `true` if `-1`
    inline virtual bool is_minus_one() const { return this->i == -1; }
    //! \return `true` if positive
    inline virtual bool is_positive() const { return this->i > 0; }
    //! \return `true` if negative
    inline virtual bool is_negative() const { return this->i < 0; }

    /* These are very fast methods for add/sub/mul/div/pow on Integers only */
    //! Fast Integer Addition
    inline RCP<const Integer> addint(const Integer &other) const {
        return rcp(new Integer(this->i + other.i));
    }
    //! Fast Integer Subtraction
    inline RCP<const Integer> subint(const Integer &other) const {
        return rcp(new Integer(this->i - other.i));
    }
    //! Fast Integer Multiplication
    inline RCP<const Integer> mulint(const Integer &other) const {
        return rcp(new Integer(this->i * other.i));
    }
    //!  Integer Division
    RCP<const Number> divint(const Integer &other) const;
    //! Fast Negative Power Evaluation
    RCP<const Number> pow_negint(const Integer &other) const;
    //! Fast Power Evaluation
    inline RCP<const Number> powint(const Integer &other) const {
        if (!(other.i.fits_ulong_p())) {
            if (other.i > 0)
                throw std::runtime_error("powint: 'exp' does not fit unsigned int.");
            else
                return pow_negint(other);
        }
        mpz_class tmp;
        mpz_pow_ui(tmp.get_mpz_t(), this->i.get_mpz_t(), other.i.get_ui());
        return rcp(new Integer(tmp));
    }
    //! \return negative of self.
    inline RCP<const Integer> neg() const {
        return rcp(new Integer(-i));
    }

    /* These are general methods, overriden from the Number class, that need to
     * check types to decide what operation to do, and so are a bit slower. */
    //! Slower Addition
    virtual RCP<const Number> add(const Number &other) const {
        if (is_a<Integer>(other)) {
            return addint(static_cast<const Integer&>(other));
        } else {
            return other.add(*this);
        }
    };
    //! Slower Subtraction
    virtual RCP<const Number> sub(const Number &other) const {
        if (is_a<Integer>(other)) {
            return subint(static_cast<const Integer&>(other));
        } else {
            return other.rsub(*this);
        }
    };

    virtual RCP<const Number> rsub(const Number &other) const {
        throw std::runtime_error("Not implemented.");
    };

    //! Slower Multiplication
    virtual RCP<const Number> mul(const Number &other) const {
        if (is_a<Integer>(other)) {
            return mulint(static_cast<const Integer&>(other));
        } else {
            return other.mul(*this);
        }
    };
    //! Slower Division
    virtual RCP<const Number> div(const Number &other) const {
        if (is_a<Integer>(other)) {
            return divint(static_cast<const Integer&>(other));
        } else {
            return other.rdiv(*this);
        }
    };

    virtual RCP<const Number> rdiv(const Number &other) const ;

    //! Slower power evaluation
    virtual RCP<const Number> pow(const Number &other) const {
        if (is_a<Integer>(other)) {
            return powint(static_cast<const Integer&>(other));
        } else {
            return other.rpow(*this);
        }
    };

    virtual RCP<const Number> rpow(const Number &other) const {
        throw std::runtime_error("Not implemented.");
    };

    virtual void accept(Visitor &v) const;
};

//! less operator (<) for Integers
struct RCPIntegerKeyLess
{
    //! \return true according to `<` operator
    bool operator()(const RCP<const Integer> &a,
            const RCP<const Integer> &b) const
    {
        if(mpz_cmp(b->as_mpz().get_mpz_t(), a->as_mpz().get_mpz_t()) == 1)
            return true;
        return false;
    }
};
//! \return RCP<const Integer> from `int`
inline RCP<const Integer> integer(int i)
{
    return rcp(new Integer(i));
}

//! \return RCP<const Integer> from `mpz_class`
inline RCP<const Integer> integer(mpz_class i)
{
    return rcp(new Integer(i));
}
//! Integer Square root
RCP<const Integer> isqrt(const Integer &n);
//! Integer nth root
int i_nth_root(const Ptr<RCP<const Integer>> &r, const Integer &a,
        unsigned long int n);
//! Perfect Square
int perfect_square(const Integer &n);
//! Perfect Square
int perfect_power(const Integer &n);
//! Integer Absolute value
RCP<const Integer> iabs(const Integer &n);

} // SymEngine

#endif

