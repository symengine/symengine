
#include <symengine/numerics/utilities.h>

namespace SymEngine
{
namespace Numerics
{
namespace Utilities
{

SymEngine::vec_basic
extract_symbols(const SymEngine::map_basic_basic &substitution_values)
{
    SymEngine::vec_basic symbols;
    symbols.reserve(substitution_values.size());

    for (typename SymEngine::map_basic_basic::const_iterator it
         = substitution_values.begin();
         it != substitution_values.end(); ++it)
        symbols.push_back(it->first);

    return symbols;
}

std::vector<SymEngine::RCP<const SymEngine::Basic>>
extract_values_basic(const SymEngine::map_basic_basic &substitution_values)
{
    std::vector<SymEngine::RCP<const SymEngine::Basic>> values;
    values.reserve(substitution_values.size());

    for (typename SymEngine::map_basic_basic::const_iterator it
         = substitution_values.begin();
         it != substitution_values.end(); ++it)
        values.push_back(it->second);

    return values;
}

} // namespace Utilities

} // namespace Numerics
} // namespace SymEngine
