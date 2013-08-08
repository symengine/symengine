#include "integer.h"

using Teuchos::RCP;
using Teuchos::rcp;

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

// Initialize (declare) the integers 0 and 1 (those are exposed in integer.h):
RCP<Integer> zero = rcp(new Integer(0));
RCP<Integer> one = rcp(new Integer(1));

} // CSymPy
