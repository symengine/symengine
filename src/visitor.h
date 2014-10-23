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
    virtual void visit(const Symbol &) = 0;
    virtual void visit(const Add &) = 0;
    virtual void visit(const Mul &) = 0;
    virtual void visit(const Pow &) = 0;
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
    virtual void visit(const KroneckerDelta &) = 0;
    virtual void visit(const LeviCivita &) = 0;
    virtual void visit(const Zeta &) = 0;
    virtual void visit(const Dirichlet_eta &) = 0;
    virtual void visit(const Gamma &) = 0;
    virtual void visit(const LowerGamma &) = 0;
    virtual void visit(const UpperGamma &) = 0;
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
    virtual void visit(const Symbol &) { };
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

RCP<const Basic> coeff(const Basic &b, const RCP<const Symbol> &x,
        const RCP<const Integer> &n);

class EvalDoubleVisitor : public Visitor {
private:
    double result_;
public:
    void visit(const Integer &x);
    void visit(const Rational &x);
    void visit(const Add &x);
    void visit(const Mul &x);
    void visit(const Pow &x);
    void visit(const Sin &x);
    void visit(const Cos &x);
    void visit(const Tan &x);
    double apply(const Basic &b) {
        b.accept(*this);
        return result_;
    }

    virtual void visit(const Symbol &) {
        throw std::runtime_error("Symbol cannot be evaluated as a double.");
    };
    virtual void visit(const Complex &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Log &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Derivative &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Cot &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Csc &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Sec &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ASin &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ACos &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ASec &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ACsc &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ATan &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ACot &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ATan2 &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const LambertW &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const FunctionSymbol &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Sinh &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Cosh &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Tanh &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Coth &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ASinh &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ACosh &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ATanh &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const ACoth &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const KroneckerDelta &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const LeviCivita &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Zeta &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Dirichlet_eta &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Gamma &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const LowerGamma &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const UpperGamma &) {
        throw std::runtime_error("Not implemented.");
    };
};

double eval_double(const Basic &b);

} // CSymPy

#endif
