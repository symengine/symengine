#include "complex.h"

namespace CSymPy {

Complex::Complex(mpq_class real, mpq_class imaginary)
    : real_{real}, imaginary_{imaginary}
{
    throw std::runtime_error("Yet to implement all virtual functions");
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

std::string Complex::__str__() const
{
    std::ostringstream s;
    s << this->real_;
    if (((imaginary_.get_den() < 0) && (imaginary_.get_num() > 0)) ||
                ((imaginary_.get_den() > 0) && (imaginary_.get_num() < 0))) {
		s << " -i";
		mpq_class q(imaginary_.get_num()*(-1), imaginary_.get_den());
		s << q;
	} else {
		s << " +i";
		s << this->imaginary_;
	}
    return s.str();
}

std::size_t Complex::__hash__() const
{
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    std::size_t seed = 0;
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

RCP<const Number> Complex::from_mpq(const mpq_class re, const mpq_class im)
{
	// It is assumed that `re` and `im` are already in canonical form.
	if (im.get_num() == 0) {
		if (im.get_den() != 0) {
			return Rational::from_mpq(re);
		} else {
			throw std::runtime_error("Divide by zero. Not implemented yet");
		}
	} else {
		//return rcp(new Complex(re, im));
		throw std::runtime_error("Yet to implement all virtual functions");
	}

}

RCP<const Number> Complex::from_two_rats(const RCP<const Rational> &re,
            const RCP<const Rational> &im)
{
    return Complex::from_mpq(re->i, im->i);
}

} // CSymPy
