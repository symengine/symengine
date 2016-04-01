/**
 *  \file complex.h
 *  Class for Complex built on top of Number class
 *
 **/
#ifndef SYMENGINE_COMPLEX_H
#define SYMENGINE_COMPLEX_H

#include <symengine/basic.h>
#include <symengine/number.h>
#include <symengine/integer.h>
#include <symengine/rational.h>

namespace SymEngine
{
//! Complex Class
class Complex : public Number
{
public:
    //! `real_` : Real part of the complex Number
    //! `imaginary_` : Imaginary part of the complex Number
    // Complex Number is of the form `real + i(imaginary)`
    rational_class real_;
    rational_class imaginary_;

public:
    IMPLEMENT_TYPEID(COMPLEX)
    //! Constructor of Complex class
    Complex(rational_class real, rational_class imaginary);
    /*! Creates an instance of Complex if imaginary part is non-zero
     * \param `re` must already be in rational_class canonical form
     * \param `im` must already be in rational_class canonical form
     * \return Complex or Rational depending on imaginary part.
     * */
    static RCP<const Number> from_mpq(const rational_class re,
                                      const rational_class im);
    //! \return true if canonical
    bool is_canonical(const rational_class &real,
                      const rational_class &imaginary) const;
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \returns `false`
    // False is returned because complex cannot be compared with zero
    inline virtual bool is_positive() const
    {
        return false;
    }
    //! \returns `false`
    // False is returned because complex cannot be compared with zero
    inline virtual bool is_negative() const
    {
        return false;
    }

    /*! Constructs Complex from re, im. If im is 0
     * it will return a Rational instead.
     * */
    static RCP<const Number> from_two_rats(const Rational &re,
                                           const Rational &im);

    /*! Constructs Complex from re, im. If im is 0
     * it will return a Rational instead.
     * */
    static RCP<const Number> from_two_nums(const Number &re, const Number &im);

    //! \return `false` since `imaginary_` cannot be zero
    virtual bool is_zero() const
    {
        return false;
    }
    //! \return `false` since `imaginary_` cannot be zero
    virtual bool is_one() const
    {
        return false;
    }
    //! \return `false` since `imaginary_` cannot be zero
    virtual bool is_minus_one() const
    {
        return false;
    }
    // This is needed while printing inside `mul`.
    //! \return `true` if `real_`  is zero
    inline bool is_re_zero() const
    {
        return (this->real_ == 0);
    }
    /*! Add Complex
     * \param other of type Complex
     * */
    inline RCP<const Number> addcomp(const Complex &other) const
    {
        return from_mpq(this->real_ + other.real_,
                        this->imaginary_ + other.imaginary_);
    }
    /*! Add Complex
     * \param other of type Rational
     * */
    inline RCP<const Number> addcomp(const Rational &other) const
    {
        return from_mpq(this->real_ + other.i, this->imaginary_);
    }
    /*! Add Complex
     * \param other of type Integer
     * */
    inline RCP<const Number> addcomp(const Integer &other) const
    {
        return from_mpq(this->real_ + other.i, this->imaginary_);
    }

    /*! Subtract Complex
     * \param other of type Complex
     * */
    inline RCP<const Number> subcomp(const Complex &other) const
    {
        return from_mpq(this->real_ - other.real_,
                        this->imaginary_ - other.imaginary_);
    }
    /*! Subtract Complex
     * \param other of type Rational
     * */
    inline RCP<const Number> subcomp(const Rational &other) const
    {
        return from_mpq(this->real_ - other.i, this->imaginary_);
    }
    /*! Subtract Complex
     * \param other of type Integer
     * */
    inline RCP<const Number> subcomp(const Integer &other) const
    {
        return from_mpq(this->real_ - other.i, this->imaginary_);
    }
    /*! Subtract Complex from other
     * \param other of type Complex
     * */
    inline RCP<const Number> rsubcomp(const Complex &other) const
    {
        return from_mpq(other.real_ - this->real_,
                        other.imaginary_ - this->imaginary_);
    }
    /*! Subtract Complex from other
     * \param other of type Rational
     * */
    inline RCP<const Number> rsubcomp(const Rational &other) const
    {
        return from_mpq(other.i - this->real_, -this->imaginary_);
    }
    /*! Subtract Complex from other
     * \param other of type Integer
     * */
    inline RCP<const Number> rsubcomp(const Integer &other) const
    {
        return from_mpq(other.i - this->real_, -this->imaginary_);
    }

    /*! Multiply Complex
     * \param other of type Complex
     * */
    inline RCP<const Number> mulcomp(const Complex &other) const
    {
        return from_mpq(
            this->real_ * other.real_ - this->imaginary_ * other.imaginary_,
            this->real_ * other.imaginary_ + this->imaginary_ * other.real_);
    }
    /*! Multiply Complex
     * \param other of type Rational
     * */
    inline RCP<const Number> mulcomp(const Rational &other) const
    {
        return from_mpq(this->real_ * other.i, this->imaginary_ * other.i);
    }
    /*! Multiply Complex
     * \param other of type Integer
     * */
    inline RCP<const Number> mulcomp(const Integer &other) const
    {
        return from_mpq(this->real_ * other.i, this->imaginary_ * other.i);
    }

    /*! Divide Complex
     * \param other of type Complex
     * */
    inline RCP<const Number> divcomp(const Complex &other) const
    {
        rational_class conjugate
            = other.real_ * other.real_ + other.imaginary_ * other.imaginary_;
        if (get_num(conjugate) == 0) {
            throw std::runtime_error("Divide by zero.");
        } else {
            return from_mpq((this->real_ * other.real_
                             + this->imaginary_ * other.imaginary_)
                                / conjugate,
                            (-this->real_ * other.imaginary_
                             + this->imaginary_ * other.real_)
                                / conjugate);
        }
    }
    /*! Divide Complex
     * \param other of type Rational
     * */
    inline RCP<const Number> divcomp(const Rational &other) const
    {
        if (other.is_zero()) {
            throw std::runtime_error("Divide by zero.");
        } else {
            return from_mpq(this->real_ / other.i, this->imaginary_ / other.i);
        }
    }
    /*! Divide Complex
     * \param other of type Integer
     * */
    inline RCP<const Number> divcomp(const Integer &other) const
    {
        if (other.is_zero()) {
            throw std::runtime_error("Divide by zero.");
        } else {
            return from_mpq(this->real_ / other.i, this->imaginary_ / other.i);
        }
    }
    /*! Divide other by the Complex
     * \param other of type Integer
     * */
    inline RCP<const Number> rdivcomp(const Integer &other) const
    {
        rational_class conjugate
            = this->real_ * this->real_ + this->imaginary_ * this->imaginary_;
        if (get_num(conjugate) == 0) {
            throw std::runtime_error("Divide by zero.");
        } else {
            return from_mpq((this->real_ * other.i) / conjugate,
                            (this->imaginary_ * (-other.i)) / conjugate);
        }
    }
    /*! Pow Complex
     * \param other of type Integer
     * */
    RCP<const Number> powcomp(const Integer &other) const;

    //! Converts the param `other` appropriately and then calls `addcomp`
    virtual RCP<const Number> add(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return addcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return addcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return addcomp(static_cast<const Complex &>(other));
        } else {
            return other.add(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `subcomp`
    virtual RCP<const Number> sub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return subcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return subcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return subcomp(static_cast<const Complex &>(other));
        } else {
            return other.rsub(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `rsubcomp`
    virtual RCP<const Number> rsub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rsubcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rsubcomp(static_cast<const Integer &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
    //! Converts the param `other` appropriately and then calls `mulcomp`
    virtual RCP<const Number> mul(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return mulcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return mulcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return mulcomp(static_cast<const Complex &>(other));
        } else {
            return other.mul(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `divcomp`
    virtual RCP<const Number> div(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return divcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return divcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return divcomp(static_cast<const Complex &>(other));
        } else {
            return other.rdiv(*this);
        }
    };
    //! Converts the param `other` appropriately and then calls `rdivcomp`
    virtual RCP<const Number> rdiv(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return rdivcomp(static_cast<const Integer &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    };
    //! Converts the param `other` appropriately and then calls `powcomp`
    virtual RCP<const Number> pow(const Number &other) const
    {
        if (is_a<Integer>(other)) {
            return powcomp(static_cast<const Integer &>(other));
        } else {
            return other.rpow(*this);
        }
    };

    //! Get the real part of the complex number
    inline RCP<const Number> real_part() const
    {
        return Rational::from_mpq(real_);
    };

    //! Get the imaginary part of the complex number
    inline RCP<const Number> imaginary_part() const
    {
        return Rational::from_mpq(imaginary_);
    };

    virtual RCP<const Number> rpow(const Number &other) const
    {
        throw std::runtime_error("Not implemented.");
    };
};

} // SymEngine

#endif
