#include <symengine/rational.h>

namespace SymEngine {

Rational::Rational(mpq_class i)
    : i{i}
{
    SYMENGINE_ASSERT(is_canonical(this->i))
}

bool Rational::is_canonical(const mpq_class &i) const
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
        return integer(i.get_num());
    } else {
        return make_rcp<const Rational>(i);
    }
}

RCP<const Number> Rational::from_two_ints(const Integer &n,
            const Integer &d)
{
    if (d.i == 0)
        throw std::runtime_error("Rational: Division by zero.");
    mpq_class q(n.i, d.i);

    // This is potentially slow, but has to be done, since 'n/d' might not be
    // in canonical form.
    q.canonicalize();

    return Rational::from_mpq(q);
}

RCP<const Number> Rational::from_two_ints(long n, long d)
{
    if (d == 0)
        throw std::runtime_error("Rational: Division by zero.");
    mpq_class q(n, d);

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
    if (is_a<Rational>(o)) {
        const Rational &s = static_cast<const Rational &>(o);
        if (i == s.i) return 0;
        return i < s.i ? -1 : 1;
    }
    if (is_a<Integer>(o)) {
        const Integer &s = static_cast<const Integer &>(o);
        return i < s.i ? -1 : 1;
    }
    throw std::runtime_error("unhandled comparison of Rational");
}

void get_num_den(const Rational &rat,
            const Ptr<RCP<const Integer>> &num,
            const Ptr<RCP<const Integer>> &den)
{
    *num = integer((rat.i).get_num());
    *den = integer((rat.i).get_den());
}

bool Rational::is_perfect_power(bool is_expected) const
{
    const mpz_class &num = i.get_num();
    if (num == 0)
        return true;
    else if (num == 1)
        return mpz_perfect_power_p(i.get_den().get_mpz_t()) != 0;

    const mpz_class &den = i.get_den();
    if (den == 0)
        return mpz_perfect_power_p(num.get_mpz_t()) != 0;

    if (not is_expected) {
        if (mpz_cmpabs(num.get_mpz_t(), den.get_mpz_t()) > 0) {
            if (mpz_perfect_power_p(den.get_mpz_t()) == 0)
                return false;
        }
        else {
            if (mpz_perfect_power_p(num.get_mpz_t()) == 0)
                return false;
        }
    }
    mpz_class prod = num * den;
    return mpz_perfect_power_p(prod.get_mpz_t()) != 0;
}

bool Rational::nth_root(const Ptr<RCP<const Number>> &the_rat, unsigned int n) const
{
    mpz_class rn;
    int ret = mpz_root(rn.get_mpz_t(), i.get_num().get_mpz_t(), n);
    if (ret == 0)
        return false;
    mpz_class rd;
    ret = mpz_root(rd.get_mpz_t(), i.get_den().get_mpz_t(), n);
    if (ret == 0)
        return false;
    mpq_class res(rn, rd);
    res.canonicalize();
    *the_rat = make_rcp<const Rational>(res);
    return true;
}
} // SymEngine
