#include "basic.h"
#include "symbol.h"
#include "polynomial.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "functions.h"
#include "constants.h"
#include "visitor.h"
#include "eval_arb.h"

#ifdef HAVE_SYMENGINE_ARB

namespace SymEngine {

class EvalArbVisitor : public Visitor {
private:
    long prec_;
    arb_ptr result_;
public:
    EvalArbVisitor(long precision) : prec_{precision} { }

    void apply(arb_ptr result, const Basic &b) {
        arb_ptr tmp = result_;
        result_ = result;
        b.accept(*this);
        result_ = tmp;
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

    virtual void visit(const RealDouble &x) {
        arf_t f_;
        arf_init(f_);
        arf_set_d(f_, x.i);
        arb_set_arf(result_, f_);
        arf_clear(f_);
    }

    virtual void visit(const Add &x) {
        arb_t t;
        arb_init(t);

        auto d = x.get_args();
        for (auto p = d.begin(); p != d.end();  p++) {

            if (p == d.begin()) {
                apply(result_, *(*p));
            } else {
                apply(t, *(*p));
                arb_add(result_, result_, t, prec_);
            }
        }

        arb_clear(t);
    }

    virtual void visit(const Mul &x) {
        arb_t t;
        arb_init(t);

        auto d = x.get_args();
        for (auto p = d.begin(); p != d.end(); p++) {

            if (p == d.begin()) {
                apply(result_, *(*p));
            } else {
                apply(t, *(*p));
                arb_mul(result_, result_, t, prec_);
            }
        }

        arb_clear(t);
    }

    virtual void visit(const Pow &x) {
        if (eq(x.get_base(), E)) {
            apply(result_, *(x.exp_));
            arb_exp(result_, result_, prec_);
        } else {
            arb_t b;
            arb_init(b);

            apply(b, *(x.base_));
            apply(result_, *(x.exp_));
            arb_pow(result_, b, result_, prec_);

            arb_clear(b);
        }
    }

    virtual void visit(const Sin &x) {
        apply(result_, *(x.get_arg()));
        arb_sin(result_, result_, prec_);
    }

    virtual void visit(const Cos &x) {
        apply(result_, *(x.get_arg()));
        arb_cos(result_, result_, prec_);
    }

    virtual void visit(const Tan &x) {
        apply(result_, *(x.get_arg()));
        arb_tan(result_, result_, prec_);
    }

    virtual void visit(const Symbol &x) {
        throw std::runtime_error("Symbol cannot be evaluated as an arb type.");
    }

    virtual void visit(const Polynomial &x) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Complex &) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Log &x) {
        apply(result_, *(x.get_arg()));
        arb_log(result_, result_, prec_);
    }

    virtual void visit(const Derivative &) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Cot &x) {
        apply(result_, *(x.get_arg()));
        arb_cot(result_, result_, prec_);
    }

    virtual void visit(const Csc &x) {
        apply(result_, *(x.get_arg()));
        arb_sin(result_, result_, prec_);
        arb_inv(result_, result_, prec_);
    }

    virtual void visit(const Sec &x) {
        apply(result_, *(x.get_arg()));
        arb_cos(result_, result_, prec_);
        arb_inv(result_, result_, prec_);
    }

    virtual void visit(const ASin &x) {
        apply(result_, *(x.get_arg()));
        arb_asin(result_, result_, prec_);
    }

    virtual void visit(const ACos &x) {
        apply(result_, *(x.get_arg()));
        arb_acos(result_, result_, prec_);
    }

    virtual void visit(const ASec &x) {
        apply(result_, *(x.get_arg()));
        arb_inv(result_, result_, prec_);
        arb_acos(result_, result_, prec_);
    }

    virtual void visit(const ACsc &x) {
        apply(result_, *(x.get_arg()));
        arb_inv(result_, result_, prec_);
        arb_asin(result_, result_, prec_);
    }

    virtual void visit(const ATan &x) {
        apply(result_, *(x.get_arg()));
        arb_atan(result_, result_, prec_);
    }

    virtual void visit(const ACot &x) {
        apply(result_, *(x.get_arg()));
        arb_inv(result_, result_, prec_);
        arb_atan(result_, result_, prec_);
    }

    virtual void visit(const ATan2 &x) {
        arb_t t;
        arb_init(t);

        apply(t, *(x.get_num()));
        apply(result_, *(x.get_den()));
        arb_atan2(result_, t, result_, prec_);

        arb_clear(t);
    }

    virtual void visit(const LambertW &) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const FunctionSymbol &) {
        throw std::runtime_error("Not implemented.");
    }

    virtual void visit(const Sinh &x) {
        apply(result_, *(x.get_arg()));
        arb_sinh(result_, result_, prec_);
    }

    virtual void visit(const Cosh &x) {
        apply(result_, *(x.get_arg()));
        arb_cosh(result_, result_, prec_);
    }

    virtual void visit(const Tanh &x) {
        apply(result_, *(x.get_arg()));
        arb_tanh(result_, result_, prec_);
    }

    virtual void visit(const Coth &x) {
        apply(result_, *(x.get_arg()));
        arb_coth(result_, result_, prec_);
    }

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
    virtual void visit(const ASech &) {
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

    virtual void visit(const Constant &x) {
        if (x.__eq__(*pi)) {
            arb_const_pi(result_, prec_);
        } else if (x.__eq__(*E)) {
            arb_const_e(result_, prec_);
        } else {
            throw std::runtime_error("Constant " + x.get_name() + " is not implemented.");
        }
    }

    virtual void visit(const Abs &) {
        throw std::runtime_error("Not implemented.");
    };

    virtual void visit(const Subs &) {
        throw std::runtime_error("Not implemented.");
    };
};

void eval_arb(arb_t result, const Basic &b, long precision)
{
    EvalArbVisitor v(precision);
    v.apply(result, b);
}

} // SymEngine

#endif // HAVE_SYMENGINE_ARB
