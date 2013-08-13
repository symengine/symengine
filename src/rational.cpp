#include "rational.h"

using Teuchos::RCP;
using Teuchos::rcp;

namespace CSymPy {

Rational::Rational(mpq_class i)
{
    this->i = i;
}

Rational::Rational(mpz_class num, mpz_class den)
{
    this->i = mpq_class(num, den);
    // TODO: investigate if this is needed here:
    this->i.canonicalize();
}

bool Rational::is_canonical(const mpq_class &i)
{
    mpq_class x = i;
    x.canonicalize();
    // If 'x' is an integer, it should not be Rational:
    if (x.get_den() == 1) return false;
    return true;
}

std::size_t Rational::__hash__() const
{
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    std::size_t seed = 0;
    hash_combine<long long int>(seed, this->i.get_num().get_si());
    hash_combine<long long int>(seed, this->i.get_den().get_si());
    return seed;
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

} // CSymPy
