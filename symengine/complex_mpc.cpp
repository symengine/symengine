/**
 *  \file ComplexMPC.h
 *  Class for ComplexMPC built on top of Number class
 *
 **/
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

RCP<const Number> ComplexMPC::real_part() const {
    mpfr_class t(get_prec());
    mpc_real(t.get_mpfr_t(), i.get_mpc_t(), MPFR_RNDN);
    return real_mpfr(std::move(t));
}

RCP<const Number> ComplexMPC::imaginary_part() const {
    mpfr_class t(get_prec());
    mpc_imag(t.get_mpfr_t(), i.get_mpc_t(), MPFR_RNDN);
    return real_mpfr(std::move(t));
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


//! Evaluate functions with double precision
class EvaluateMPC : public Evaluate {
    virtual RCP<const Basic> sin(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_sin(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> cos(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_cos(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> tan(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_tan(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> cot(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_tan(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> sec(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_cos(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> csc(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_sin(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> asin(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_asin(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> acos(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_acos(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> atan(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_atan(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> acot(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_atan(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> asec(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_acos(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> acsc(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_asin(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> sinh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_sinh(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> cosh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_cosh(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> tanh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_tanh(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> coth(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_tanh(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> asinh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_asinh(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> acosh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_acosh(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> atanh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_atanh(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> acoth(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_ui_div(t.get_mpc_t(), 1, static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        mpc_atanh(t.get_mpc_t(), t.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> log(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpc_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_log(t.get_mpc_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return complex_mpc(std::move(t));
    }
    virtual RCP<const Basic> abs(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<ComplexMPC>(x))
        mpfr_class t(static_cast<const ComplexMPC &>(x).i.get_prec());
        mpc_abs(t.get_mpfr_t(), static_cast<const ComplexMPC &>(x).i.get_mpc_t(), MPFR_RNDN);
        return real_mpfr(std::move(t));
    }
    virtual RCP<const Basic> gamma(Basic const &aConst) const {
        throw std::runtime_error("Not Implemented.");
    }
};

Evaluate& ComplexMPC::get_eval() const
{
    static EvaluateMPC evaluate_mpc;
    return evaluate_mpc;
}


} // SymEngine
#endif //HAVE_SYMENGINE_MPC
