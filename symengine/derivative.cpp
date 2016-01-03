#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/polynomial.h>
#include <symengine/complex_double.h>
#include <symengine/complex_mpc.h>

namespace SymEngine {

class DiffImplementation {
public:
    static RCP<const Basic> diff(const Basic &self,
            const RCP<const Symbol> &x) {
        return Derivative::create(self.rcp_from_this(), {x});
    }

    static RCP<const Basic> diff(const Number &self,
            const RCP<const Symbol> &x) {
        return zero;
    }

    static RCP<const Basic> diff(const Sin &self,
            const RCP<const Symbol> &x) {
        return mul(cos(self.get_arg()), self.get_arg()->diff(x));
    }

    static RCP<const Basic> diff(const Cos &self,
            const RCP<const Symbol> &x) {
        return mul(mul(minus_one, sin(self.get_arg())),
                self.get_arg()->diff(x));
    }

    // ...and so on for all the other functions that want to return a specific
    // derivative, instead of the general diff(const Basic &, ...) one.
};

#define DIFF2(CLASS) \
RCP<const Basic> CLASS::diff2(const RCP<const Symbol> &x) const { \
    return DiffImplementation::diff(*this, x); \
};


#define SYMENGINE_ENUM(TypeID, Class) DIFF2(Class)
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM


#define IMPLEMENT_DIFF(CLASS) \
RCP<const Basic> CLASS::diff(const RCP<const Symbol> &x) const \
{ \
    return Derivative::create(rcp_from_this(), {x}); \
}

#define IMPLEMENT_DIFF0(CLASS) \
RCP<const Basic> CLASS::diff(const RCP<const Symbol> &x) const \
{ \
    return zero; \
}

IMPLEMENT_DIFF(FunctionWrapper)
IMPLEMENT_DIFF(UpperGamma)
IMPLEMENT_DIFF(LowerGamma)
IMPLEMENT_DIFF(Gamma)
IMPLEMENT_DIFF(LeviCivita)
IMPLEMENT_DIFF(KroneckerDelta)
IMPLEMENT_DIFF(Dirichlet_eta)
IMPLEMENT_DIFF(UnivariateSeries)

IMPLEMENT_DIFF0(RealDouble)
IMPLEMENT_DIFF0(ComplexDouble)
IMPLEMENT_DIFF0(Complex)
IMPLEMENT_DIFF0(Rational)
IMPLEMENT_DIFF0(Integer)
IMPLEMENT_DIFF0(NumberWrapper)


} // SymEngine
