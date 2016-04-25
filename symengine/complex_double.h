/**
 *  \file ComplexDouble.h
 *  Class for ComplexDouble built on top of Number class
 *
 **/
#ifndef SYMENGINE_COMPLEX_DOUBLE_H
#define SYMENGINE_COMPLEX_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/number.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/real_double.h>

namespace SymEngine
{
//! Complex Double Class to hold std::complex<double> values
class ComplexDouble : public Number
{
public:
    std::complex<double> i;

public:
    IMPLEMENT_TYPEID(COMPLEX_DOUBLE)
    //! Constructor of ComplexDouble class
    explicit ComplexDouble(std::complex<double> i);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! Get the real part of the complex number
    RCP<const Number> real_part() const;
    //! Get the imaginary part of the complex number
    RCP<const Number> imaginary_part() const;
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
    //! \return self as a double
    inline std::complex<double> as_complex_double() const
    {
        return i;
    }
    //! \returns `false`
    // False is returned because std::complex<double> is not exact
    inline virtual bool is_exact() const
    {
        return false;
    }
    //! Get `Evaluate` singleton to evaluate numerically
    virtual Evaluate &get_eval() const;

    //! \return `true` if equal to `0`
    virtual bool is_zero() const
    {
        return i == 0.0;
    }
    //! \return `false`
    // A std::complex<double> is not exactly equal to `1`
    virtual bool is_one() const
    {
        return false;
    }
    //! \return `false`
    // A std::complex<double> is not exactly equal to `-1`
    virtual bool is_minus_one() const
    {
        return false;
    }

    /*! Add ComplexDoubles
     * \param other of type Integer
     * */
    RCP<const Number> addcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(i + mp_get_d(other.i));
    }

    /*! Add ComplexDoubles
     * \param other of type Rational
     * */
    RCP<const Number> addcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(i + mp_get_d(other.i));
    }

    /*! Add ComplexDoubles
     * \param other of type Complex
     * */
    RCP<const Number> addcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>(
            i + std::complex<double>(mp_get_d(other.real_),
                                     mp_get_d(other.imaginary_)));
    }

    /*! Add ComplexDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> addcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i + other.i);
    }

    /*! Add ComplexDoubles
     * \param other of type ComplexDouble
     * */
    RCP<const Number> addcomp(const ComplexDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i + other.i);
    }

    //! Converts the param `other` appropriately and then calls `addcomp`
    virtual RCP<const Number> add(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return addcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return addcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return addcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return addcomp(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return addcomp(static_cast<const ComplexDouble &>(other));
        } else {
            return other.add(*this);
        }
    }

    /*! Subtract ComplexDoubles
     * \param other of type Integer
     * */
    RCP<const Number> subcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(i - mp_get_d(other.i));
    }

    /*! Subtract ComplexDoubles
     * \param other of type Rational
     * */
    RCP<const Number> subcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(i - mp_get_d(other.i));
    }

    /*! Subtract ComplexDoubles
     * \param other of type Complex
     * */
    RCP<const Number> subcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>(
            i - std::complex<double>(mp_get_d(other.real_),
                                     mp_get_d(other.imaginary_)));
    }

    /*! Subtract ComplexDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> subcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i - other.i);
    }

    /*! Subtract ComplexDoubles
     * \param other of type ComplexDouble
     * */
    RCP<const Number> subcomp(const ComplexDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i - other.i);
    }

    //! Converts the param `other` appropriately and then calls `subcomp`
    virtual RCP<const Number> sub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return subcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return subcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return subcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return subcomp(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return subcomp(static_cast<const ComplexDouble &>(other));
        } else {
            return other.rsub(*this);
        }
    }

    /*! Subtract ComplexDoubles
     * \param other of type Integer
     * */
    RCP<const Number> rsubcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(mp_get_d(other.i) - i);
    }

    /*! Subtract ComplexDoubles
     * \param other of type Rational
     * */
    RCP<const Number> rsubcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(mp_get_d(other.i) - i);
    }

    /*! Subtract ComplexDoubles
     * \param other of type Complex
     * */
    RCP<const Number> rsubcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>(
            -i + std::complex<double>(mp_get_d(other.real_),
                                      mp_get_d(other.imaginary_)));
    }

    /*! Subtract ComplexDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> rsubcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(other.i - i);
    }

    //! Converts the param `other` appropriately and then calls `subcomp`
    virtual RCP<const Number> rsub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rsubcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rsubcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return rsubcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return rsubcomp(static_cast<const RealDouble &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    /*! Multiply ComplexDoubles
     * \param other of type Integer
     * */
    RCP<const Number> mulcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(i * mp_get_d(other.i));
    }

    /*! Multiply ComplexDoubles
     * \param other of type Rational
     * */
    RCP<const Number> mulcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(i * mp_get_d(other.i));
    }

    /*! Multiply ComplexDoubles
     * \param other of type Complex
     * */
    RCP<const Number> mulcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>(
            i * std::complex<double>(mp_get_d(other.real_),
                                     mp_get_d(other.imaginary_)));
    }

    /*! Multiply ComplexDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> mulcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i * other.i);
    }

    /*! Multiply ComplexDoubles
     * \param other of type ComplexDouble
     * */
    RCP<const Number> mulcomp(const ComplexDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i * other.i);
    }

    //! Converts the param `other` appropriately and then calls `mulcomp`
    virtual RCP<const Number> mul(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return mulcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return mulcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return mulcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return mulcomp(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return mulcomp(static_cast<const ComplexDouble &>(other));
        } else {
            return other.mul(*this);
        }
    }

    /*! Divide ComplexDoubles
     * \param other of type Integer
     * */
    RCP<const Number> divcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(i / mp_get_d(other.i));
    }

    /*! Divide ComplexDoubles
     * \param other of type Rational
     * */
    RCP<const Number> divcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(i / mp_get_d(other.i));
    }

    /*! Divide ComplexDoubles
     * \param other of type Complex
     * */
    RCP<const Number> divcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>(
            i / std::complex<double>(mp_get_d(other.real_),
                                     mp_get_d(other.imaginary_)));
    }

    /*! Divide ComplexDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> divcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i / other.i);
    }

    /*! Divide ComplexDoubles
     * \param other of type ComplexDouble
     * */
    RCP<const Number> divcomp(const ComplexDouble &other) const
    {
        return make_rcp<const ComplexDouble>(i / other.i);
    }

    //! Converts the param `other` appropriately and then calls `divcomp`
    virtual RCP<const Number> div(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return divcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return divcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return divcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return divcomp(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return divcomp(static_cast<const ComplexDouble &>(other));
        } else {
            return other.rdiv(*this);
        }
    }

    /*! Divide ComplexDoubles
     * \param other of type Integer
     * */
    RCP<const Number> rdivcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(mp_get_d(other.i) / i);
    }

    /*! Divide ComplexDoubles
     * \param other of type Rational
     * */
    RCP<const Number> rdivcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(mp_get_d(other.i) / i);
    }

    /*! Divide ComplexDoubles
     * \param other of type Complex
     * */
    RCP<const Number> rdivcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>(
            std::complex<double>(mp_get_d(other.real_),
                                 mp_get_d(other.imaginary_))
            / i);
    }

    /*! Divide ComplexDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> rdivcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(other.i / i);
    }

    //! Converts the param `other` appropriately and then calls `divcomp`
    virtual RCP<const Number> rdiv(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rdivcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rdivcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return rdivcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return rdivcomp(static_cast<const RealDouble &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    /*! Raise ComplexDouble to power `other`
     * \param other of type Integer
     * */
    RCP<const Number> powcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(i, mp_get_d(other.i)));
    }

    /*! Raise ComplexDouble to power `other`
     * \param other of type Rational
     * */
    RCP<const Number> powcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(i, mp_get_d(other.i)));
    }

    /*! Raise ComplexDouble to power `other`
     * \param other of type Complex
     * */
    RCP<const Number> powcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>((std::complex<double>)std::pow(
            i, std::complex<double>(mp_get_d(other.real_),
                                    mp_get_d(other.imaginary_))));
    }
    /*! Raise ComplexDouble to power `other`
     * \param other of type RealDouble
     * */
    RCP<const Number> powcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(i, other.i));
    }

    /*! Raise ComplexDouble to power `other`
     * \param other of type ComplexDouble
     * */
    RCP<const Number> powcomp(const ComplexDouble &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(i, other.i));
    }

    //! Converts the param `other` appropriately and then calls `powcomp`
    virtual RCP<const Number> pow(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return powcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return powcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return powcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return powcomp(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return powcomp(static_cast<const ComplexDouble &>(other));
        } else {
            return other.rpow(*this);
        }
    }

    /*! Raise `other` to power ComplexDouble
     * \param other of type Integer
     * */
    RCP<const Number> rpowcomp(const Integer &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(mp_get_d(other.i), i));
    }

    /*! Raise `other` to power ComplexDouble
     * \param other of type Rational
     * */
    RCP<const Number> rpowcomp(const Rational &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(mp_get_d(other.i), i));
    }

    /*! Raise `other` to power ComplexDouble
     * \param other of type Complex
     * */
    RCP<const Number> rpowcomp(const Complex &other) const
    {
        return make_rcp<const ComplexDouble>((std::complex<double>)std::pow(
            std::complex<double>(mp_get_d(other.real_),
                                 mp_get_d(other.imaginary_)),
            i));
    }

    /*! Raise `other` to power ComplexDouble
     * \param other of type RealDouble
     * */
    RCP<const Number> rpowcomp(const RealDouble &other) const
    {
        return make_rcp<const ComplexDouble>(
            (std::complex<double>)std::pow(other.i, i));
    }

    //! Converts the param `other` appropriately and then calls `powcomp`
    virtual RCP<const Number> rpow(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rpowcomp(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rpowcomp(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return rpowcomp(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return rpowcomp(static_cast<const RealDouble &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }
};

RCP<const ComplexDouble> complex_double(std::complex<double> x);

} // SymEngine

#endif
