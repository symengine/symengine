#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/eval_mpfr.h>

#ifdef HAVE_SYMENGINE_MPFR

namespace SymEngine
{

class EvalMPFRVisitor : public BaseVisitor<EvalMPFRVisitor>
{
protected:
    mpfr_rnd_t rnd_;
    mpfr_ptr result_;

public:
    EvalMPFRVisitor(mpfr_rnd_t rnd) : rnd_{rnd}
    {
    }

    void apply(mpfr_ptr result, const Basic &b)
    {
        mpfr_ptr tmp = result_;
        result_ = result;
        b.accept(*this);
        result_ = tmp;
    }

    void bvisit(const Integer &x)
    {
        mpfr_set_z(result_, get_mpz_t(x.i), rnd_);
    }

    void bvisit(const Rational &x)
    {
        mpfr_set_q(result_, get_mpq_t(x.i), rnd_);
    }

    void bvisit(const RealDouble &x)
    {
        mpfr_set_d(result_, x.i, rnd_);
    }

    void bvisit(const RealMPFR &x)
    {
        mpfr_set(result_, x.i.get_mpfr_t(), rnd_);
    }

    void bvisit(const Add &x)
    {
        mpfr_class t(mpfr_get_prec(result_));
        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;
        for (; p != d.end(); p++) {
            apply(t.get_mpfr_t(), *(*p));
            mpfr_add(result_, result_, t.get_mpfr_t(), rnd_);
        }
    }

    void bvisit(const Mul &x)
    {
        mpfr_class t(mpfr_get_prec(result_));
        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;
        for (; p != d.end(); p++) {
            apply(t.get_mpfr_t(), *(*p));
            mpfr_mul(result_, result_, t.get_mpfr_t(), rnd_);
        }
    }

    void bvisit(const Pow &x)
    {
        if (eq(*x.get_base(), *E)) {
            apply(result_, *(x.get_exp()));
            mpfr_exp(result_, result_, rnd_);
        } else {
            mpfr_class b(mpfr_get_prec(result_));
            apply(b.get_mpfr_t(), *(x.get_base()));
            apply(result_, *(x.get_exp()));
            mpfr_pow(result_, b.get_mpfr_t(), result_, rnd_);
        }
    }

    void bvisit(const Sin &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_sin(result_, result_, rnd_);
    }

    void bvisit(const Cos &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_cos(result_, result_, rnd_);
    }

    void bvisit(const Tan &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_tan(result_, result_, rnd_);
    }

    void bvisit(const Log &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_log(result_, result_, rnd_);
    }

    void bvisit(const Cot &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_cot(result_, result_, rnd_);
    }

    void bvisit(const Csc &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_csc(result_, result_, rnd_);
    }

    void bvisit(const Sec &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_sec(result_, result_, rnd_);
    }

    void bvisit(const ASin &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_asin(result_, result_, rnd_);
    }

    void bvisit(const ACos &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_acos(result_, result_, rnd_);
    }

    void bvisit(const ASec &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_asin(result_, result_, rnd_);
    }

    void bvisit(const ACsc &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_acos(result_, result_, rnd_);
    }

    void bvisit(const ATan &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_atan(result_, result_, rnd_);
    }

    void bvisit(const ACot &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_atan(result_, result_, rnd_);
    }

    void bvisit(const ATan2 &x)
    {
        mpfr_class t(mpfr_get_prec(result_));
        apply(t.get_mpfr_t(), *(x.get_num()));
        apply(result_, *(x.get_den()));
        mpfr_atan2(result_, t.get_mpfr_t(), result_, rnd_);
    }

    void bvisit(const Sinh &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_sinh(result_, result_, rnd_);
    }

    void bvisit(const Csch &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_csch(result_, result_, rnd_);
    }

    void bvisit(const Cosh &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_cosh(result_, result_, rnd_);
    }

    void bvisit(const Sech &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_sech(result_, result_, rnd_);
    }

    void bvisit(const Tanh &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_tanh(result_, result_, rnd_);
    }

    void bvisit(const Coth &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_coth(result_, result_, rnd_);
    }

    void bvisit(const ASinh &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_asinh(result_, result_, rnd_);
    }

    void bvisit(const ACsch &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_asinh(result_, result_, rnd_);
    };

    void bvisit(const ACosh &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_acosh(result_, result_, rnd_);
    }

    void bvisit(const ATanh &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_atanh(result_, result_, rnd_);
    }

    void bvisit(const ACoth &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_atanh(result_, result_, rnd_);
    }

    void bvisit(const ASech &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_ui_div(result_, 1, result_, rnd_);
        mpfr_acosh(result_, result_, rnd_);
    };

    void bvisit(const Gamma &x)
    {
        apply(result_, *(x.get_args()[0]));
        mpfr_gamma(result_, result_, rnd_);
    };

    void bvisit(const LogGamma &x)
    {
        apply(result_, *(x.get_args()[0]));
        mpfr_lngamma(result_, result_, rnd_);
    }

    void bvisit(const Beta &x)
    {
        apply(result_, *(x.rewrite_as_gamma()));
    };

    void bvisit(const Constant &x)
    {
        if (x.__eq__(*pi)) {
            mpfr_const_pi(result_, rnd_);
        } else if (x.__eq__(*E)) {
            mpfr_t one_;
            mpfr_init2(one_, mpfr_get_prec(result_));
            mpfr_set_ui(one_, 1, rnd_);
            mpfr_exp(result_, one_, rnd_);
            mpfr_clear(one_);
        } else if (x.__eq__(*EulerGamma)) {
            mpfr_const_euler(result_, rnd_);
        } else {
            throw std::runtime_error("Constant " + x.get_name()
                                     + " is not implemented.");
        }
    }

    void bvisit(const Abs &x)
    {
        apply(result_, *(x.get_arg()));
        mpfr_abs(result_, result_, rnd_);
    };

    void bvisit(const NumberWrapper &x)
    {
        x.eval(mpfr_get_prec(result_))->accept(*this);
    }

    void bvisit(const FunctionWrapper &x)
    {
        x.eval(mpfr_get_prec(result_))->accept(*this);
    }
    void bvisit(const Erf &x)
    {
        apply(result_, *(x.get_args()[0]));
        mpfr_erf(result_, result_, rnd_);
    }
    void bvisit(const Max &x)
    {
        mpfr_class t(mpfr_get_prec(result_));
        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;
        for (; p != d.end(); p++) {
            apply(t.get_mpfr_t(), *(*p));
            mpfr_max(result_, result_, t.get_mpfr_t(), rnd_);
        }
    }

    void bvisit(const Min &x)
    {
        mpfr_class t(mpfr_get_prec(result_));
        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;
        for (; p != d.end(); p++) {
            apply(t.get_mpfr_t(), *(*p));
            mpfr_min(result_, result_, t.get_mpfr_t(), rnd_);
        }
    }

    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, LambertW
    // Derivative, Complex, ComplexDouble, ComplexMPC
    void bvisit(const Basic &)
    {
        throw std::runtime_error("Not implemented.");
    };
};

void eval_mpfr(mpfr_ptr result, const Basic &b, mpfr_rnd_t rnd)
{
    EvalMPFRVisitor v(rnd);
    v.apply(result, b);
}

} // SymEngine

#endif // HAVE_SYMENGINE_MPFR
