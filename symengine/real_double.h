/**
 *  \file RealDouble.h
 *  Class for RealDouble built on top of Number class
 *
 **/
#ifndef SYMENGINE_REAL_DOUBLE_H
#define SYMENGINE_REAL_DOUBLE_H

#include <symengine/basic.h>
#include <symengine/number.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>

namespace SymEngine {

RCP<const Number> number(std::complex<double> x);
RCP<const Number> number(double x);

//! RealDouble Class to hold double values
class RealDouble : public Number {
public:
    double i;

public:
    IMPLEMENT_TYPEID(REAL_DOUBLE)
    //! Constructor of RealDouble class
    RealDouble(double i);
    //! \return size of the hash
    virtual std::size_t __hash__() const;
    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    //! \return `true` if positive
    inline virtual bool is_positive() const {
        return i > 0;
    }
    //! \return `true` if negative
    inline virtual bool is_negative() const {
        return i < 0;
    }
    //! \return self as a double
    inline double as_double() const {
        return i;
    }
    //! \return `true` if this number is an exact number
    inline virtual bool is_exact() const { return false; }
    //! Get `Evaluate` singleton to evaluate numerically
    virtual Evaluate& get_eval() const;

    //! \return `true` when equals to 0
    virtual bool is_zero() const { return this->i == 0.0; }
    //! \return `false`
    // A double is not exactly equal to `1`
    virtual bool is_one() const { return false; }
    //! \return `false`
    // A double is not exactly equal to `-1`
    virtual bool is_minus_one() const { return false; }

    /*! Add RealDoubles
     * \param other of type Integer
     * */
    RCP<const Number> addreal(const Integer &other) const {
        return make_rcp<const RealDouble>(i + other.i.get_d());
    }

    /*! Add RealDoubles
     * \param other of type Rational
     * */
    RCP<const Number> addreal(const Rational &other) const {
        return make_rcp<const RealDouble>(i + other.i.get_d());
    }

    /*! Add RealDoubles
     * \param other of type Complex
     * */
    RCP<const Number> addreal(const Complex &other) const {
        return number(i + std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()));
    }

    /*! Add RealDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> addreal(const RealDouble &other) const {
        return make_rcp<const RealDouble>(i + other.i);
    }

    //! Converts the param `other` appropriately and then calls `addreal`
    virtual RCP<const Number> add(const Number &other) const {
        if (is_a<Rational>(other)) {
            return addreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return addreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return addreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return addreal(static_cast<const RealDouble&>(other));
        } else {
            return other.add(*this);
        }
    }

    /*! Subtract RealDoubles
     * \param other of type Integer
     * */
    RCP<const Number> subreal(const Integer &other) const {
        return make_rcp<const RealDouble>(i - other.i.get_d());
    }

    /*! Subtract RealDoubles
     * \param other of type Rational
     * */
    RCP<const Number> subreal(const Rational &other) const {
        return make_rcp<const RealDouble>(i - other.i.get_d());
    }

    /*! Subtract RealDoubles
     * \param other of type Complex
     * */
    RCP<const Number> subreal(const Complex &other) const {
        return number(i - std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()));
    }

    /*! Subtract RealDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> subreal(const RealDouble &other) const {
        return make_rcp<const RealDouble>(i - other.i);
    }

    //! Converts the param `other` appropriately and then calls `subreal`
    virtual RCP<const Number> sub(const Number &other) const {
        if (is_a<Rational>(other)) {
            return subreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return subreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return subreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return subreal(static_cast<const RealDouble&>(other));
        } else {
            return other.rsub(*this);
        }
    }

    /*! Subtract RealDoubles
     * \param other of type Integer
     * */
    RCP<const Number> rsubreal(const Integer &other) const {
        return make_rcp<const RealDouble>(other.i.get_d() - i);
    }

    /*! Subtract RealDoubles
     * \param other of type Rational
     * */
    RCP<const Number> rsubreal(const Rational &other) const {
        return make_rcp<const RealDouble>(other.i.get_d() - i);
    }

    /*! Subtract RealDoubles
     * \param other of type Complex
     * */
    RCP<const Number> rsubreal(const Complex &other) const {
        return number(-i + std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()));
    }

    //! Converts the param `other` appropriately and then calls `subreal`
    virtual RCP<const Number> rsub(const Number &other) const {
        if (is_a<Rational>(other)) {
            return rsubreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return rsubreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return rsubreal(static_cast<const Complex&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }


    /*! Multiply RealDoubles
     * \param other of type Integer
     * */
    RCP<const Number> mulreal(const Integer &other) const {
        return make_rcp<const RealDouble>(i * other.i.get_d());
    }

    /*! Multiply RealDoubles
     * \param other of type Rational
     * */
    RCP<const Number> mulreal(const Rational &other) const {
        return make_rcp<const RealDouble>(i * other.i.get_d());
    }

    /*! Multiply RealDoubles
     * \param other of type Complex
     * */
    RCP<const Number> mulreal(const Complex &other) const {
        return number(i * std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()));
    }

    /*! Multiply RealDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> mulreal(const RealDouble &other) const {
        return make_rcp<const RealDouble>(i * other.i);
    }

    //! Converts the param `other` appropriately and then calls `mulreal`
    virtual RCP<const Number> mul(const Number &other) const {
        if (is_a<Rational>(other)) {
            return mulreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return mulreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return mulreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return mulreal(static_cast<const RealDouble&>(other));
        } else {
            return other.mul(*this);
        }
    }

    /*! Divide RealDoubles
     * \param other of type Integer
     * */
    RCP<const Number> divreal(const Integer &other) const {
        return make_rcp<const RealDouble>(i / other.i.get_d());
    }

    /*! Divide RealDoubles
     * \param other of type Rational
     * */
    RCP<const Number> divreal(const Rational &other) const {
        return make_rcp<const RealDouble>(i / other.i.get_d());
    }

    /*! Divide RealDoubles
     * \param other of type Complex
     * */
    RCP<const Number> divreal(const Complex &other) const {
        return number(i / std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()));
    }

    /*! Divide RealDoubles
     * \param other of type RealDouble
     * */
    RCP<const Number> divreal(const RealDouble &other) const {
        return make_rcp<const RealDouble>(i / other.i);
    }

    //! Converts the param `other` appropriately and then calls `divreal`
    virtual RCP<const Number> div(const Number &other) const {
        if (is_a<Rational>(other)) {
            return divreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return divreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return divreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return divreal(static_cast<const RealDouble&>(other));
        } else {
            return other.rdiv(*this);
        }
    }

    /*! Divide RealDoubles
     * \param other of type Integer
     * */
    RCP<const Number> rdivreal(const Integer &other) const {
        return make_rcp<const RealDouble>(other.i.get_d() / i);
    }

    /*! Divide RealDoubles
     * \param other of type Rational
     * */
    RCP<const Number> rdivreal(const Rational &other) const {
        return make_rcp<const RealDouble>(other.i.get_d() / i);
    }

    /*! Divide RealDoubles
     * \param other of type Complex
     * */
    RCP<const Number> rdivreal(const Complex &other) const {
        return number(std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()) / i);
    }

    //! Converts the param `other` appropriately and then calls `divreal`
    virtual RCP<const Number> rdiv(const Number &other) const {
        if (is_a<Rational>(other)) {
            return rdivreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return rdivreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return rdivreal(static_cast<const Complex&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    /*! Raise RealDouble to power `other`
     * \param other of type Integer
     * */
    RCP<const Number> powreal(const Integer &other) const {
        return make_rcp<const RealDouble>(std::pow(i, other.i.get_d()));
    }

    /*! Raise RealDouble to power `other`
     * \param other of type Rational
     * */
    RCP<const Number> powreal(const Rational &other) const {
        if (i < 0) {
            return number(std::pow(std::complex<double>(i), other.i.get_d()));
        }
        return make_rcp<const RealDouble>(std::pow(i, other.i.get_d()));
    }

    /*! Raise RealDouble to power `other`
     * \param other of type Complex
     * */
    RCP<const Number> powreal(const Complex &other) const {
        return number(std::pow(i, std::complex<double>(other.real_.get_d(), other.imaginary_.get_d())));
    }

    /*! Raise RealDouble to power `other`
     * \param other of type RealDouble
     * */
    RCP<const Number> powreal(const RealDouble &other) const {
        if (i < 0) {
            return number(std::pow(std::complex<double>(i), other.i));
        }
        return make_rcp<const RealDouble>(std::pow(i, other.i));
    }

    //! Converts the param `other` appropriately and then calls `powreal`
    virtual RCP<const Number> pow(const Number &other) const {
        if (is_a<Rational>(other)) {
            return powreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return powreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return powreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return powreal(static_cast<const RealDouble&>(other));
        } else {
            return other.rpow(*this);
        }
    }

    /*! Raise `other` to power RealDouble
     * \param other of type Integer
     * */
    RCP<const Number> rpowreal(const Integer &other) const {
        if (other.is_negative()) {
            return number(std::pow(other.i.get_d(), std::complex<double>(i)));
        }
        return make_rcp<const RealDouble>(std::pow(other.i.get_d(), i));
    }

    /*! Raise `other` to power RealDouble
     * \param other of type Rational
     * */
    RCP<const Number> rpowreal(const Rational &other) const {
        if (other.is_negative()) {
            return number(std::pow(std::complex<double>(i), other.i.get_d()));
        }
        return make_rcp<const RealDouble>(std::pow(other.i.get_d(), i));
    }

    /*! Raise `other` to power RealDouble
     * \param other of type Complex
     * */
    RCP<const Number> rpowreal(const Complex &other) const {
        return number(std::pow(std::complex<double>(other.real_.get_d(), other.imaginary_.get_d()), i));
    }

    //! Converts the param `other` appropriately and then calls `powreal`
    virtual RCP<const Number> rpow(const Number &other) const {
        if (is_a<Rational>(other)) {
            return rpowreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return rpowreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return rpowreal(static_cast<const Complex&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }
};

RCP<const RealDouble> real_double(double x);

} // SymEngine

#endif
