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
#include "eval_double.h"

namespace CSymPy {

class EvalDoubleVisitor : public Visitor {
private:
    /*
       The 'result_' variable is assigned into at the very end of each visit()
       methods below. The only place where these methods are called from is the
       line 'b.accept(*this)' in apply() and the 'result_' is immediately
       returned. Thus no corruption can happen and apply() can be safely called
       recursively.
    */
    double result_;
public:
    double apply(const Basic &b) {
        b.accept(*this);
        return result_;
    }

    void visit(const Integer &x) {
        double tmp = x.i.get_d();
        result_ = tmp;
    }

    void visit(const Rational &x) {
        double tmp = x.i.get_d();
        result_ = tmp;
    }

    void visit(const Add &x) {
        double tmp = 0;
        for (auto &p: x.get_args()) tmp = tmp + apply(*p);
        result_ = tmp;
    }

    void visit(const Mul &x) {
        double tmp = 1;
        for (auto &p: x.get_args()) tmp = tmp * apply(*p);
        result_ = tmp;
    }

    void visit(const Pow &x) {
        double a = apply(*(x.base_));
        double b = apply(*(x.exp_));
        result_ = ::pow(a, b);
    }

    void visit(const Sin &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::sin(tmp);
    }

    void visit(const Cos &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::cos(tmp);
    }

    void visit(const Tan &x) {
        double tmp = apply(*(x.get_arg()));
        result_ = ::tan(tmp);
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
    virtual void visit(const Constant &) {
        throw std::runtime_error("Not implemented.");
    };
    virtual void visit(const Abs &) {
        throw std::runtime_error("Not implemented.");
    };
};

double eval_double(const Basic &b)
{
    EvalDoubleVisitor v;
    return v.apply(b);
}

} // CSymPy
