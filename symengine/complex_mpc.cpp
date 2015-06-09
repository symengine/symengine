/**
 *  \file ComplexMPC.h
 *  Class for ComplexMPC built on top of Number class
 *
 **/
#include <cmath>
#include <symengine/basic.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>
#include <symengine/complex_mpc.h>

#ifdef HAVE_SYMENGINE_MPC
namespace SymEngine {

ComplexMPC::ComplexMPC(mpc_class i) : i{std::move(i)} {

}

std::size_t ComplexMPC::__hash__() const
{
    std::hash<mpc_srcptr> hash_fn;
    return hash_fn(i.get_mpc_t());
}

bool ComplexMPC::__eq__(const Basic &o) const
{
    if (is_a<ComplexMPC>(o)) {
        const ComplexMPC &s = static_cast<const ComplexMPC &>(o);
        if (get_prec() == s.get_prec()) {
            return mpc_cmp(this->i.get_mpc_t(), s.i.get_mpc_t()) == 0;
        } else {
            return false;
        }
    }
    return false;
}

int ComplexMPC::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ComplexMPC>(o))
    const ComplexMPC &s = static_cast<const ComplexMPC &>(o);
    if (get_prec() == s.get_prec()) {
        int cmp = mpc_cmp(this->i.get_mpc_t(), s.i.get_mpc_t());
        if (cmp == 0) return 0;
        return cmp > 0 ? 1 : -1;
    } else {
        return get_prec() > s.get_prec() ? 1 : -1;
    }
}

/*! Add ComplexMPCs
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::add(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_add(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Add ComplexMPCs
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::add(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_add(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Add ComplexMPCs
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::add(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_add(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Add ComplexMPCs
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::add(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_add(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Add ComplexMPCs
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::add(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_add(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Add ComplexMPCs
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::add(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_add_fr(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Add ComplexMPCs
 * \param other of type ComplexMPC
 * */
RCP<const Number> ComplexMPC::add(const ComplexMPC &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_add(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::sub(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::sub(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::sub(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::sub(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::sub(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::sub(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_sub_fr(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}


/*! Subtract ComplexMPCs
 * \param other of type ComplexMPC
 * */
RCP<const Number> ComplexMPC::sub(const ComplexMPC &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_sub(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::rsub(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::rsub(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::rsub(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::rsub(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::rsub(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_sub(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Subtract ComplexMPCs
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::rsub(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_fr_sub(t.get_mpc_t(), other.i.get_mpfr_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::mul(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_mul(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::mul(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_mul(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::mul(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_mul(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type ComplexMPC
 * */
RCP<const Number> ComplexMPC::mul(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_mul(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::mul(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_mul(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::mul(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_mul_fr(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Multiply ComplexMPCs
 * \param other of type ComplexMPC
 * */
RCP<const Number> ComplexMPC::mul(const ComplexMPC &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_mul(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::div(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::div(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::div(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::div(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_div(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::div(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::div(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_div_fr(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type ComplexMPC
 * */
RCP<const Number> ComplexMPC::div(const ComplexMPC &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_div(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::rdiv(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::rdiv(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::rdiv(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::rdiv(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_div(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::rdiv(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_div(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Divide ComplexMPCs
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::rdiv(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_fr_div(t.get_mpc_t(), other.i.get_mpfr_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::pow(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::pow(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::pow(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::pow(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type ComplexDouble
 * */
RCP<const Number> ComplexMPC::pow(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), i.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type RealMPFR
 * */
RCP<const Number> ComplexMPC::pow(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_pow_fr(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise ComplexMPC to power `other`
 * \param other of type ComplexMPC
 * */
RCP<const Number> ComplexMPC::pow(const ComplexMPC &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_pow(t.get_mpc_t(), i.get_mpc_t(), other.i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise `other` to power ComplexMPC
 * \param other of type Integer
 * */
RCP<const Number> ComplexMPC::rpow(const Integer &other) const {
    mpc_class t(get_prec());
    mpc_set_z(t.get_mpc_t(), other.i.get_mpz_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise `other` to power ComplexMPC
 * \param other of type Rational
 * */
RCP<const Number> ComplexMPC::rpow(const Rational &other) const {
    mpc_class t(get_prec());
    mpc_set_q(t.get_mpc_t(), other.i.get_mpq_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise `other` to power ComplexMPC
 * \param other of type Complex
 * */
RCP<const Number> ComplexMPC::rpow(const Complex &other) const {
    mpc_class t(get_prec());
    mpc_set_q_q(t.get_mpc_t(), other.real_.get_mpq_t(), other.imaginary_.get_mpq_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}


/*! Raise `other` to power ComplexMPC
 * \param other of type RealDouble
 * */
RCP<const Number> ComplexMPC::rpow(const RealDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d(t.get_mpc_t(), other.i, MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise `other` to power ComplexMPC
* \param other of type ComplexDouble
* */
RCP<const Number> ComplexMPC::rpow(const ComplexDouble &other) const {
    mpc_class t(get_prec());
    mpc_set_d_d(t.get_mpc_t(), other.i.real(), other.i.imag(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

/*! Raise `other` to power ComplexMPC
* \param other of type RealMPFR
* */
RCP<const Number> ComplexMPC::rpow(const RealMPFR &other) const {
    mpc_class t(std::max(get_prec(), other.get_prec()));
    mpc_set_fr(t.get_mpc_t(), other.i.get_mpfr_t(), MPFR_RNDN);
    mpc_pow(t.get_mpc_t(), t.get_mpc_t(), i.get_mpc_t(), MPFR_RNDN);
    return rcp(new ComplexMPC(std::move(t)));
}

} // SymEngine
#endif //HAVE_SYMENGINE_MPC