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
#include "constants.h"
#include "visitor.h"
#include "polynomial.h"

#define ACCEPT(CLASS) void CLASS::accept(Visitor &v) const { v.visit(*this); }

namespace SymEngine {

ACCEPT(Polynomial)
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
ACCEPT(ASech)
ACCEPT(KroneckerDelta)
ACCEPT(LeviCivita)
ACCEPT(Zeta)
ACCEPT(Dirichlet_eta)
ACCEPT(Gamma)
ACCEPT(LowerGamma)
ACCEPT(UpperGamma)
ACCEPT(Constant)
ACCEPT(Abs)
ACCEPT(Subs)
ACCEPT(RealDouble)

void preorder_traversal(const Basic &b, Visitor &v)
{
    b.accept(v);
    for (auto &p: b.get_args()) preorder_traversal(*p, v);
}

void postorder_traversal(const Basic &b, Visitor &v)
{
    for (auto &p: b.get_args()) postorder_traversal(*p, v);
    b.accept(v);
}

template<class T>
void preorder_traversal_stop(const Basic &b, StopVisitor<T> &v)
{
    b.accept(v);
    if (v.stop_) return;
    for (auto &p: b.get_args()) {
        preorder_traversal_stop(*p, v);
        if (v.stop_) return;
    }
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

class FreeSymbolsVisitor : public BaseVisitor<FreeSymbolsVisitor> {
public:
    set_basic s;
    FreeSymbolsVisitor() : BaseVisitor(this) { };

    void bvisit(const Symbol &x) {
        s.insert(rcp(&x));
    }

    void bvisit(const Subs &x) {
        set_basic set_ = free_symbols(*x.get_arg());
        for (auto &p: x.get_variables()) {
            set_.erase(p);
        }
        s.insert(set_.begin(), set_.end());
        for (auto &p: x.get_point()) {
            p->accept(*this);
        }
    }

    void bvisit(const Basic &x) {
        for (auto &p: x.get_args()) {
            p->accept(*this);
        }
    }

    set_basic apply(const Basic &b) {
        b.accept(*this);
        return s;
    }
};

set_basic free_symbols(const Basic &b) {
    FreeSymbolsVisitor visitor;
    return visitor.apply(b);
}

} // SymEngine

