#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;

namespace CSymPy {

Sin::Sin(const Teuchos::RCP<Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Sin::is_canonical(const Teuchos::RCP<Basic> &arg)
{
    // e.g. sin(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<Integer>(arg)->is_zero())
        return false;
    // TODO: add things like sin(k*pi) etc.
    return true;
}

std::size_t Sin::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Sin::__eq__(const Basic &o) const
{
    if (is_a<Sin>(o) &&
        eq(arg_, static_cast<const Sin &>(o).arg_))
        return true;
    return false;
}

std::string Sin::__str__() const
{
    std::ostringstream o;
    o << "sin(" << *arg_ << ")";
    return o.str();
}

RCP<Basic> sin(const RCP<Basic> &arg)
{
    if (eq(arg, zero)) return zero;
    return rcp(new Sin(arg));
}


Cos::Cos(const Teuchos::RCP<Basic> &arg)
    : arg_{arg}
{
    CSYMPY_ASSERT(is_canonical(arg))
}

bool Cos::is_canonical(const Teuchos::RCP<Basic> &arg)
{
    // e.g. cos(0)
    if (is_a<Integer>(*arg) &&
            rcp_static_cast<Integer>(arg)->is_zero())
        return false;
    // TODO: add things like cos(k*pi) etc.
    return true;
}

std::size_t Cos::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Cos::__eq__(const Basic &o) const
{
    if (is_a<Cos>(o) &&
        eq(arg_, static_cast<const Cos &>(o).arg_))
        return true;
    return false;
}

std::string Cos::__str__() const
{
    std::ostringstream o;
    o << "cos(" << *arg_ << ")";
    return o.str();
}

RCP<Basic> cos(const RCP<Basic> &arg)
{
    if (eq(arg, zero)) return one;
    return rcp(new Cos(arg));
}

} // CSymPy
