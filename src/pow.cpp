#include <stdexcept>

#include "pow.h"
#include "symbol.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

Pow::Pow(const Teuchos::RCP<Basic> &base, const Teuchos::RCP<Basic> &exp)
{
    this->base = base;
    this->exp = exp;
}

std::size_t Pow::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *(this->base));
    hash_combine<Basic>(seed, *(this->exp));
    return seed;
}

bool Pow::__eq__(const Basic &o) const
{
    if (is_a<Pow>(o)) {
        const Pow &s = static_cast<const Pow &>(o);
        if (*(this->base) == *(s.base) && *(this->exp) == *(s.exp)) {
            return true;
        }
    }
    return false;
}

std::string Pow::__str__() const
{
    std::ostringstream o;
    o << *(this->base) << "^" << *(this->exp);
    return o.str();
}

} // CSymPy
