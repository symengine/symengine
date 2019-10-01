#include <symengine/basic.h>
#include <symengine/polys/basic_conversions.h>

using SymEngine::RCP;

namespace SymEngine
{
RCP<const Basic> apart(const RCP<const Basic> &self, bool full = false)
{
    umap_basic_num gens = _find_gens_poly(self);

    if (gens.size() != 1) {
        // only considering univariate here
        return self;
    }
    RCP<const Basic> var = gens.begin()->first;

    RCP<const UExprPoly> poly = from_basic<UExprPoly>(self, var);
    std::cout << poly->get_poly();

    return self;
}
}
