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
#include "eval_double.h"

namespace CSymPy {

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

void EvalDoubleVisitor::visit(const Integer &x)
{
    double tmp = x.i.get_d();
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Rational &x)
{
    double tmp = x.i.get_d();
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Add &x)
{
    double tmp = 0;
    for (auto &p: x.get_args()) tmp = tmp + eval_double(*p);
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Mul &x)
{
    double tmp = 1;
    for (auto &p: x.get_args()) tmp = tmp * eval_double(*p);
    result_ = tmp;
}

void EvalDoubleVisitor::visit(const Pow &x)
{
    double a = eval_double(*(x.base_));
    double b = eval_double(*(x.exp_));
    result_ = ::pow(a, b);
}

void EvalDoubleVisitor::visit(const Sin &x)
{
    double tmp = eval_double(*(x.get_arg()));
    result_ = ::sin(tmp);
}

void EvalDoubleVisitor::visit(const Cos &x)
{
    double tmp = eval_double(*(x.get_arg()));
    result_ = ::cos(tmp);
}

void EvalDoubleVisitor::visit(const Tan &x)
{
    double tmp = eval_double(*(x.get_arg()));
    result_ = ::tan(tmp);
}

double eval_double(const Basic &b)
{
    EvalDoubleVisitor v;
    return v.apply(b);
}

} // CSymPy
