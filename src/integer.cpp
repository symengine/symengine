#include "integer.h"
#include "rational.h"

using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_static_cast;

namespace CSymPy {

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

std::string Integer::__str__() const
{
    std::ostringstream s;
    s << this->i;
    return s.str();
}

signed long int Integer::as_int()
{
    // get_si() returns "signed long int", so that's what we return from
    // "as_int()" and we leave it to the user to do any possible further integer
    // conversions.
    if (!(this->i.fits_sint_p())) {
        throw std::runtime_error("as_int: Integer larger than int");
    }
    return this->i.get_si();
}

Teuchos::RCP<Number> Integer::divint(const Integer &other) const {
    if (other.i == 0)
        throw std::runtime_error("Rational: Division by zero.");
    mpq_class q(this->i, other.i);

    // This is potentially slow, but has to be done, since q might not
    // be in canonical form.
    q.canonicalize();

    return Rational::from_mpq(q);
}

Teuchos::RCP<Number> Integer::pow_negint(const Integer &other) const {
    RCP<Number> tmp = powint(*other.neg());
    if (is_a<Integer>(*tmp)) {
        mpq_class q(1, rcp_static_cast<Integer>(tmp)->i);
        return rcp(new Rational(q));
    } else {
        throw std::runtime_error("powint returned non-integer");
    }
}

// Initialize (declare) the integers -1, 0 and 1 (those are exposed in
// integer.h):
RCP<Integer> zero = rcp(new Integer(0));
RCP<Integer> one = rcp(new Integer(1));
RCP<Integer> minus_one = rcp(new Integer(-1));

} // CSymPy
