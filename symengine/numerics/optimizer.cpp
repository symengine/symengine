
#include <symengine/numerics/optimizer.h>
#include <symengine/expression.h>

#include <utility>

/* --- Explicit instantiations --- */

namespace SymEngine
{
namespace Numerics
{
template <>
class BatchOptimizer<float, Expression>;
template <>
class BatchOptimizer<double, Expression>;
template <>
class BatchOptimizer<std::complex<float>, Expression>;
template <>
class BatchOptimizer<std::complex<double>, Expression>;
} // namespace Numerics
} // namespace SymEngine
