/**
 *  \file integer.h
 *  Class to implement Integers
 *
 **/

#ifndef SYMENGINE_INTEGER_H
#define SYMENGINE_INTEGER_H

#include <symengine/number.h>

namespace SymEngine
{

//! Integer Class
class Integer : public Number
{
public:
    //! `i` : object of `integer_class`
    integer_class i;

public:
    IMPLEMENT_TYPEID(INTEGER)
    //! Constructor of Integer using `integer_class`
    explicit Integer(integer_class i);
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
    //! Convert to `integer_class`.
    inline integer_class as_mpz() const
    {
        return this->i;
    }
    //! \return `true` if `0`
    inline virtual bool is_zero() const
    {
        return this->i == 0u;
    }
    //! \return `true` if `1`
    inline virtual bool is_one() const
    {
        return this->i == 1u;
    }
    //! \return `true` if `-1`
    inline virtual bool is_minus_one() const
    {
        return this->i == -1;
    }
    //! \return `true` if positive
    inline virtual bool is_positive() const
    {
        return this->i > 0u;
    }
    //! \return `true` if negative
    inline virtual bool is_negative() const
    {
        return this->i < 0u;
    }

    /* These are very fast methods for add/sub/mul/div/pow on Integers only */
    //! Fast Integer Addition
    inline RCP<const Integer> addint(const Integer &other) const
    {
        return make_rcp<const Integer>(this->i + other.i);
    }
    //! Fast Integer Subtraction
    inline RCP<const Integer> subint(const Integer &other) const
    {
        return make_rcp<const Integer>(this->i - other.i);
    }
    //! Fast Integer Multiplication
    inline RCP<const Integer> mulint(const Integer &other) const
    {
        return make_rcp<const Integer>(this->i * other.i);
    }
    //!  Integer Division
    RCP<const Number> divint(const Integer &other) const;
    //! Fast Negative Power Evaluation
    RCP<const Number> pow_negint(const Integer &other) const;
    //! Fast Power Evaluation
    inline RCP<const Number> powint(const Integer &other) const
    {
        if (not(mp_fits_ulong_p(other.i))) {
            if (other.i > 0u)
                throw std::runtime_error(
                    "powint: 'exp' does not fit unsigned long.");
            else
                return pow_negint(other);
        }
        integer_class tmp;
        mp_pow_ui(tmp, i, mp_get_ui(other.i));
        return make_rcp<const Integer>(std::move(tmp));
    }
    //! \return negative of self.
    inline RCP<const Integer> neg() const
    {
        return make_rcp<const Integer>(-i);
    }

    /* These are general methods, overriden from the Number class, that need to
     * check types to decide what operation to do, and so are a bit slower. */
    //! Slower Addition
    virtual RCP<const Number> add(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return addint(static_cast<const Integer &>(other));
        } else {
            return other.add(*this);
        }
    };
    //! Slower Subtraction
    virtual RCP<const Number> sub(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return subint(static_cast<const Integer &>(other));
        } else {
            return other.rsub(*this);
        }
    };

    virtual RCP<const Number> rsub(const Number &other) const
    {
        throw std::runtime_error("Not implemented.");
    };

    //! Slower Multiplication
    virtual RCP<const Number> mul(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return mulint(static_cast<const Integer &>(other));
        } else {
            return other.mul(*this);
        }
    };
    //! Slower Division
    virtual RCP<const Number> div(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return divint(static_cast<const Integer &>(other));
        } else {
            return other.rdiv(*this);
        }
    };

    virtual RCP<const Number> rdiv(const Number &other) const;

    //! Slower power evaluation
    virtual RCP<const Number> pow(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return powint(static_cast<const Integer &>(other));
        } else {
            return other.rpow(*this);
        }
    };

    virtual RCP<const Number> rpow(const Number &other) const
    {
        throw std::runtime_error("Not implemented.");
    };
};

//! less operator (<) for Integers
struct RCPIntegerKeyLess {
    //! \return true according to `<` operator
    bool operator()(const RCP<const Integer> &a,
                    const RCP<const Integer> &b) const
    {
        return a->as_mpz() < b->as_mpz();
    }
};
//! \return RCP<const Integer> from integral values
template <typename T>
inline typename std::enable_if<std::is_integral<T>::value,
                               RCP<const Integer>>::type
integer(T i)
{
    return make_rcp<const Integer>(integer_class(i));
}

//! \return RCP<const Integer> from integer_class
inline RCP<const Integer> integer(integer_class i)
{
    return make_rcp<const Integer>(std::move(i));
}

//! Integer Square root
RCP<const Integer> isqrt(const Integer &n);
//! Integer nth root
int i_nth_root(const Ptr<RCP<const Integer>> &r, const Integer &a,
               unsigned long int n);
//! Perfect Square
bool perfect_square(const Integer &n);
//! Perfect Square
bool perfect_power(const Integer &n);
//! Integer Absolute value
RCP<const Integer> iabs(const Integer &n);

inline Integer::Integer(integer_class i_) : i{std::move(i_)}
{
}

} // SymEngine

#endif
