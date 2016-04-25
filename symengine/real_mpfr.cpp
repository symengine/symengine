/**
 *  \file RealMPFR.h
 *  Class for RealMPFR built on top of Number class
 *
 **/
#include <symengine/basic.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/complex_mpc.h>

#ifdef HAVE_SYMENGINE_MPFR

namespace SymEngine
{

RealMPFR::RealMPFR(mpfr_class i) : i{std::move(i)}
{
}

std::size_t RealMPFR::__hash__() const
{
    std::hash<mpfr_srcptr> hash_fn;
    return hash_fn(i.get_mpfr_t());
}

bool RealMPFR::__eq__(const Basic &o) const
{
    if (is_a<RealMPFR>(o)) {
        const RealMPFR &s = static_cast<const RealMPFR &>(o);
        if (get_prec() == s.get_prec()) {
            return mpfr_cmp(this->i.get_mpfr_t(), s.i.get_mpfr_t()) == 0;
        }
    }
    return false;
}

int RealMPFR::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<RealMPFR>(o))
    const RealMPFR &s = static_cast<const RealMPFR &>(o);
    if (get_prec() == s.get_prec()) {
        int cmp = mpfr_cmp(this->i.get_mpfr_t(), s.i.get_mpfr_t());
        if (cmp == 0)
            return 0;
        return cmp > 0 ? 1 : -1;
    } else {
        return get_prec() > s.get_prec() ? 1 : -1;
    }
}

/*! Add RealMPFRs
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::addreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_add_z(t.get_mpfr_t(), i.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Add RealMPFRs
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::addreal(const Rational &other) const
{
    mpfr_class t(get_prec());
    mpfr_add_q(t.get_mpfr_t(), i.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Add RealMPFRs
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::addreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_add_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Add RealMPFRs
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::addreal(const RealDouble &other) const
{
    mpfr_class t(get_prec());
    mpfr_add_d(t.get_mpfr_t(), i.get_mpfr_t(), other.i, MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Add RealMPFRs
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::addreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_add_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Add RealMPFRs
 * \param other of type RealMPFR
 * */
RCP<const Number> RealMPFR::addreal(const RealMPFR &other) const
{
    mpfr_class t(std::max(get_prec(), other.get_prec()));
    mpfr_add(t.get_mpfr_t(), i.get_mpfr_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::subreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_sub_z(t.get_mpfr_t(), i.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::subreal(const Rational &other) const
{
    mpfr_class t(get_prec());
    mpfr_sub_q(t.get_mpfr_t(), i.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::subreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_sub_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Subtract RealMPFRs
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::subreal(const RealDouble &other) const
{
    mpfr_class t(get_prec());
    mpfr_sub_d(t.get_mpfr_t(), i.get_mpfr_t(), other.i, MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::subreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_sub_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Subtract RealMPFRs
 * \param other of type RealMPFR
 * */
RCP<const Number> RealMPFR::subreal(const RealMPFR &other) const
{
    mpfr_class t(std::max(get_prec(), other.get_prec()));
    mpfr_sub(t.get_mpfr_t(), i.get_mpfr_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::rsubreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_z_sub(t.get_mpfr_t(), get_mpz_t(other.i), i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::rsubreal(const Rational &other) const
{
    mpfr_class t(get_prec());
    mpfr_sub_q(t.get_mpfr_t(), i.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    mpfr_neg(t.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::rsubreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_fr_sub(t.get_mpc_t(), this->i.get_mpfr_t(), t.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Subtract RealMPFRs
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::rsubreal(const RealDouble &other) const
{
    mpfr_class t(get_prec());
    mpfr_d_sub(t.get_mpfr_t(), other.i, i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Subtract RealMPFRs
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::rsubreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_fr_sub(t.get_mpc_t(), this->i.get_mpfr_t(), t.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Multiply RealMPFRs
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::mulreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_mul_z(t.get_mpfr_t(), i.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Multiply RealMPFRs
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::mulreal(const Rational &other) const
{
    mpfr_class t(get_prec());
    mpfr_mul_q(t.get_mpfr_t(), i.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Multiply RealMPFRs
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::mulreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_mul_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Multiply RealMPFRs
 * \param other of type RealMPFR
 * */
RCP<const Number> RealMPFR::mulreal(const RealDouble &other) const
{
    mpfr_class t(get_prec());
    mpfr_mul_d(t.get_mpfr_t(), i.get_mpfr_t(), other.i, MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Multiply RealMPFRs
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::mulreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_mul_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Multiply RealMPFRs
 * \param other of type RealMPFR
 * */
RCP<const Number> RealMPFR::mulreal(const RealMPFR &other) const
{
    mpfr_class t(std::max(get_prec(), other.get_prec()));
    mpfr_mul(t.get_mpfr_t(), i.get_mpfr_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::divreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_div_z(t.get_mpfr_t(), i.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::divreal(const Rational &other) const
{
    mpfr_class t(get_prec());
    mpfr_div_q(t.get_mpfr_t(), i.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::divreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_div_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Divide RealMPFRs
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::divreal(const RealDouble &other) const
{
    mpfr_class t(get_prec());
    mpfr_div_d(t.get_mpfr_t(), i.get_mpfr_t(), other.i, MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::divreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_div_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Divide RealMPFRs
 * \param other of type RealMPFR
 * */
RCP<const Number> RealMPFR::divreal(const RealMPFR &other) const
{
    mpfr_class t(std::max(get_prec(), other.get_prec()));
    mpfr_div(t.get_mpfr_t(), i.get_mpfr_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::rdivreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_div_z(t.get_mpfr_t(), i.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    mpfr_pow_si(t.get_mpfr_t(), t.get_mpfr_t(), -1, MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::rdivreal(const Rational &other) const
{
    mpfr_class t(get_prec());
    mpfr_div_q(t.get_mpfr_t(), i.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    mpfr_pow_si(t.get_mpfr_t(), t.get_mpfr_t(), -1, MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::rdivreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_fr_div(t.get_mpc_t(), this->i.get_mpfr_t(), t.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Divide RealMPFRs
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::rdivreal(const RealDouble &other) const
{
    mpfr_class t(get_prec());
    mpfr_d_div(t.get_mpfr_t(), other.i, i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Divide RealMPFRs
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::rdivreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_fr_div(t.get_mpc_t(), this->i.get_mpfr_t(), t.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Raise RealMPFR to power `other`
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::powreal(const Integer &other) const
{
    mpfr_class t(get_prec());
    mpfr_pow_z(t.get_mpfr_t(), i.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise RealMPFR to power `other`
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::powreal(const Rational &other) const
{
    if (mpfr_cmp_si(i.get_mpfr_t(), 0) < 0) {
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(get_prec()), s(get_prec());
        mpc_set_q(t.get_mpc_t(), get_mpq_t(other.i), MPFR_RNDN);
        mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
        mpc_pow(t.get_mpc_t(), s.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    mpfr_class t(get_prec());
    mpfr_set_q(t.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    mpfr_pow(t.get_mpfr_t(), i.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise RealMPFR to power `other`
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::powreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_pow_fr(t.get_mpc_t(), t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Raise RealMPFR to power `other`
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::powreal(const RealDouble &other) const
{
    if (mpfr_cmp_si(i.get_mpfr_t(), 0) < 0) {
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(get_prec());
        mpc_set_fr(t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
        mpc_pow_d(t.get_mpc_t(), t.get_mpc_t(), other.i, MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    mpfr_class t(get_prec());
    mpfr_set_d(t.get_mpfr_t(), other.i, MPFR_RNDN);
    mpfr_pow(t.get_mpfr_t(), i.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise RealMPFR to power `other`
 * \param other of type ComplexDouble
 * */
RCP<const Number> RealMPFR::powreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec()), s(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), s.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Raise RealMPFR to power `other`
 * \param other of type RealMPFR
 * */
RCP<const Number> RealMPFR::powreal(const RealMPFR &other) const
{
    if (mpfr_cmp_si(i.get_mpfr_t(), 0) < 0) {
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(get_prec());
        mpc_set_fr(t.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
        mpc_pow_fr(t.get_mpc_t(), t.get_mpc_t(), other.i.get_mpfr_t(),
                   MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    mpfr_class t(std::max(get_prec(), other.get_prec()));
    mpfr_pow(t.get_mpfr_t(), i.get_mpfr_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise `other` to power RealMPFR
 * \param other of type Integer
 * */
RCP<const Number> RealMPFR::rpowreal(const Integer &other) const
{
    if (other.is_negative()) {
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(get_prec()), s(get_prec());
        mpc_set_z(t.get_mpc_t(), get_mpz_t(other.i), MPFR_RNDN);
        mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
        mpc_pow(t.get_mpc_t(), t.get_mpc_t(), s.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    mpfr_class t(get_prec());
    mpfr_set_z(t.get_mpfr_t(), get_mpz_t(other.i), MPFR_RNDN);
    mpfr_pow(t.get_mpfr_t(), t.get_mpfr_t(), i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise `other` to power RealMPFR
 * \param other of type Rational
 * */
RCP<const Number> RealMPFR::rpowreal(const Rational &other) const
{
    if (other.is_negative()) {
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(get_prec()), s(get_prec());
        mpc_set_q(t.get_mpc_t(), get_mpq_t(other.i), MPFR_RNDN);
        mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
        mpc_pow(t.get_mpc_t(), t.get_mpc_t(), s.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    mpfr_class t(get_prec());
    mpfr_set_q(t.get_mpfr_t(), get_mpq_t(other.i), MPFR_RNDN);
    mpfr_pow(t.get_mpfr_t(), t.get_mpfr_t(), i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise `other` to power RealMPFR
 * \param other of type Complex
 * */
RCP<const Number> RealMPFR::rpowreal(const Complex &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec()), s(get_prec());
    mpc_set_q_q(t.get_mpc_t(), get_mpq_t(other.real_),
                get_mpq_t(other.imaginary_), MPFR_RNDN);
    mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), s.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

/*! Raise `other` to power RealMPFR
 * \param other of type RealDouble
 * */
RCP<const Number> RealMPFR::rpowreal(const RealDouble &other) const
{
    if (mpfr_cmp_si(i.get_mpfr_t(), 0) < 0) {
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(get_prec()), s(get_prec());
        mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
        mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
        mpc_pow(t.get_mpc_t(), t.get_mpc_t(), s.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    mpfr_class t(get_prec());
    mpfr_set_d(t.get_mpfr_t(), other.i, MPFR_RNDN);
    mpfr_pow(t.get_mpfr_t(), t.get_mpfr_t(), i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new RealMPFR(std::move(t)));
}

/*! Raise `other` to power RealMPFR
* \param other of type ComplexDouble
* */
RCP<const Number> RealMPFR::rpowreal(const ComplexDouble &other) const
{
#ifdef HAVE_SYMENGINE_MPC
    mpc_class t(get_prec()), s(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_set_fr(s.get_mpc_t(), this->i.get_mpfr_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), s.get_mpc_t(), MPFR_RNDN);
    return complex_mpc(std::move(t));
#else
    throw std::runtime_error("Result is complex. Recompile with MPC support.");
#endif
}

//! Evaluate functions with double precision
class EvaluateMPFR : public Evaluate
{
    virtual RCP<const Basic> sin(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_sin(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> cos(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_cos(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> tan(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_tan(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> cot(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_cot(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> sec(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_sec(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> csc(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_csc(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> asin(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) <= 0 and mpfr_cmp_si(x_, -1) >= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_asin(t.get_mpfr_t(), x_, MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_fr(t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_asin(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> acos(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) <= 0 and mpfr_cmp_si(x_, -1) >= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_acos(t.get_mpfr_t(), x_, MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_fr(t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_acos(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> atan(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_atan(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> acot(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_ui_div(t.get_mpfr_t(), 1,
                    static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        mpfr_atan(t.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> asec(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) >= 0 or mpfr_cmp_si(x_, -1) <= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_ui_div(t.get_mpfr_t(), 1, x_, MPFR_RNDN);
            mpfr_acos(t.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_ui(t.get_mpc_t(), 1, MPFR_RNDN);
        mpc_div_fr(t.get_mpc_t(), t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_acos(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> acsc(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) >= 0 or mpfr_cmp_si(x_, -1) <= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_ui_div(t.get_mpfr_t(), 1, x_, MPFR_RNDN);
            mpfr_asin(t.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_ui(t.get_mpc_t(), 1, MPFR_RNDN);
        mpc_div_fr(t.get_mpc_t(), t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_asin(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> sinh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_sinh(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> csch(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_csch(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> cosh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_cosh(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> sech(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_sech(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> tanh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_tanh(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> coth(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_coth(t.get_mpfr_t(),
                  static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> asinh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_asinh(t.get_mpfr_t(),
                   static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> acsch(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        mpfr_class t(mpfr_get_prec(x_));
        mpfr_ui_div(t.get_mpfr_t(), 1, t.get_mpfr_t(), MPFR_RNDN);
        mpfr_asinh(t.get_mpfr_t(), x_, MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> acosh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) >= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_acosh(t.get_mpfr_t(), t.get_mpfr_t(), MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_ui(t.get_mpc_t(), 1, MPFR_RNDN);
        mpc_acosh(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> atanh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) <= 0 and mpfr_cmp_si(x_, -1) >= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_atanh(t.get_mpfr_t(), x_, MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_fr(t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_atanh(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> acoth(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 1) >= 0 or mpfr_cmp_si(x_, -1) <= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_ui_div(t.get_mpfr_t(), 1, t.get_mpfr_t(), MPFR_RNDN);
            mpfr_atanh(t.get_mpfr_t(), x_, MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_fr(t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_ui_div(t.get_mpc_t(), 1, t.get_mpc_t(), MPFR_RNDN);
        mpc_atanh(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> log(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 0) >= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_log(t.get_mpfr_t(), x_, MPFR_RNDN);
            return real_mpfr(std::move(t));
        }
#ifdef HAVE_SYMENGINE_MPC
        mpc_class t(mpfr_get_prec(x_));
        mpc_set_fr(t.get_mpc_t(), x_, MPFR_RNDN);
        mpc_log(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
#else
        throw std::runtime_error(
            "Result is complex. Recompile with MPC support.");
#endif
    }
    virtual RCP<const Basic> abs(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_class t(static_cast<const RealMPFR &>(x).i.get_prec());
        mpfr_abs(t.get_mpfr_t(),
                 static_cast<const RealMPFR &>(x).i.get_mpfr_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }

    virtual RCP<const Basic> gamma(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealMPFR>(x))
        mpfr_srcptr x_ = static_cast<const RealMPFR &>(x).i.get_mpfr_t();
        if (mpfr_cmp_si(x_, 0) >= 0) {
            mpfr_class t(mpfr_get_prec(x_));
            mpfr_gamma(t.get_mpfr_t(), x_, MPFR_RNDN);
            return real_mpfr(std::move(t));
        } else {
            throw std::runtime_error("Not Implemented.");
        }
    }
};

Evaluate &RealMPFR::get_eval() const
{
    static EvaluateMPFR evaluate_mpfr;
    return evaluate_mpfr;
}

} // SymEngine
#endif // HAVE_SYMENGINE_MPFR
