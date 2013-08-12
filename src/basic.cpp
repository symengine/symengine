#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "rational.h"
#include "mul.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::rcp_static_cast;

namespace CSymPy {

RCP<Basic> expand(const RCP<Basic> &self)
{
    if (is_a<Symbol>(*self)) return self;
    if (is_a<Rational>(*self)) return self;
    if (is_a<Add>(*self)) return add_expand(rcp_static_cast<Add>(self));
    if (is_a<Mul>(*self)) return mul_expand(rcp_static_cast<Mul>(self));
    if (is_a<Pow>(*self)) return pow_expand(rcp_static_cast<Pow>(self));
    throw std::runtime_error("expand: type not implemented");
}

} // CSymPy
