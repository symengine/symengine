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
    virtual void visit(const Number &) { };
    virtual void visit(const Function &) { };
    virtual void visit(const Log &) { };
    virtual void visit(const Derivative &) { };
};

class HasVisitor : public Visitor {
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

} // CSymPy

#endif
