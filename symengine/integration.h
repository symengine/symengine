#ifndef SYMENGINE_INTEGRATION_H
#define SYMENGINE_INTEGRATION_H

#include <symengine/basic.h>
#include <symengine/visitor.h>

namespace SymEngine
{

RCP<const Basic> integrate(const RCP<const Basic> &arg,
                           const RCP<const Symbol> &x);

} // namespace SymEngine

#endif // SYMENGINE_INTEGRATION_H
