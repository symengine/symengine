#include "basic.h"
#include "mul.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

RCP<Basic> expand(const RCP<Basic> &self)
{
    if (is_a<Mul>(*self)) return mul_expand(rcp_dynamic_cast<Mul>(self));
    if (is_a<Pow>(*self)) return pow_expand(rcp_dynamic_cast<Pow>(self));
    throw std::runtime_error("expand: type not implemented");
}

} // CSymPy
