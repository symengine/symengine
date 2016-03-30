/**
 *  \file ComplexMPC.h
 *  Class for ComplexMPC built on top of Number class
 *
 **/
#ifndef SYMENGINE_REAL_MPC_H
#define SYMENGINE_REAL_MPC_H

#include <symengine/basic.h>
#include <symengine/number.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>
#include <symengine/real_mpfr.h>

#ifdef HAVE_SYMENGINE_MPC
#include <mpc.h>

namespace SymEngine
{

class mpc_class
{
private:
    mpc_t mp;

public:
    mpc_ptr get_mpc_t()
    {
        return mp;
    }
    mpc_srcptr get_mpc_t() const
    {
        return mp;
    }
    explicit mpc_class(mpc_t m)
    {
        mpc_init2(mp, mpc_get_prec(m));
        mpc_set(mp, m, MPFR_RNDN);
    }
    explicit mpc_class(mpfr_prec_t prec = 53)
    {
        mpc_init2(mp, prec);
    }
    mpc_class(std::string s, mpfr_prec_t prec = 53, unsigned base = 10)
    {
        mpc_init2(mp, prec);
        mpc_set_str(mp, s.c_str(), base, MPFR_RNDN);
    }
    mpc_class(const mpc_class &other)
    {
        mpc_init2(mp, mpc_get_prec(other.get_mpc_t()));
        mpc_set(mp, other.get_mpc_t(), MPFR_RNDN);
    }
    mpc_class(mpc_class &&other)
    {
        mp->re->_mpfr_d = nullptr;
        mpc_swap(mp, other.get_mpc_t());
    }
    mpc_class &operator=(const mpc_class &other)
    {
        mpc_set_prec(mp, mpc_get_prec(other.get_mpc_t()));
        mpc_set(mp, other.get_mpc_t(), MPFR_RNDN);
        return *this;
    }
    mpc_class &operator=(mpc_class &&other)
    {
        mpc_swap(mp, other.get_mpc_t());
        return *this;
    }
    ~mpc_class()
    {
        if (mp->re->_mpfr_d != nullptr) {
            mpc_clear(mp);
        }
    }
    mpfr_prec_t get_prec() const
    {
        return mpc_get_prec(mp);
    }
};

RCP<const Number> number(mpfr_ptr x);

//! ComplexMPC Class to hold mpc_t values
class ComplexMPC : public Number
{
public:
    mpc_class i;

public:
    IMPLEMENT_TYPEID(COMPLEX_MPC)
    //! Constructor of ComplexMPC class
    ComplexMPC(mpc_class i);
    inline mpc_class as_mpc() const
    {
        return i;
    }
    inline mpfr_prec_t get_prec() const
    {
        return mpc_get_prec(i.get_mpc_t());
    }
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
    //! \return `true` if positive
    inline virtual bool is_positive() const
    {
        return false;
    }
    //! \return `true` if negative
    inline virtual bool is_negative() const
    {
        return false;
    }
    //! \return `true` if this number is an exact number
    inline virtual bool is_exact() const
    {
        return false;
    }
    //! Get `Evaluate` singleton to evaluate numerically
    virtual Evaluate &get_eval() const;

    //! \return `true` if equal to `0`
    virtual bool is_zero() const
    {
        return mpc_cmp_si_si(i.get_mpc_t(), 0, 0) == 0;
    }
    //! \return `false`
    // A mpc_t is not exactly equal to `1`
    virtual bool is_one() const
    {
        return false;
    }
    //! \return `false`
    // A mpc_t is not exactly equal to `-1`
    virtual bool is_minus_one() const
    {
        return false;
    }

    /*! Add ComplexMPCs
     * \param other of type Integer
     * */
    RCP<const Number> add(const Integer &other) const;
    RCP<const Number> add(const Rational &other) const;
    RCP<const Number> add(const Complex &other) const;
    RCP<const Number> add(const RealDouble &other) const;
    RCP<const Number> add(const ComplexDouble &other) const;
    RCP<const Number> add(const RealMPFR &other) const;
    RCP<const Number> add(const ComplexMPC &other) const;

    //! Converts the param `other` appropriately and then calls `add`
    virtual RCP<const Number> add(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return add(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return add(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return add(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return add(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return add(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return add(static_cast<const RealMPFR &>(other));
        } else if (is_a<ComplexMPC>(other)) {
            return add(static_cast<const ComplexMPC &>(other));
        } else {
            return other.add(*this);
        }
    }

    RCP<const Number> sub(const Integer &other) const;
    RCP<const Number> sub(const Rational &other) const;
    RCP<const Number> sub(const Complex &other) const;
    RCP<const Number> sub(const RealDouble &other) const;
    RCP<const Number> sub(const ComplexDouble &other) const;
    RCP<const Number> sub(const RealMPFR &other) const;
    RCP<const Number> sub(const ComplexMPC &other) const;

    //! Converts the param `other` appropriately and then calls `sub`
    virtual RCP<const Number> sub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return sub(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return sub(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return sub(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return sub(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return sub(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return sub(static_cast<const RealMPFR &>(other));
        } else if (is_a<ComplexMPC>(other)) {
            return sub(static_cast<const ComplexMPC &>(other));
        } else {
            return other.rsub(*this);
        }
    }

    RCP<const Number> rsub(const Integer &other) const;
    RCP<const Number> rsub(const Rational &other) const;
    RCP<const Number> rsub(const Complex &other) const;
    RCP<const Number> rsub(const RealDouble &other) const;
    RCP<const Number> rsub(const ComplexDouble &other) const;
    RCP<const Number> rsub(const RealMPFR &other) const;

    //! Converts the param `other` appropriately and then calls `sub`
    virtual RCP<const Number> rsub(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rsub(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rsub(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return rsub(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return rsub(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return rsub(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return rsub(static_cast<const RealMPFR &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    RCP<const Number> mul(const Integer &other) const;
    RCP<const Number> mul(const Rational &other) const;
    RCP<const Number> mul(const Complex &other) const;
    RCP<const Number> mul(const RealDouble &other) const;
    RCP<const Number> mul(const ComplexDouble &other) const;
    RCP<const Number> mul(const RealMPFR &other) const;
    RCP<const Number> mul(const ComplexMPC &other) const;

    //! Converts the param `other` appropriately and then calls `mul`
    virtual RCP<const Number> mul(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return mul(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return mul(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return mul(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return mul(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return mul(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return mul(static_cast<const RealMPFR &>(other));
        } else if (is_a<ComplexMPC>(other)) {
            return mul(static_cast<const ComplexMPC &>(other));
        } else {
            return other.mul(*this);
        }
    }

    RCP<const Number> div(const Integer &other) const;
    RCP<const Number> div(const Rational &other) const;
    RCP<const Number> div(const Complex &other) const;
    RCP<const Number> div(const RealDouble &other) const;
    RCP<const Number> div(const ComplexDouble &other) const;
    RCP<const Number> div(const RealMPFR &other) const;
    RCP<const Number> div(const ComplexMPC &other) const;

    //! Converts the param `other` appropriately and then calls `div`
    virtual RCP<const Number> div(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return div(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return div(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return div(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return div(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return div(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return div(static_cast<const RealMPFR &>(other));
        } else if (is_a<ComplexMPC>(other)) {
            return div(static_cast<const ComplexMPC &>(other));
        } else {
            return other.rdiv(*this);
        }
    }

    RCP<const Number> rdiv(const Integer &other) const;
    RCP<const Number> rdiv(const Rational &other) const;
    RCP<const Number> rdiv(const Complex &other) const;
    RCP<const Number> rdiv(const RealDouble &other) const;
    RCP<const Number> rdiv(const ComplexDouble &other) const;
    RCP<const Number> rdiv(const RealMPFR &other) const;

    //! Converts the param `other` appropriately and then calls `div`
    virtual RCP<const Number> rdiv(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rdiv(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rdiv(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return rdiv(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return rdiv(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return rdiv(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return rdiv(static_cast<const RealMPFR &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }

    RCP<const Number> pow(const Integer &other) const;
    RCP<const Number> pow(const Rational &other) const;
    RCP<const Number> pow(const Complex &other) const;
    RCP<const Number> pow(const RealDouble &other) const;
    RCP<const Number> pow(const ComplexDouble &other) const;
    RCP<const Number> pow(const RealMPFR &other) const;
    RCP<const Number> pow(const ComplexMPC &other) const;

    //! Converts the param `other` appropriately and then calls `pow`
    virtual RCP<const Number> pow(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return pow(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return pow(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return pow(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return pow(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return pow(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return pow(static_cast<const RealMPFR &>(other));
        } else if (is_a<ComplexMPC>(other)) {
            return pow(static_cast<const ComplexMPC &>(other));
        } else {
            return other.rpow(*this);
        }
    }

    RCP<const Number> rpow(const Integer &other) const;
    RCP<const Number> rpow(const Rational &other) const;
    RCP<const Number> rpow(const Complex &other) const;
    RCP<const Number> rpow(const RealDouble &other) const;
    RCP<const Number> rpow(const ComplexDouble &other) const;
    RCP<const Number> rpow(const RealMPFR &other) const;

    //! Converts the param `other` appropriately and then calls `pow`
    virtual RCP<const Number> rpow(const Number &other) const
    {
        if (is_a<Rational>(other)) {
            return rpow(static_cast<const Rational &>(other));
        } else if (is_a<Integer>(other)) {
            return rpow(static_cast<const Integer &>(other));
        } else if (is_a<Complex>(other)) {
            return rpow(static_cast<const Complex &>(other));
        } else if (is_a<RealDouble>(other)) {
            return rpow(static_cast<const RealDouble &>(other));
        } else if (is_a<ComplexDouble>(other)) {
            return rpow(static_cast<const ComplexDouble &>(other));
        } else if (is_a<RealMPFR>(other)) {
            return rpow(static_cast<const RealMPFR &>(other));
        } else {
            throw std::runtime_error("Not implemented.");
        }
    }
};

inline RCP<const ComplexMPC> complex_mpc(mpc_class x)
{
    return rcp(new ComplexMPC(std::move(x)));
}
}
#else

namespace SymEngine
{
class ComplexMPC : public Number
{
public:
    IMPLEMENT_TYPEID(COMPLEX_MPC)
};
}

#endif // HAVE_SYMENGINE_MPC
#endif // SymEngine
