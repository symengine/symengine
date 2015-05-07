#include "integer.h"
#include "rational.h"

namespace SymEngine {

Integer::Integer(int i)
{
    this->i = i;
}

Integer::Integer(mpz_class i)
{
    this->i = i;
}

std::size_t Integer::__hash__() const
{
    std::hash<long long int> hash_fn;
    // only the least significant bits that fit into "signed long int" are
    // hashed:
    return hash_fn(this->i.get_si());
}

bool Integer::__eq__(const Basic &o) const
{
    if (is_a<Integer>(o)) {
        const Integer &s = static_cast<const Integer &>(o);
        return this->i == s.i;
    }
    return false;
}

int Integer::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Integer>(o))
    const Integer &s = static_cast<const Integer &>(o);
    if (i == s.i) return 0;
    return i < s.i ? -1 : 1;
}

signed long int Integer::as_int() const
{
    // get_si() returns "signed long int", so that's what we return from
    // "as_int()" and we leave it to the user to do any possible further integer
    // conversions.
    if (!(this->i.fits_sint_p())) {
        throw std::runtime_error("as_int: Integer larger than int");
    }
    return this->i.get_si();
}

RCP<const Number> Integer::divint(const Integer &other) const {
    if (other.i == 0)
        throw std::runtime_error("Rational: Division by zero.");
    mpq_class q(this->i, other.i);

    // This is potentially slow, but has to be done, since q might not
    // be in canonical form.
    q.canonicalize();

    return Rational::from_mpq(q);
}

RCP<const Number> Integer::rdiv(const Number &other) const
{
    if (is_a<Integer>(other)) {
        if (this->i == 0) {
            throw std::runtime_error("Rational: Division by zero.");
        }
        mpq_class q((static_cast<const Integer&>(other)).i, this->i);

        // This is potentially slow, but has to be done, since q might not
        // be in canonical form.
        q.canonicalize();

        return Rational::from_mpq(q);
    } else {
        throw std::runtime_error("Not implemented.");
    }
};

RCP<const Number> Integer::pow_negint(const Integer &other) const {
    RCP<const Number> tmp = powint(*other.neg());
    if (is_a<Integer>(*tmp)) {
        mpq_class q(sgn(rcp_static_cast<const Integer>(tmp)->i), abs(rcp_static_cast<const Integer>(tmp)->i));
        return rcp(new Rational(q));
    } else {
        throw std::runtime_error("powint returned non-integer");
    }
}

RCP<const Integer> isqrt(const Integer &n)
{
    mpz_class m;
    mpz_t m_t;

    mpz_init(m_t);
    mpz_sqrt(m_t, n.as_mpz().get_mpz_t());
    m = mpz_class(m_t);

    mpz_clear(m_t);

    return integer(m);
}

RCP<const Integer> iabs(const Integer &n)
{
    mpz_class m;
    mpz_t m_t;

    mpz_init(m_t);
    mpz_abs(m_t, n.as_mpz().get_mpz_t());
    m = mpz_class(m_t);

    mpz_clear(m_t);

    return integer(mpz_class(m));
}

int i_nth_root(const Ptr<RCP<const Integer>> &r, const Integer &a,
        unsigned long int n)
{
    if (n == 0)
        throw std::runtime_error("i_nth_root: Can not find Zeroth root");

    int ret_val;
    mpz_t t;
    mpz_init(t);

    ret_val = mpz_root(t, a.as_mpz().get_mpz_t(), n);
    *r = integer(mpz_class(t));

    mpz_clear(t);

    return ret_val;
}

int perfect_square(const Integer &n)
{
    return mpz_perfect_square_p(n.as_mpz().get_mpz_t());
}

int perfect_power(const Integer &n)
{
    return mpz_perfect_power_p(n.as_mpz().get_mpz_t());
}

} // SymEngine

