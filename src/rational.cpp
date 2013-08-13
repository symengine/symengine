#include "rational.h"

using Teuchos::RCP;
using Teuchos::rcp;

namespace CSymPy {

Rational::Rational(mpq_class i)
    : i{i}
{
    CSYMPY_ASSERT(is_canonical(this->i))
}

bool Rational::is_canonical(const mpq_class &i)
{
    mpq_class x = i;
    x.canonicalize();
    // If 'x' is an integer, it should not be Rational:
    if (x.get_den() == 1) return false;
    // if 'i' is not in canonical form:
    if (x.get_num() != i.get_num()) return false;
    if (x.get_den() != i.get_den()) return false;
    return true;
}

Teuchos::RCP<Basic> Rational::from_two_ints(const Teuchos::RCP<Integer> &n,
            const Teuchos::RCP<Integer> &d)
{
    if (d->i == 0)
        throw std::runtime_error("Rational: Division by zero.");
    mpq_class q(n->i, d->i);

    // This is potentially slow, but has to be done, since 'n' and 'd' might
    // have some common divisors:
    q.canonicalize();

    // If the result is an Integer, return an Integer:
    if (q.get_den() == 1) {
        return rcp(new Integer(q.get_num()));
    } else {
        return rcp(new Rational(q));
    }
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
