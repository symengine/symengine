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
#include "eval_arb.h"

#ifdef HAVE_CSYMPY_ARB
#include "arb.h"

namespace CSymPy {

class EvalArbVisitor : public Visitor {
private:
    long prec_;
    arb_t result_;
public:
    void apply(arb_t &result, const Basic &b, long precision) {
        arb_init(result_);
        prec_ = precision;
        b.accept(*this);
        arb_set(result, result_);
    }

    virtual void visit(const Integer &x) {
        fmpz_t z_;
        fmpz_init(z_);
        fmpz_set_mpz(z_, x.i.get_mpz_t());
        arb_set_fmpz(result_, z_);
        fmpz_clear(z_);
    }

    virtual void visit(const Rational &x) {
        fmpq_t q_;
        fmpq_init(q_);
        fmpq_set_mpq(q_, x.i.get_mpq_t());
        arb_set_fmpq(result_, q_, prec_);
        fmpq_clear(q_);
    }

    virtual void visit(const Add &x) {
        arb_t t1, t2;
        arb_init(t1);
        arb_init(t2);
        for (auto &p: x.get_args()) {
            apply(t1, *p, prec_);
            arb_add(t2, t2, t1, prec_);
        }
        arb_set(result_, t2);
    }

    virtual void visit(const Mul &x) {
        arb_t t1, t2;
        arb_init(t1);
        arb_one(t2);
        for (auto &p: x.get_args()) {
            apply(t1, *p, prec_);
            arb_mul(t2, t2, t1, prec_);
        }
        arb_set(result_, t2);
    }

    virtual void visit(const Pow &x) {
        arb_t a, b;
        arb_init(a);
        arb_init(b);
        apply(a, *(x.base_), prec_);
        apply(b, *(x.exp_), prec_);
        arb_pow(result_, a, b, prec_);
    }

    virtual void visit(const Sin &x) {
        arb_t tmp;
        arb_init(tmp);
        apply(tmp, *(x.get_arg()), prec_);
        arb_sin(result_, tmp, prec_);
    }

    virtual void visit(const Cos &x) {
        arb_t tmp;
        arb_init(tmp);
        apply(tmp, *(x.get_arg()), prec_);
        arb_cos(result_, tmp, prec_);
    }

    virtual void visit(const Tan &x) {
        arb_t tmp;
        arb_init(tmp);
        apply(tmp, *(x.get_arg()), prec_);
        arb_tan(result_, tmp, prec_);
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

void eval_arb(arb_t &result, const Basic &b, long precision)
{
    EvalArbVisitor v;
    v.apply(result, b, precision);
}

} // CSymPy

#endif // HAVE_CSYMPY_ARB
