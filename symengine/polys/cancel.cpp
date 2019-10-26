#include <symengine/basic.h>
#include <symengine/polys/basic_conversions.h>

using SymEngine::RCP;
using SymEngine::divides_upoly;
using SymEngine::UIntPolyFlint;

namespace SymEngine
{
void cancel(const RCP<const Basic> &self, const RCP<const Basic> &other,
            const Ptr<RCP<const UIntPolyFlint>> &result_self,
            const Ptr<RCP<const UIntPolyFlint>> &result_other)
{
    umap_basic_num self_gens = _find_gens_poly(self);
    umap_basic_num other_gens = _find_gens_poly(other);

    if (self_gens.size() != 1 && other_gens.size() != 1) {
        // only considering univariate here
        return self;
    }
    RCP<const Basic> self_var = self_gens.begin()->first;
    RCP<const Basic> other_var = other_gens.begin()->first;

    RCP<const UIntPolyFlint> self_poly
        = from_basic<UIntPolyFlint>(self, self_var);
    RCP<const UIntPolyFlint> other_poly
        = from_basic<UIntPolyFlint>(other, other_var);
    // std::cout << self_poly->__str__() << "\n" << other_poly->__str__()
    // <<"\n";

    RCP<const UIntPolyFlint> gcd_poly = gcd_upoly(*self_poly, *other_poly);
    // std::cout << gcd_poly->__str__() << "\n";

    // RCP<const UIntPolyFlint> result_self, result_other;

    divides_upoly(*gcd_poly, *self_poly, outArg(*result_self));
    divides_upoly(*gcd_poly, *other_poly, outArg(*result_other));
    // std::cout << (*result_self)->__str__() << "result self\n" <<
    // (*result_other)->__str__() << "result other\n";
}
}
