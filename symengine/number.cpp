#include <stdexcept>

#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/symbol.h>
#include <symengine/pow.h>
#include <symengine/rational.h>
#include <symengine/functions.h>

namespace SymEngine {

RCP<const Basic> Number::diff(const RCP<const Symbol> &x) const
{
    return zero;
}

RCP<const Number> get_one_from_zero(const RCP<const Number> &x) {
    SYMENGINE_ASSERT(x->is_zero())
    return x->pow(*x);
}

} // SymEngine
