#include <symengine/dict.h>

namespace SymEngine
{
// Common subexpression elimination of symbolic expressions
// Return a vector of replacement pairs and a vector of reduced exprs
typedef std::vector<std::pair<RCP<const Basic>, RCP<const Basic>>> vec_pair;
void cse(vec_pair &replacements, vec_basic &reduced_exprs,
         const vec_basic &exprs);
}
