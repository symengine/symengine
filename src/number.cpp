#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "symbol.h"
#include "pow.h"
#include "rational.h"
#include "functions.h"

namespace CSymPy {

RCP<Basic> Number::diff(const RCP<Symbol> &x) const
{
    return zero;
}

} // CSymPy
