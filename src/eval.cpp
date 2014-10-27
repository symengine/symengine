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
#include "eval.h"

#ifdef HAVE_CSYMPY_ARB
#include "arb.h"

namespace CSymPy {

class EvalArbVisitor : public Visitor {
private:
    arb_t result_;
public:
    void apply(arb_t &result, const Basic &b) {
        b.accept(*this);
        arb_set(result, result_);
    }

    virtual void visit(const Integer &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Rational &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Add &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Mul &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Pow &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Sin &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Cos &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Tan &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Symbol &) {
        throw std::runtime_error("Symbol cannot be evaluated as an arb type.");
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

void eval_arb(arb_t &result, const Basic &b)
{
    EvalArbVisitor v;
    v.apply(result, b);
}

} // CSymPy

#endif // HAVE_CSYMPY_ARB
