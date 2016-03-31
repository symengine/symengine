#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/polynomial.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/eval_arb.h>
#include <symengine/real_mpfr.h>

#ifdef HAVE_SYMENGINE_ARB

namespace SymEngine
{

class EvalArbVisitor : public BaseVisitor<EvalArbVisitor>
{
protected:
    long prec_;
    arb_ptr result_;

public:
    EvalArbVisitor(long precision) : prec_{precision}
    {
    }

    void apply(arb_ptr result, const Basic &b)
    {
        arb_ptr tmp = result_;
        result_ = result;
        b.accept(*this);
        result_ = tmp;
    }

    void bvisit(const Integer &x)
    {
        fmpz_t z_;
        fmpz_init(z_);
        fmpz_set_mpz(z_, get_mpz_t(x.i));
        arb_set_fmpz(result_, z_);
        fmpz_clear(z_);
    }

    void bvisit(const Rational &x)
    {
        fmpq_t q_;
        fmpq_init(q_);
        fmpq_set_mpq(q_, get_mpq_t(x.i));
        arb_set_fmpq(result_, q_, prec_);
        fmpq_clear(q_);
    }

    void bvisit(const RealDouble &x)
    {
        arf_t f_;
        arf_init(f_);
        arf_set_d(f_, x.i);
        arb_set_arf(result_, f_);
        arf_clear(f_);
    }

    void bvisit(const Add &x)
    {
        arb_t t;
        arb_init(t);

        auto d = x.get_args();
        for (auto p = d.begin(); p != d.end(); p++) {

            if (p == d.begin()) {
                apply(result_, *(*p));
            } else {
                apply(t, *(*p));
                arb_add(result_, result_, t, prec_);
            }
        }

        arb_clear(t);
    }

    void bvisit(const Mul &x)
    {
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

    void bvisit(const Pow &x)
    {
        if (eq(*x.get_base(), *E)) {
            apply(result_, *(x.get_exp()));
            arb_exp(result_, result_, prec_);
        } else {
            arb_t b;
            arb_init(b);

            apply(b, *(x.get_base()));
            apply(result_, *(x.get_exp()));
            arb_pow(result_, b, result_, prec_);

            arb_clear(b);
        }
    }

    void bvisit(const Sin &x)
    {
        apply(result_, *(x.get_arg()));
        arb_sin(result_, result_, prec_);
    }

    void bvisit(const Cos &x)
    {
        apply(result_, *(x.get_arg()));
        arb_cos(result_, result_, prec_);
    }

    void bvisit(const Tan &x)
    {
        apply(result_, *(x.get_arg()));
        arb_tan(result_, result_, prec_);
    }

    void bvisit(const Symbol &x)
    {
        throw std::runtime_error("Symbol cannot be evaluated as an arb type.");
    }

    void bvisit(const UnivariateIntPolynomial &x)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const Complex &)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const ComplexDouble &x)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const RealMPFR &x)
    {
        throw std::runtime_error("Not implemented.");
    }
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &)
    {
        throw std::runtime_error("Not implemented.");
    };
#endif
    void bvisit(const Log &x)
    {
        apply(result_, *(x.get_arg()));
        arb_log(result_, result_, prec_);
    }

    void bvisit(const Derivative &)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const Cot &x)
    {
        apply(result_, *(x.get_arg()));
        arb_cot(result_, result_, prec_);
    }

    void bvisit(const Csc &x)
    {
        apply(result_, *(x.get_arg()));
        arb_sin(result_, result_, prec_);
        arb_inv(result_, result_, prec_);
    }

    void bvisit(const Sec &x)
    {
        apply(result_, *(x.get_arg()));
        arb_cos(result_, result_, prec_);
        arb_inv(result_, result_, prec_);
    }

    void bvisit(const ASin &x)
    {
        apply(result_, *(x.get_arg()));
        arb_asin(result_, result_, prec_);
    }

    void bvisit(const ACos &x)
    {
        apply(result_, *(x.get_arg()));
        arb_acos(result_, result_, prec_);
    }

    void bvisit(const ASec &x)
    {
        apply(result_, *(x.get_arg()));
        arb_inv(result_, result_, prec_);
        arb_acos(result_, result_, prec_);
    }

    void bvisit(const ACsc &x)
    {
        apply(result_, *(x.get_arg()));
        arb_inv(result_, result_, prec_);
        arb_asin(result_, result_, prec_);
    }

    void bvisit(const ATan &x)
    {
        apply(result_, *(x.get_arg()));
        arb_atan(result_, result_, prec_);
    }

    void bvisit(const ACot &x)
    {
        apply(result_, *(x.get_arg()));
        arb_inv(result_, result_, prec_);
        arb_atan(result_, result_, prec_);
    }

    void bvisit(const ATan2 &x)
    {
        arb_t t;
        arb_init(t);

        apply(t, *(x.get_num()));
        apply(result_, *(x.get_den()));
        arb_atan2(result_, t, result_, prec_);

        arb_clear(t);
    }

    void bvisit(const LambertW &)
    {
        throw std::runtime_error("Not implemented.");
    }

    void bvisit(const FunctionWrapper &x)
    {
        x.eval(prec_)->accept(*this);
    }

    void bvisit(const Sinh &x)
    {
        apply(result_, *(x.get_arg()));
        arb_sinh(result_, result_, prec_);
    }

    void bvisit(const Csch &)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const Cosh &x)
    {
        apply(result_, *(x.get_arg()));
        arb_cosh(result_, result_, prec_);
    }

    void bvisit(const Sech &)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const Tanh &x)
    {
        apply(result_, *(x.get_arg()));
        arb_tanh(result_, result_, prec_);
    }

    void bvisit(const Coth &x)
    {
        apply(result_, *(x.get_arg()));
        arb_coth(result_, result_, prec_);
    }

    void bvisit(const Max &x)
    {
        arb_t t;
        arb_init(t);

        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;

        for (; p != d.end(); p++) {

            apply(t, *(*p));
            if (arb_gt(t, result_))
                arb_set(result_, t);
        }

        arb_clear(t);
    }

    void bvisit(const Min &x)
    {
        arb_t t;
        arb_init(t);

        auto d = x.get_args();
        auto p = d.begin();
        apply(result_, *(*p));
        p++;

        for (; p != d.end(); p++) {

            apply(t, *(*p));
            if (arb_lt(t, result_))
                arb_set(result_, t);
        }

        arb_clear(t);
    }

    void bvisit(const ASinh &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const ACsch &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const ACosh &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const ATanh &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const ACoth &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const ASech &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const KroneckerDelta &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const LeviCivita &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const Zeta &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const Dirichlet_eta &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const Gamma &x)
    {
        apply(result_, *(x.get_args())[0]);
        arb_gamma(result_, result_, prec_);
    };
    void bvisit(const LogGamma &x)
    {
        apply(result_, *(x.get_args())[0]);
        arb_lgamma(result_, result_, prec_);
    }
    void bvisit(const LowerGamma &)
    {
        throw std::runtime_error("Not implemented.");
    };
    void bvisit(const UpperGamma &)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const Constant &x)
    {
        if (x.__eq__(*pi)) {
            arb_const_pi(result_, prec_);
        } else if (x.__eq__(*E)) {
            arb_const_e(result_, prec_);
        } else if (x.__eq__(*EulerGamma)) {
            arb_const_euler(result_, prec_);
        } else {
            throw std::runtime_error("Constant " + x.get_name()
                                     + " is not implemented.");
        }
    }

    void bvisit(const Abs &)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const Basic &)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const NumberWrapper &x)
    {
        x.eval(prec_)->accept(*this);
    }
};

void eval_arb(arb_t result, const Basic &b, long precision)
{
    EvalArbVisitor v(precision);
    v.apply(result, b);
}

} // SymEngine

#endif // HAVE_SYMENGINE_ARB
