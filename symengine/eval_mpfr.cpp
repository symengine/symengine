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
#include "eval_mpfr.h"

#ifdef HAVE_SYMENGINE_MPFR

namespace SymEngine {

class EvalMPFRVisitor : public BaseVisitor<EvalMPFRVisitor> {
private:
    mpfr_rnd_t rnd_;
    mpfr_ptr result_;
public:
    EvalMPFRVisitor(mpfr_rnd_t rnd) : BaseVisitor(this), rnd_{rnd} { }

    void apply(mpfr_ptr result, const Basic &b) {
        mpfr_ptr tmp = result_;
        result_ = result;
        b.accept(*this);
        result_ = tmp;
    }

    void bvisit(const Integer &x) {
        mpfr_set_z(result_, x.i.get_mpz_t(), rnd_);
    }

    void bvisit(const Rational &x) {
        mpfr_set_q(result_, x.i.get_mpq_t(), rnd_);
    }

    void bvisit(const RealDouble &x) {
        mpfr_set_d(result_, x.i, rnd_);
    }

    void bvisit(const Add &x) {
        mpfr_t t;
        mpfr_init2(t, mpfr_get_prec(result_));

        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;
        for (; p != d.end();  p++) {
            apply(t, *(*p));
            mpfr_add(result_, result_, t, rnd_);
        }
        mpfr_clear(t);
    }

    void bvisit(const Mul &x) {
        mpfr_t t;
        mpfr_init2(t, mpfr_get_prec(result_));

        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;
        for (; p != d.end();  p++) {
            apply(t, *(*p));
            mpfr_mul(result_, result_, t, rnd_);
        }
        mpfr_clear(t);
    }

    void bvisit(const Pow &x) {
        if (eq(x.get_base(), E)) {
            apply(result_, *(x.exp_));
            mpfr_exp(result_, result_, rnd_);
        } else {
            mpfr_t b;
            mpfr_init2(b, mpfr_get_prec(result_));

            apply(b, *(x.base_));
            apply(result_, *(x.exp_));
            mpfr_pow(result_, b, result_, rnd_);

            mpfr_clear(b);
        }
    }

    void bvisit(const Sin &x) {
        apply(result_, *(x.get_arg()));
        mpfr_sin(result_, result_, rnd_);
    }

    void bvisit(const Cos &x) {
        apply(result_, *(x.get_arg()));
        mpfr_cos(result_, result_, rnd_);
    }

    void bvisit(const Tan &x) {
        apply(result_, *(x.get_arg()));
        mpfr_tan(result_, result_, rnd_);
    }

    void bvisit(const Log &x) {
        apply(result_, *(x.get_arg()));
        mpfr_log(result_, result_, rnd_);
    }

    void bvisit(const Cot &x) {
        apply(result_, *(x.get_arg()));
        mpfr_cot(result_, result_, rnd_);
    }

    void bvisit(const Csc &x) {
        apply(result_, *(x.get_arg()));
        mpfr_sin(result_, result_, rnd_);
        mpfr_ui_div(result_, 1, result_, rnd_);
    }

    void bvisit(const Sec &x) {
        apply(result_, *(x.get_arg()));
        mpfr_cos(result_, result_, rnd_);
        mpfr_ui_div(result_, 1, result_, rnd_);
    }

    void bvisit(const ASin &x) {
        apply(result_, *(x.get_arg()));
        mpfr_asin(result_, result_, rnd_);
    }

    void bvisit(const ACos &x) {
        apply(result_, *(x.get_arg()));
        mpfr_acos(result_, result_, rnd_);
    }

    void bvisit(const ASec &x) {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_asin(result_, result_, rnd_);
    }

    void bvisit(const ACsc &x) {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_acos(result_, result_, rnd_);
    }

    void bvisit(const ATan &x) {
        apply(result_, *(x.get_arg()));
        mpfr_atan(result_, result_, rnd_);
    }

    void bvisit(const ACot &x) {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_atan(result_, result_, rnd_);
    }

    void bvisit(const ATan2 &x) {
        mpfr_t t;
        mpfr_init(t);

        apply(t, *(x.get_num()));
        apply(result_, *(x.get_den()));
        mpfr_atan2(result_, t, result_, rnd_);

        mpfr_clear(t);
    }

    void bvisit(const Sinh &x) {
        apply(result_, *(x.get_arg()));
        mpfr_sinh(result_, result_, rnd_);
    }

    void bvisit(const Cosh &x) {
        apply(result_, *(x.get_arg()));
        mpfr_cosh(result_, result_, rnd_);
    }

    void bvisit(const Tanh &x) {
        apply(result_, *(x.get_arg()));
        mpfr_tanh(result_, result_, rnd_);
    }

    void bvisit(const Coth &x) {
        apply(result_, *(x.get_arg()));
        mpfr_coth(result_, result_, rnd_);
    }

    void bvisit(const ASinh &x) {
        apply(result_, *(x.get_arg()));
        mpfr_asinh(result_, result_, rnd_);
    }

    void bvisit(const ACosh &x) {
        apply(result_, *(x.get_arg()));
        mpfr_acosh(result_, result_, rnd_);
    }

    void bvisit(const ATanh &x) {
        apply(result_, *(x.get_arg()));
        mpfr_atanh(result_, result_, rnd_);
    }

    void bvisit(const ACoth &x) {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_atanh(result_, result_, rnd_);
    }

    void bvisit(const ASech &x) {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_acosh(result_, result_, rnd_);
    };

    void bvisit(const Gamma &x) {
        apply(result_, *(x.get_args()[0]));
        mpfr_gamma(result_, result_, rnd_);
    };

    void bvisit(const Constant &x) {
        if (x.__eq__(*pi)) {
            mpfr_const_pi(result_, rnd_);
        } else if (x.__eq__(*E)) {
            mpfr_const_euler(result_, rnd_);
        } else {
            throw std::runtime_error("Constant " + x.get_name() + " is not implemented.");
        }
    }

    void bvisit(const Abs &x) {
        apply(result_, *(x.get_arg()));
        mpfr_abs(result_, result_, rnd_);
    };

    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, FunctionSymbol, LambertW
    // Derivative, Complex, ComplexDouble
    void bvisit(const Basic &) {
        throw std::runtime_error("Not implemented.");
    };

};

void eval_mpfr(mpfr_t result, const Basic &b, mpfr_rnd_t rnd)
{
    EvalMPFRVisitor v(rnd);
    v.apply(result, b);
}

} // SymEngine

#endif // HAVE_SYMENGINE_MPFR
