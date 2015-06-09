/**
 *  \file RealMPFR.h
 *  Class for RealMPFR built on top of Number class
 *
 **/
#ifndef SYMENGINE_REAL_MPFR_H
#define SYMENGINE_REAL_MPFR_H

#include <cmath>
#include <complex>
#include <mpfr.h>
#include "basic.h"
#include "number.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "real_double.h"
#include "complex_double.h"

#ifdef HAVE_SYMENGINE_MPC
#include "mpc.h"

namespace SymEngine {

class mpc_class {
private:
    mpc_t mp;
public:
    mpc_ptr get_mpc_t() { return mp; }
    mpc_srcptr get_mpc_t() const { return mp; }
    mpc_class(mpc_t m) {
        mpc_init2(mp, mpc_get_prec(m));
        mpc_set(mp, m, MPFR_RNDN);
    }
    mpc_class(mpfr_prec_t prec = 53) {
        mpc_init2(mp, prec);
    }
    mpc_class(const mpc_class& other) {
        mpc_init2(mp, mpc_get_prec(other.get_mpc_t()));
        mpc_set(mp, other.get_mpc_t(), MPFR_RNDN);
    }
    mpc_class(mpc_class&& other) {
        mp->re->_mpfr_d = nullptr;
        mpc_swap(mp, other.get_mpc_t());
    }
    mpc_class& operator=(const mpc_class& other) {
        mpc_set_prec(mp, mpc_get_prec(other.get_mpc_t()));
        mpc_set(mp, other.get_mpc_t(), MPFR_RNDN);
        return *this;
    }
    mpc_class& operator=(mpc_class&& other) {
        mpc_swap(mp, other.get_mpc_t());
        return *this;
    }
    ~mpc_class() {
        if (mp->re->_mpfr_d != nullptr) {
            mpc_clear(mp);
        }
    }
};

RCP<const Number> number(mpfr_ptr x);

//! RealMPFR Class to hold mpc_t values
class RealMPFR : public Number {
public:
    mpc_class i;
    mpfr_rnd_t rnd_;

public:
    IMPLEMENT_TYPEID(REAL_DOUBLE)
    //! Constructor of RealMPFR class
    RealMPFR(mpc_class i, mpfr_rnd_t rnd = MPFR_RNDN);
    inline mpc_class as_mpfr() const { return i; }
    inline mpfr_prec_t get_prec() const { return mpc_get_prec(i.get_mpc_t()); }
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
        return false;
    }
    //! \return `true` if negative
    inline virtual bool is_negative() const {
        return false;
    }
    //! \return `true` if this number is an exact number
    inline virtual bool is_exact() const { return false; }
    //! Get `Evaluate` singleton to evaluate numerically
    //virtual Evaluate& get_eval() const;

    //! \return `false`
    // A mpc_t is not exactly equal to `0`
    virtual bool is_zero() const { return false; }
    //! \return `false`
    // A mpc_t is not exactly equal to `1`
    virtual bool is_one() const { return false; }
    //! \return `false`
    // A mpc_t is not exactly equal to `-1`
    virtual bool is_minus_one() const { return false; }

    /*! Add RealMPFRs
     * \param other of type Integer
     * */
    RCP<const Number> addreal(const Integer &other) const;
    RCP<const Number> addreal(const Rational &other) const;
    RCP<const Number> addreal(const Complex &other) const;
    RCP<const Number> addreal(const RealDouble &other) const;
    RCP<const Number> addreal(const ComplexDouble &other) const;
    RCP<const Number> addreal(const RealMPFR &other) const;

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
        } else if (is_a<ComplexDouble>(other)) {
            return addreal(static_cast<const ComplexDouble&>(other));
        } else if (is_a<RealMPFR>(other)) {
            return addreal(static_cast<const RealMPFR&>(other));
        } else {
            return other.add(*this);
        }
    }

    RCP<const Number> subreal(const Integer &other) const;
    RCP<const Number> subreal(const Rational &other) const;
    RCP<const Number> subreal(const Complex &other) const;
    RCP<const Number> subreal(const RealDouble &other) const;
    RCP<const Number> subreal(const ComplexDouble &other) const;
    RCP<const Number> subreal(const RealMPFR &other) const;

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
        } else if (is_a<ComplexDouble>(other)) {
            return subreal(static_cast<const ComplexDouble&>(other));
        } else if (is_a<RealMPFR>(other)) {
            return subreal(static_cast<const RealMPFR&>(other));
        } else {
            return other.rsub(*this);
        }
    }

    RCP<const Number> rsubreal(const Integer &other) const;
    RCP<const Number> rsubreal(const Rational &other) const;
    RCP<const Number> rsubreal(const Complex &other) const;
    RCP<const Number> rsubreal(const RealDouble &other) const;
    RCP<const Number> rsubreal(const ComplexDouble &other) const;

    //! Converts the param `other` appropriately and then calls `subreal`
    virtual RCP<const Number> rsub(const Number &other) const {
        if (is_a<Rational>(other)) {
            return rsubreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return rsubreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return rsubreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return rsubreal(static_cast<const RealDouble&>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return rsubreal(static_cast<const ComplexDouble&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    RCP<const Number> mulreal(const Integer &other) const;
    RCP<const Number> mulreal(const Rational &other) const;
    RCP<const Number> mulreal(const Complex &other) const;
    RCP<const Number> mulreal(const RealDouble &other) const;
    RCP<const Number> mulreal(const ComplexDouble &other) const;
    RCP<const Number> mulreal(const RealMPFR &other) const;

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
        } else if (is_a<ComplexDouble>(other)) {
            return mulreal(static_cast<const ComplexDouble&>(other));
        } else if (is_a<RealMPFR>(other)) {
            return mulreal(static_cast<const RealMPFR&>(other));
        } else {
            return other.mul(*this);
        }
    }

    RCP<const Number> divreal(const Integer &other) const;
    RCP<const Number> divreal(const Rational &other) const;
    RCP<const Number> divreal(const Complex &other) const;
    RCP<const Number> divreal(const RealDouble &other) const;
    RCP<const Number> divreal(const ComplexDouble &other) const;
    RCP<const Number> divreal(const RealMPFR &other) const;

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
        } else if (is_a<ComplexDouble>(other)) {
            return divreal(static_cast<const ComplexDouble&>(other));
        } else if (is_a<RealMPFR>(other)) {
            return divreal(static_cast<const RealMPFR&>(other));
        } else {
            return other.rdiv(*this);
        }
    }

    RCP<const Number> rdivreal(const Integer &other) const;
    RCP<const Number> rdivreal(const Rational &other) const;
    RCP<const Number> rdivreal(const Complex &other) const;
    RCP<const Number> rdivreal(const RealDouble &other) const;
    RCP<const Number> rdivreal(const ComplexDouble &other) const;

    //! Converts the param `other` appropriately and then calls `divreal`
    virtual RCP<const Number> rdiv(const Number &other) const {
        if (is_a<Rational>(other)) {
            return rdivreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return rdivreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return rdivreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return rdivreal(static_cast<const RealDouble&>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return rdivreal(static_cast<const ComplexDouble&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    RCP<const Number> powreal(const Integer &other) const;
    RCP<const Number> powreal(const Rational &other) const;
    RCP<const Number> powreal(const Complex &other) const;
    RCP<const Number> powreal(const RealDouble &other) const;
    RCP<const Number> powreal(const ComplexDouble &other) const;
    RCP<const Number> powreal(const RealMPFR &other) const;

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
        } else if (is_a<ComplexDouble>(other)) {
            return powreal(static_cast<const ComplexDouble&>(other));
        } else if (is_a<RealMPFR>(other)) {
            return powreal(static_cast<const RealMPFR&>(other));
        } else {
            return other.rpow(*this);
        }
    }

    RCP<const Number> rpowreal(const Integer &other) const;
    RCP<const Number> rpowreal(const Rational &other) const;
    RCP<const Number> rpowreal(const Complex &other) const;
    RCP<const Number> rpowreal(const RealDouble &other) const;
    RCP<const Number> rpowreal(const ComplexDouble &other) const;

    //! Converts the param `other` appropriately and then calls `powreal`
    virtual RCP<const Number> rpow(const Number &other) const {
        if (is_a<Rational>(other)) {
            return rpowreal(static_cast<const Rational&>(other));
        } else if (is_a<Integer>(other)) {
            return rpowreal(static_cast<const Integer&>(other));
        } else if (is_a<Complex>(other)) {
            return rpowreal(static_cast<const Complex&>(other));
        } else if (is_a<RealDouble>(other)) {
            return rpowreal(static_cast<const RealDouble&>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return rpowreal(static_cast<const ComplexDouble&>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    virtual void accept(Visitor &v) const;
};

inline RCP<const RealMPFR> real_mpfr(mpc_class x) {
    return rcp(new RealMPFR(std::move(x)));
}

} // SymEngine

#endif //HAVE_SYMENGINE_MPFR
#endif