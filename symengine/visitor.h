/**
 *  \file visitor.h
 *  Class Visitor
 *
 **/
#ifndef SYMENGINE_VISITOR_H
#define SYMENGINE_VISITOR_H

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/polynomial.h>
#include <symengine/functions.h>
#include <symengine/symbol.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/constants.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>

namespace SymEngine {

class Visitor {
public:
    virtual void visit(const Symbol &) = 0;
    virtual void visit(const Add &) = 0;
    virtual void visit(const Mul &) = 0;
    virtual void visit(const Pow &) = 0;
    virtual void visit(const UnivariatePolynomial &) = 0;
    virtual void visit(const Integer &) = 0;
    virtual void visit(const Rational &) = 0;
    virtual void visit(const Complex &) = 0;
    virtual void visit(const Log &) = 0;
    virtual void visit(const Derivative &) = 0;
    virtual void visit(const Sin &) = 0;
    virtual void visit(const Cos &) = 0;
    virtual void visit(const Tan &) = 0;
    virtual void visit(const Cot &) = 0;
    virtual void visit(const Csc &) = 0;
    virtual void visit(const Sec &) = 0;
    virtual void visit(const ASin &) = 0;
    virtual void visit(const ACos &) = 0;
    virtual void visit(const ASec &) = 0;
    virtual void visit(const ACsc &) = 0;
    virtual void visit(const ATan &) = 0;
    virtual void visit(const ACot &) = 0;
    virtual void visit(const ATan2 &) = 0;
    virtual void visit(const LambertW &) = 0;
    virtual void visit(const FunctionSymbol &) = 0;
    virtual void visit(const Sinh &) = 0;
    virtual void visit(const Cosh &) = 0;
    virtual void visit(const Tanh &) = 0;
    virtual void visit(const Coth &) = 0;
    virtual void visit(const ASinh &) = 0;
    virtual void visit(const ACosh &) = 0;
    virtual void visit(const ATanh &) = 0;
    virtual void visit(const ACoth &) = 0;
    virtual void visit(const ASech &) = 0;
    virtual void visit(const KroneckerDelta &) = 0;
    virtual void visit(const LeviCivita &) = 0;
    virtual void visit(const Zeta &) = 0;
    virtual void visit(const Dirichlet_eta &) = 0;
    virtual void visit(const Gamma &) = 0;
    virtual void visit(const LowerGamma &) = 0;
    virtual void visit(const UpperGamma &) = 0;
    virtual void visit(const Constant &) = 0;
    virtual void visit(const Abs &) = 0;
    virtual void visit(const Subs &) = 0;
    virtual void visit(const RealDouble &) = 0;
    virtual void visit(const ComplexDouble &) = 0;
};

void preorder_traversal(const Basic &b, Visitor &v);
void postorder_traversal(const Basic &b, Visitor &v);

template<class T>
class BaseVisitor : public Visitor {

public:
    T *p_;
public:
    BaseVisitor(T* p) : p_ {p} {

    };
    virtual void visit(const Add &x) { p_->bvisit(x); };
    virtual void visit(const Symbol &x) { p_->bvisit(x); };
    virtual void visit(const Mul &x) { p_->bvisit(x); };
    virtual void visit(const Pow &x) { p_->bvisit(x); };
    virtual void visit(const UnivariatePolynomial &x) { p_->bvisit(x); };
    virtual void visit(const Integer &x) { p_->bvisit(x); };
    virtual void visit(const Rational &x) { p_->bvisit(x); };
    virtual void visit(const Complex &x) { p_->bvisit(x); };
    virtual void visit(const Log &x) { p_->bvisit(x); };
    virtual void visit(const Derivative &x) { p_->bvisit(x); };
    virtual void visit(const Sin &x) { p_->bvisit(x); };
    virtual void visit(const Cos &x) { p_->bvisit(x); };
    virtual void visit(const Tan &x) { p_->bvisit(x); };
    virtual void visit(const Cot &x) { p_->bvisit(x); };
    virtual void visit(const Csc &x) { p_->bvisit(x); };
    virtual void visit(const Sec &x) { p_->bvisit(x); };
    virtual void visit(const ASin &x) { p_->bvisit(x); };
    virtual void visit(const ACos &x) { p_->bvisit(x); };
    virtual void visit(const ASec &x) { p_->bvisit(x); };
    virtual void visit(const ACsc &x) { p_->bvisit(x); };
    virtual void visit(const ATan &x) { p_->bvisit(x); };
    virtual void visit(const ACot &x) { p_->bvisit(x); };
    virtual void visit(const ATan2 &x) { p_->bvisit(x); };
    virtual void visit(const LambertW &x) { p_->bvisit(x); };
    virtual void visit(const FunctionSymbol &x) { p_->bvisit(x); };
    virtual void visit(const Sinh &x) { p_->bvisit(x); };
    virtual void visit(const Cosh &x) { p_->bvisit(x); };
    virtual void visit(const Tanh &x) { p_->bvisit(x); };
    virtual void visit(const Coth &x) { p_->bvisit(x); };
    virtual void visit(const ASinh &x) { p_->bvisit(x); };
    virtual void visit(const ACosh &x) { p_->bvisit(x); };
    virtual void visit(const ATanh &x) { p_->bvisit(x); };
    virtual void visit(const ACoth &x) { p_->bvisit(x); };
    virtual void visit(const ASech &x) { p_->bvisit(x); };
    virtual void visit(const KroneckerDelta &x) { p_->bvisit(x); };
    virtual void visit(const LeviCivita &x) { p_->bvisit(x); };
    virtual void visit(const Zeta &x) { p_->bvisit(x); };
    virtual void visit(const Dirichlet_eta &x) { p_->bvisit(x); };
    virtual void visit(const Gamma &x) { p_->bvisit(x); };
    virtual void visit(const LowerGamma &x) { p_->bvisit(x); };
    virtual void visit(const UpperGamma &x) { p_->bvisit(x); };
    virtual void visit(const Constant &x) { p_->bvisit(x); };
    virtual void visit(const Abs &x) { p_->bvisit(x); };
    virtual void visit(const Subs &x) { p_->bvisit(x); };
    virtual void visit(const RealDouble &x) { p_->bvisit(x); };
    virtual void visit(const ComplexDouble &x) { p_->bvisit(x); };
};

template<class T>
class StopVisitor : public BaseVisitor<T> {
public:
    StopVisitor(T *p) : BaseVisitor<T>(p) { };
    bool stop_;
};

template<class T>
void preorder_traversal_stop(const Basic &b, StopVisitor<T> &v);

class HasSymbolVisitor : public StopVisitor<HasSymbolVisitor> {
private:
    RCP<const Symbol> x_;
    bool has_;
public:
    HasSymbolVisitor() : StopVisitor(this) { };

    void bvisit(const Symbol &x) {
        if (x_->__eq__(x)) {
            has_ = true;
            stop_ = true;
        }
    }

    void bvisit(const Basic &x) { };

    bool apply(const Basic &b, const RCP<const Symbol> &x) {
        x_ = x;
        has_ = false;
        stop_ = false;
        preorder_traversal_stop(b, *this);
        return has_;
    }
};

bool has_symbol(const Basic &b, const RCP<const Symbol> &x);

class CoeffVisitor : public StopVisitor<CoeffVisitor> {
private:
    RCP<const Symbol> x_;
    RCP<const Integer> n_;
    RCP<const Basic> coeff_;
public:
    CoeffVisitor() : StopVisitor(this) { };

    void bvisit(const Add &x) {
        // TODO: Implement coeff for Add
    }

    void bvisit(const Basic &x) {

    }

    RCP<const Basic> apply(const Basic &b, const RCP<const Symbol> &x,
            const RCP<const Integer> &n) {
        x_ = x;
        n_ = n;
        coeff_ = zero;
        b.accept(*this);
        return coeff_;
    }
};

RCP<const Basic> coeff(const Basic &b, const RCP<const Symbol> &x,
        const RCP<const Integer> &n);

set_basic free_symbols(const Basic &b);

} // SymEngine

#endif
