#include "rational.h"

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

RCP<const Number> Rational::from_mpq(const mpq_class i)
{
    // If the result is an Integer, return an Integer:
    if (i.get_den() == 1) {
        return rcp(new Integer(i.get_num()));
    } else {
        return rcp(new Rational(i));
    }
}

RCP<const Number> Rational::from_two_ints(const RCP<const Integer> &n,
            const RCP<const Integer> &d)
{
    if (d->i == 0)
        throw std::runtime_error("Rational: Division by zero.");
    mpq_class q(n->i, d->i);

    // This is potentially slow, but has to be done, since 'n/d' might not be
    // in canonical form.
    q.canonicalize();

    return Rational::from_mpq(q);
}

std::size_t Rational::__hash__() const
{
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    std::size_t seed = RATIONAL;
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

int Rational::compare(const Basic &o) const
{
    CSYMPY_ASSERT(is_a<Rational>(o))
    const Rational &s = static_cast<const Rational &>(o);
    if (i == s.i) return 0;
    return i < s.i ? -1 : 1;
}

void get_num_den(const RCP<const Rational> &rat,
            const Ptr<RCP<const Integer>> &num,
            const Ptr<RCP<const Integer>> &den)
{
    *num = integer(rat->i.get_num());
    *den = integer(rat->i.get_den());
}
} // CSymPy
