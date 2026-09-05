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
    std::cout << poly->__str__() << "\n";

    //main algorithm
    if(is_a_Atom(*self)) return self;

    RCP<const Basic> self_num, self_denom;
    as_numer_denom(poly, outArg(self_num), outArg(self_denom));
    std::cout << self_num->__str__() << "/" << self_denom->__str__();

    try{
      RCP<const UIntPolyFlint> a = from_basic<UIntPolyFlint>(self_num, var);
      RCP<const UIntPolyFlint> b = from_basic<UIntPolyFlint>(self_denom, var);
      std::cout << a->__str__() << "/" << b->__str__();
    }
    catch(SymEngineException &e)
    {
      std::cout<<"No";
    }


    // UIntPolyFlint::container_type quotient, remainder;
    // a->get_poly().divrem(quotient, remainder, b->get_poly());

    return self;
}
}
