#include <cmath>

#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "visitor.h"

#define ACCEPT(CLASS) void CLASS::accept(Visitor &v) const { v.visit(*this); }

namespace CSymPy {

ACCEPT(Symbol)
ACCEPT(Add)
ACCEPT(Mul)
ACCEPT(Pow)
ACCEPT(Integer)
ACCEPT(Rational)
ACCEPT(Complex)
ACCEPT(Log)
ACCEPT(Derivative)
ACCEPT(Sin)
ACCEPT(Cos)
ACCEPT(Tan)
ACCEPT(Cot)
ACCEPT(Csc)
ACCEPT(Sec)
ACCEPT(ASin)
ACCEPT(ACos)
ACCEPT(ASec)
ACCEPT(ACsc)
ACCEPT(ATan)
ACCEPT(ACot)
ACCEPT(ATan2)
ACCEPT(LambertW)
ACCEPT(FunctionSymbol)
ACCEPT(Sinh)
ACCEPT(Cosh)
ACCEPT(Tanh)
ACCEPT(Coth)
ACCEPT(ASinh)
ACCEPT(ACosh)
ACCEPT(ATanh)
ACCEPT(ACoth)
ACCEPT(KroneckerDelta)
ACCEPT(LeviCivita)
ACCEPT(Zeta)
ACCEPT(Dirichlet_eta)
ACCEPT(Gamma)
ACCEPT(LowerGamma)
ACCEPT(UpperGamma)

void Basic::preorder_traversal(Visitor &v) const
{
    accept(v);
    for (auto &p: get_args()) p->preorder_traversal(v);
}

void Basic::postorder_traversal(Visitor &v) const
{
    for (auto &p: get_args()) p->postorder_traversal(v);
    accept(v);
}

bool has_symbol(const Basic &b, const RCP<const Symbol> &x)
{
    HasSymbolVisitor v;
    return v.apply(b, x);
}

RCP<const Basic> coeff(const Basic &b, const RCP<const Symbol> &x,
        const RCP<const Integer> &n)
{
    CoeffVisitor v;
    return v.apply(b, x, n);
}

} // CSymPy

