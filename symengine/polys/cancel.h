
#ifndef SYMENGINE_CANCEL_H
#define SYMENGINE_CANCEL_H

#include <symengine/basic.h>
#include <symengine/polys/basic_conversions.h>

#ifdef HAVE_SYMENGINE_FLINT
using SymEngine::UIntPolyFlint;

using SymEngine::RCP;
using SymEngine::divides_upoly;

namespace SymEngine
{
// Declaration of cancel function
template <typename Poly>
void cancel(const RCP<const Basic> &numer, const RCP<const Basic> &denom,
            const Ptr<RCP<const Poly>> &result_numer,
            const Ptr<RCP<const Poly>> &result_denom,
            const Ptr<RCP<const Poly>> &common);
}
#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_CANCEL_H
