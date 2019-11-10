
#include <symengine/basic.h>
#include <symengine/polys/basic_conversions.h>

#ifdef HAVE_SYMENGINE_FLINT
using SymEngine::UIntPolyFlint;
#endif

using SymEngine::RCP;
using SymEngine::divides_upoly;

namespace SymEngine
{
// Declaration of cancel function
void cancel(const RCP<const Basic> &numer, const RCP<const Basic> &denom,
            const Ptr<RCP<const UIntPolyFlint>> &result_numer,
            const Ptr<RCP<const UIntPolyFlint>> &result_denom,
            const Ptr<RCP<const UIntPolyFlint>> &common);
}
