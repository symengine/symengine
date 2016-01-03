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
