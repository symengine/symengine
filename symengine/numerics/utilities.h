
#ifndef SYMENGINE_NUMERICS_UTILITIES_H
#define SYMENGINE_NUMERICS_UTILITIES_H

// Low level
#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/symengine_exception.h>
#include <symengine/symengine_rcp.h>

#include <complex>
#include <type_traits>

namespace SymEngine
{
namespace Numerics
{
namespace Utilities
{
/**
 * A mechanism to check if a type matches the signature of the complex
 * numbers supported by SymEngine.
 *
 * By default, it is assumed that such a @p NumberType does not satisfy
 * these criteria.
 */
template <typename NumberType>
struct is_complex : std::false_type {
};

/**
 * A mechanism to check if a type matches the signature of the complex
 * numbers supported by SymEngine.
 *
 * This specialization is for types that do match the criteria.
 */
template <typename T>
struct is_complex<std::complex<T>> : public std::true_type {
};

/**
 * Substitute the values in the @p substitution_value_map into the
 * @p expression, and then evaluate the result. The template parameter
 * @p ExpressionType must be a class that can perform evaluation by
 * casting the result of substitution to the desired @p ReturnType. The
 * Expression class. and any class derived from it. matches these requirements.
 */
template <typename ReturnType, typename ExpressionType>
ReturnType substitute_and_evaluate(
    const SymEngine::RCP<const SymEngine::Basic> &expression,
    const SymEngine::map_basic_basic &substitution_value_map)
{
    return static_cast<ReturnType>(
        ExpressionType(expression->subs(substitution_value_map)));
}

/**
 * Extract the symbols (key entries) from a SymEngine substitution map.
 *
 * @note It is guarenteed that the order of extraction of data into the
 * output vector is the same as that for extract_values().
 * That is to say that the unzipped key and value pairs as given by
 * extract_symbols() and extract_values() always have a 1:1
 * correspondence.
 */
SymEngine::vec_basic
extract_symbols(const SymEngine::map_basic_basic &substitution_values);

/**
 * Extract the values from a SymEngine substitution map.
 * The value entries will be converted into the @p NumberType given
 * as a template parameter to this function.
 *
 * @note It is guarenteed that the order of extraction of data into the
 * output vector is the same as that for extract_symbols().
 * That is to say that the unzipped key and value pairs as given by
 * extract_symbols() and extract_values() always have a 1:1
 * correspondence.
 */
template <typename NumberType, typename ExpressionType>
std::vector<NumberType>
extract_values(const SymEngine::map_basic_basic &substitution_values)
{
    std::vector<NumberType> values;
    values.reserve(substitution_values.size());

    for (const auto &it : substitution_values) {
        values.push_back(static_cast<NumberType>(ExpressionType(it.second)));
    }

    return values;
}

/**
 * Extract the values from a SymEngine substitution map.z
 * The value entries remain as the fundamental datatype used by SymEngine.
 *
 * @note It is guarenteed that the order of extraction of data into the
 * output vector is the same as that for extract_symbols().
 * That is to say that the unzipped key and value pairs as given by
 * extract_symbols() and extract_values() always have a 1:1
 * correspondence.
 */
std::vector<SymEngine::RCP<const SymEngine::Basic>>
extract_values_basic(const SymEngine::map_basic_basic &substitution_values);

} // namespace Utilities

} // namespace Numerics
} // namespace SymEngine

#endif // SYMENGINE_NUMERICS_UTILITIES_H
