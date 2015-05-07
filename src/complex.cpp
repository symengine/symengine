#include "complex.h"
#include "constants.h"
#include "ntheory.h"

namespace SymEngine {

Complex::Complex(mpq_class real, mpq_class imaginary)
    : real_{real}, imaginary_{imaginary}
{
    CSYMPY_ASSERT(is_canonical(this->real_, this->imaginary_))
}

bool Complex::is_canonical(const mpq_class &real, const mpq_class &imaginary)
{
    mpq_class re = real;
    mpq_class im = imaginary;
    re.canonicalize();
    im.canonicalize();
    // If 'im' is 0, it should not be Complex:
    if (im.get_num() == 0) return false;
    // if 'real' or `imaginary` are not in canonical form:
    if (re.get_num() != real.get_num()) return false;
    if (re.get_den() != real.get_den()) return false;
    if (im.get_num() != imaginary.get_num()) return false;
    if (im.get_den() != imaginary.get_den()) return false;
    return true;
}

std::size_t Complex::__hash__() const
{
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    std::size_t seed = COMPLEX;
    hash_combine<long long int>(seed, this->real_.get_num().get_si());
    hash_combine<long long int>(seed, this->real_.get_den().get_si());
    hash_combine<long long int>(seed, this->imaginary_.get_num().get_si());
    hash_combine<long long int>(seed, this->imaginary_.get_den().get_si());
    return seed;
}

bool Complex::__eq__(const Basic &o) const
{
    if (is_a<Complex>(o)) {
        const Complex &s = static_cast<const Complex &>(o);
        return ((this->real_ == s.real_) && (this->imaginary_ == s.imaginary_));
    }
    return false;
}

int Complex::compare(const Basic &o) const {
    CSYMPY_ASSERT(is_a<Complex>(o))
    const Complex &s = static_cast<const Complex &>(o);
    if (real_ == s.real_) {
        if (imaginary_ == s.imaginary_) {
            return 0;
        } else {
            return imaginary_ < s.imaginary_ ? -1 : 1;
        }
    } else {
        return real_ < s.real_ ? -1 : 1;
    }
}

RCP<const Number> Complex::from_mpq(const mpq_class re, const mpq_class im)
{
    // It is assumed that `re` and `im` are already in canonical form.
    if (im.get_num() == 0) {
        return Rational::from_mpq(re);
    } else {
        return rcp(new Complex(re, im));
    }

}

RCP<const Number> Complex::from_two_rats(const Rational &re,
            const Rational &im)
{
    return Complex::from_mpq(re.i, im.i);
}

RCP<const Number> Complex::from_two_nums(const Number &re,
            const Number &im)
{
    if (is_a<Integer>(re) && is_a<Integer>(im)) {
        mpq_class re_mpq(static_cast<const Integer&>(re).i, static_cast<const Integer&>(*one).i);
        mpq_class im_mpq(static_cast<const Integer&>(im).i, static_cast<const Integer&>(*one).i);
        return Complex::from_mpq(re_mpq, im_mpq);
    } else if (is_a<Rational>(re) && is_a<Integer>(im)) {
        mpq_class re_mpq = static_cast<const Rational&>(re).i;
        mpq_class im_mpq(static_cast<const Integer&>(im).i, static_cast<const Integer&>(*one).i);
        return Complex::from_mpq(re_mpq, im_mpq);
    } else if (is_a<Integer>(re) && is_a<Rational>(im)) {
        mpq_class re_mpq(static_cast<const Integer&>(re).i, static_cast<const Integer&>(*one).i);
        mpq_class im_mpq = static_cast<const Rational&>(im).i;
        return Complex::from_mpq(re_mpq, im_mpq);
    } else if (is_a<Rational>(re) && is_a<Rational>(im)) {
        mpq_class re_mpq = static_cast<const Rational&>(re).i;
        mpq_class im_mpq = static_cast<const Rational&>(im).i;
        return Complex::from_mpq(re_mpq, im_mpq);
    } else {
        throw std::runtime_error("Invalid Format: Expected Integer or Rational");
    }
}

RCP<const Number> pow_number(const Complex &x, unsigned long n)
{
    unsigned long mask = 1;
    mpq_class r_re = 1;
    mpq_class r_im = 0;

    mpq_class p_re = x.real_;
    mpq_class p_im = x.imaginary_;

    mpq_class tmp;

    while (mask > 0 && n >= mask) {
        if (n & mask) {
            // Multiply r by p
            tmp = r_re * p_re - r_im * p_im;
            r_im = r_re * p_im + r_im * p_re;
            r_re = tmp;
        }
        mask = mask << 1;
        // Multiply p by p
        tmp = p_re * p_re - p_im * p_im;
        p_im = 2 * p_re * p_im;
        p_re = tmp;
    }
    return Complex::from_mpq(r_re, r_im);
}

RCP<const Number> Complex::powcomp(const Integer &other) const {
    if (this->is_re_zero()) {
        // Imaginary Number raised to an integer power.
        RCP<const Number> im = Rational::from_mpq(this->imaginary_);
        long rem = mod(other, *integer(4))->as_int();
        RCP<const Number> res;
        if (rem == 0) {
            res = one;
        } else if (rem == 1) {
            res = I;
        } else if (rem == 2) {
            res = minus_one;
        } else {
            res = mulnum(I, minus_one);
        }
        return mulnum(im->pow(other), res);
    } else if (other.is_positive()) {
        return pow_number(*this, other.as_int());
    } else {
        return one->div(*pow_number(*this, -1 * other.as_int()));
    }
};
} // SymEngine