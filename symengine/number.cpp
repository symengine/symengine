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

} // SymEngine
