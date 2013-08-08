#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "mul.h"
#include "pow.h"

using Teuchos::RCP;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

RCP<Basic> expand(const RCP<Basic> &self)
{
    if (is_a<Symbol>(*self)) return self;
    if (is_a<Integer>(*self)) return self;
    if (is_a<Add>(*self)) return self;
    if (is_a<Mul>(*self)) return mul_expand(rcp_dynamic_cast<Mul>(self));
    if (is_a<Pow>(*self)) return pow_expand(rcp_dynamic_cast<Pow>(self));
    throw std::runtime_error("expand: type not implemented");
}

} // CSymPy
