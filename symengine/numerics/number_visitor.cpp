
#include <symengine/numerics/number_visitor.h>
#include <symengine/expression.h>

/* --- Explicit instantiations --- */

namespace SymEngine
{
namespace Numerics
{
template <>
class DictionarySubstitutionVisitor<int, Expression>;
template <>
class DictionarySubstitutionVisitor<float, Expression>;
template <>
class DictionarySubstitutionVisitor<double, Expression>;
template <>
class DictionarySubstitutionVisitor<std::complex<float>, Expression>;
template <>
class DictionarySubstitutionVisitor<std::complex<double>, Expression>;

template <>
class CSEDictionaryVisitor<int, Expression>;
template <>
class CSEDictionaryVisitor<float, Expression>;
template <>
class CSEDictionaryVisitor<double, Expression>;
template <>
class CSEDictionaryVisitor<std::complex<float>, Expression>;
template <>
class CSEDictionaryVisitor<std::complex<double>, Expression>;
}
}
