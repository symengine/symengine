#include "integer.h"

using Teuchos::RCP;
using Teuchos::rcp;

namespace CSymPy {

Rational::Rational(int i)
{
    this->i = i;
}

Rational::Rational(mpz_class i)
{
    this->i = i;
}

std::size_t Rational::__hash__() const
{
    std::hash<long long int> hash_fn;
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    return hash_fn(this->i.get_si());
}

bool Rational::__eq__(const Basic &o) const
{
    if (is_a<Rational>(o)) {
        const Rational &s = static_cast<const Rational &>(o);
        return this->i == s.i;
    }
    return false;
}

std::string Rational::__str__() const
{
    std::ostringstream s;
    s << this->i;
    return s.str();
}

signed long int Rational::as_int()
{
    // get_si() returns "signed long int", so that's what we return from
    // "as_int()" and we leave it to the user to do any possible further integer
    // conversions.
    if (!(this->i.fits_sint_p())) {
        throw std::runtime_error("as_int: Rational larger than int");
    }
    return this->i.get_si();
}

// Initialize (declare) the integers -1, 0 and 1 (those are exposed in
// integer.h):
RCP<Rational> zero = rcp(new Rational(0));
RCP<Rational> one = rcp(new Rational(1));
RCP<Rational> minus_one = rcp(new Rational(-1));

} // CSymPy
