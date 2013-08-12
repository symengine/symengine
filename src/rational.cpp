#include "rational.h"

using Teuchos::RCP;
using Teuchos::rcp;

namespace CSymPy {

Rational::Rational(int i)
    : iz{i}, is_integer{true}
{
}

Rational::Rational(mpz_class i)
    : iz{i}, is_integer{true}
{
}

/*
Rational::Rational(mpq_class i)
    : i{i}, is_integer{false}
{
}
*/

/*
Rational::Rational(mpz_class num, mpz_class den)
    : is_integer{false}
{
    this->iq = mpq_class(num, den);
    // TODO: investigate if this is needed here:
    this->iq.canonicalize();
}
*/

std::size_t Rational::__hash__() const
{
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    std::size_t seed = 0;
    if (this->is_integer) {
        hash_combine<long long int>(seed, this->iz.get_si());
    } else {
        hash_combine<long long int>(seed, this->iq.get_num().get_si());
        hash_combine<long long int>(seed, this->iq.get_den().get_si());
    }
    return seed;
}

bool Rational::__eq__(const Basic &o) const
{
    if (is_a<Rational>(o)) {
        const Rational &s = static_cast<const Rational &>(o);
        if (this->is_integer && s.is_integer)
            return this->iz == s.iz;
        else
            throw std::runtime_error("Not implemented.");
    }
    return false;
}

std::string Rational::__str__() const
{
    std::ostringstream s;
    if (is_integer)
        s << this->iz;
    else
        s << this->iq;
    return s.str();
}

signed long int Rational::as_int()
{
    // get_si() returns "signed long int", so that's what we return from
    // "as_int()" and we leave it to the user to do any possible further integer
    // conversions.
    mpz_class i = this->as_mpz();
    if (!(i.fits_sint_p())) {
        throw std::runtime_error("as_int: Rational larger than int");
    }
    return i.get_si();
}

// Initialize (declare) the integers -1, 0 and 1 (those are exposed in
// integer.h):
RCP<Rational> zero = rcp(new Rational(0));
RCP<Rational> one = rcp(new Rational(1));
RCP<Rational> minus_one = rcp(new Rational(-1));

} // CSymPy
