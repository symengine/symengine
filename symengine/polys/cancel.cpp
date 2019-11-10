#include <symengine/polys/cancel.h>

namespace SymEngine
{
#ifdef HAVE_SYMENGINE_FLINT
// Cancel common factors in a rational function ``numer/denom``.
// Return null if the rational function is not univariate.
void cancel(const RCP<const Basic> &numer, const RCP<const Basic> &denom,
            const Ptr<RCP<const UIntPolyFlint>> &result_numer,
            const Ptr<RCP<const UIntPolyFlint>> &result_denom,
            const Ptr<RCP<const UIntPolyFlint>> &common)
{
    // Converting basic to UIntPolyFlint
    umap_basic_num numer_gens = _find_gens_poly(numer);
    umap_basic_num denom_gens = _find_gens_poly(denom);

    if (numer_gens.size() != 1 && denom_gens.size() != 1) {
        // only considering univariate here
        return;
    }
    RCP<const Basic> numer_var = numer_gens.begin()->first;
    RCP<const Basic> denom_var = denom_gens.begin()->first;

    RCP<const UIntPolyFlint> numer_poly
        = from_basic<UIntPolyFlint>(numer, numer_var);
    RCP<const UIntPolyFlint> denom_poly
        = from_basic<UIntPolyFlint>(denom, denom_var);

    // Finding common factors of numer_poly and denom_poly
    RCP<const UIntPolyFlint> gcd_poly = gcd_upoly(*numer_poly, *denom_poly);

    // Dividing both by common factors
    divides_upoly(*gcd_poly, *numer_poly, outArg(*result_numer));
    divides_upoly(*gcd_poly, *denom_poly, outArg(*result_denom));
    *common = gcd_poly;
}

#endif
}
