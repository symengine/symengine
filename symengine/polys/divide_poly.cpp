#include <symengine/basic.h>
#include <symengine/polys/basic_conversions.h>

using SymEngine::RCP;
using SymEngine::UIntPolyFlint;

namespace SymEngine
{

void divide_poly(const RCP<const Basic> &dividend, const RCP<const Basic> &divisor,
            const Ptr<RCP<const UIntPolyFlint>> &quotient,
            const Ptr<RCP<const UIntPolyFlint>> &remainder)
{
    //Implementation
}
}
