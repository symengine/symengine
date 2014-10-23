/**
 *  \file visitor.h
 *  Class Visitor
 *
 **/
#ifndef CSYMPY_VISITOR_H
#define CSYMPY_VISITOR_H

#include "basic.h"

namespace CSymPy {

class Visitor {
public:
    // It should be possible to simplify this using the approach described at:
    // http://stackoverflow.com/a/11802080/479532
    // http://stackoverflow.com/a/7877397/479532
    virtual void visit(const Symbol &) { };
    virtual void visit(const Add &) { };
    virtual void visit(const Mul &) { };
    virtual void visit(const Pow &) { };
    virtual void visit(const Integer &) { };
    virtual void visit(const Rational &) { };
    virtual void visit(const Complex &) { };
    virtual void visit(const Log &) { };
    virtual void visit(const Derivative &) { };
    virtual void visit(const Sin &) { };
    virtual void visit(const Cos &) { };
    virtual void visit(const Tan &) { };
    virtual void visit(const Cot &) { };
    virtual void visit(const Csc &) { };
    virtual void visit(const Sec &) { };
    virtual void visit(const ASin &) { };
    virtual void visit(const ACos &) { };
    virtual void visit(const ASec &) { };
    virtual void visit(const ACsc &) { };
    virtual void visit(const ATan &) { };
    virtual void visit(const ACot &) { };
    virtual void visit(const ATan2 &) { };
    virtual void visit(const LambertW &) { };
    virtual void visit(const FunctionSymbol &) { };
    virtual void visit(const Sinh &) { };
    virtual void visit(const Cosh &) { };
    virtual void visit(const Tanh &) { };
    virtual void visit(const Coth &) { };
    virtual void visit(const ASinh &) { };
    virtual void visit(const ACosh &) { };
    virtual void visit(const ATanh &) { };
    virtual void visit(const ACoth &) { };
    virtual void visit(const KroneckerDelta &) { };
    virtual void visit(const LeviCivita &) { };
    virtual void visit(const Zeta &) { };
    virtual void visit(const Dirichlet_eta &) { };
    virtual void visit(const Gamma &) { };
    virtual void visit(const LowerGamma &) { };
    virtual void visit(const UpperGamma &) { };
};

class HasSymbolVisitor : public Visitor {
private:
    RCP<const Symbol> x_;
    bool has_;
public:
    // TODO: allow to return true/false from the visit() methods, and if it
    // returns false, stop the traversal in pre/postorder_traversal().
    void visit(const Symbol &x) {
        if (x_->__eq__(x)) has_ = true;
    }
    bool apply(const Basic &b, const RCP<const Symbol> &x) {
        x_ = x;
        has_ = false;
        b.preorder_traversal(*this);
        return has_;
    }
};

bool has_symbol(const Basic &b, const RCP<const Symbol> &x);

class CoeffVisitor : public Visitor {
private:
    RCP<const Symbol> x_;
    RCP<const Integer> n_;
    RCP<const Basic> coeff_;
public:
    void visit(const Add &x) {
        // TODO: Implement coeff for Add
        //coeff_ =
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

class EvalDoubleVisitor : public Visitor {
private:
    double result_;
public:
    void visit(const Sin &x);
    void visit(const Cos &x);
    void visit(const Tan &x);
    void visit(const Integer &x);
    void visit(const Rational &x);
    double apply(const Basic &b) {
        b.accept(*this);
        return result_;
    }
};

double eval_double(const Basic &b);

} // CSymPy

#endif
