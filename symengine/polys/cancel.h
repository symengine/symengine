#ifdef SYMENGINE_UINTPOLY_FLINT_H

#include <symengine/polys/uintpoly_flint.h>
#include <symengine/basic.h>
#include <symengine/polys/basic_conversions.h>

using SymEngine::RCP;
using SymEngine::UIntPolyFlint;
using SymEngine::divides_upoly;

namespace SymEngine
{
// Declaration of cancel function
void cancel(const RCP<const Basic> &numer, const RCP<const Basic> &denom,
            const Ptr<RCP<const UIntPolyFlint>> &result_numer,
            const Ptr<RCP<const UIntPolyFlint>> &result_denom,
            const Ptr<RCP<const UIntPolyFlint>> &common);
}

#endif // SYMENGINE_UINTPOLY_FLINT_H
