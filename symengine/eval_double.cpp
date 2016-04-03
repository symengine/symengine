#include <cmath>
#include <functional>
#include <complex>
#include <algorithm>

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
#include <symengine/eval_double.h>

namespace SymEngine
{

template <typename T, typename C>
class EvalDoubleVisitor : public BaseVisitor<C>
{
protected:
    /*
       The 'result_' variable is assigned into at the very end of each visit()
       methods below. The only place where these methods are called from is the
       line 'b.accept(*this)' in apply() and the 'result_' is immediately
       returned. Thus no corruption can happen and apply() can be safely called
       recursively.
    */
    T result_;

public:
    T apply(const Basic &b)
    {
        b.accept(*static_cast<C *>(this));
        return result_;
    }

    void bvisit(const Integer &x)
    {
        T tmp = mp_get_d(x.i);
        result_ = tmp;
    }

    void bvisit(const Rational &x)
    {
        T tmp = mp_get_d(x.i);
        result_ = tmp;
    }

    void bvisit(const RealDouble &x)
    {
        T tmp = x.i;
        result_ = tmp;
    }
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x)
    {
        T tmp = mpfr_get_d(x.i.get_mpfr_t(), MPFR_RNDN);
        result_ = tmp;
    }
#endif
    void bvisit(const Add &x)
    {
        T tmp = 0;
        for (const auto &p : x.get_args())
            tmp += apply(*p);
        result_ = tmp;
    }

    void bvisit(const Mul &x)
    {
        T tmp = 1;
        for (const auto &p : x.get_args())
            tmp *= apply(*p);
        result_ = tmp;
    }

    void bvisit(const Pow &x)
    {
        T exp_ = apply(*(x.get_exp()));
        if (eq(*(x.get_base()), *E)) {
            result_ = std::exp(exp_);
        } else {
            T base_ = apply(*(x.get_base()));
            result_ = std::pow(base_, exp_);
        }
    }

    void bvisit(const Sin &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::sin(tmp);
    }

    void bvisit(const Cos &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::cos(tmp);
    }

    void bvisit(const Tan &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::tan(tmp);
    }

    void bvisit(const Symbol &)
    {
        throw std::runtime_error("Symbol cannot be evaluated.");
    };

    void bvisit(const Log &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::log(tmp);
    };

    void bvisit(const Cot &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = 1.0 / std::tan(tmp);
    };

    void bvisit(const Csc &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = 1.0 / std::sin(tmp);
    };

    void bvisit(const Sec &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = 1.0 / std::cos(tmp);
    };

    void bvisit(const ASin &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::asin(tmp);
    };

    void bvisit(const ACos &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::acos(tmp);
    };

    void bvisit(const ASec &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::acos(1.0 / tmp);
    };

    void bvisit(const ACsc &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::asin(1.0 / tmp);
    };

    void bvisit(const ATan &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::atan(tmp);
    };

    void bvisit(const ACot &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::atan(1.0 / tmp);
    };

    void bvisit(const Sinh &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::sinh(tmp);
    };

    void bvisit(const Csch &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = 1.0 / std::sinh(tmp);
    };

    void bvisit(const Cosh &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::cosh(tmp);
    };

    void bvisit(const Sech &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = 1.0 / std::cosh(tmp);
    };

    void bvisit(const Tanh &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::tanh(tmp);
    };

    void bvisit(const Coth &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = 1.0 / std::tanh(tmp);
    };

    void bvisit(const ASinh &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::asinh(tmp);
    };

    void bvisit(const ACsch &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::asinh(1.0 / tmp);
    };

    void bvisit(const ACosh &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::acosh(tmp);
    };

    void bvisit(const ATanh &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::atanh(tmp);
    };

    void bvisit(const ACoth &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::atanh(1.0 / tmp);
    };

    void bvisit(const ASech &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::acosh(1.0 / tmp);
    };

    void bvisit(const Constant &x)
    {
        if (eq(x, *pi)) {
            result_ = std::atan2(0, -1);
        } else if (eq(x, *E)) {
            result_ = std::exp(1);
        } else if (eq(x, *EulerGamma)) {
            result_ = 0.5772156649015328606065; // use until polygamma or
                                                // digamma is implemented
        } else {
            throw std::runtime_error("Constant " + x.get_name()
                                     + " is not implemented.");
        }
    };

    void bvisit(const Abs &x)
    {
        T tmp = apply(*(x.get_arg()));
        result_ = std::abs(tmp);
    };

    void bvisit(const Basic &)
    {
        throw std::runtime_error("Not implemented.");
    };

    void bvisit(const NumberWrapper &x)
    {
        apply(*(x.eval(53)));
    }

    void bvisit(const FunctionWrapper &x)
    {
        apply(*(x.eval(53)));
    }
};

template <typename C>
class EvalRealDoubleVisitor : public EvalDoubleVisitor<double, C>
{
public:
    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, LambertW
    // Derivative, Complex, ComplexDouble, ComplexMPC

    using EvalDoubleVisitor<double, C>::bvisit;
    using EvalDoubleVisitor<double, C>::apply;
    using EvalDoubleVisitor<double, C>::result_;

    void bvisit(const ATan2 &x)
    {
        double num = apply(*(x.get_num()));
        double den = apply(*(x.get_den()));
        result_ = std::atan2(num, den);
    };

    void bvisit(const Gamma &x)
    {
        double tmp = apply(*(x.get_args()[0]));
        result_ = std::tgamma(tmp);
    };

    void bvisit(const LogGamma &x)
    {
        double tmp = apply(*(x.get_args()[0]));
        result_ = std::lgamma(tmp);
    }

    void bvisit(const Erf &x)
    {
        double tmp = apply(*(x.get_args()[0]));
        result_ = std::erf(tmp);
    }

    void bvisit(const Max &x)
    {
        auto d = x.get_args();
        auto p = d.begin();
        double result = apply(*(*p));
        p++;

        for (; p != d.end(); p++) {
            double tmp = apply(*(*p));
            result = std::max(result, tmp);
        }
        result_ = result;
    };

    void bvisit(const Min &x)
    {
        auto d = x.get_args();
        auto p = d.begin();
        double result = apply(*(*p));
        p++;

        for (; p != d.end(); p++) {
            double tmp = apply(*(*p));
            result = std::min(result, tmp);
        }
        result_ = result;
    };
};

class EvalRealDoubleVisitorPattern
    : public EvalRealDoubleVisitor<EvalRealDoubleVisitorPattern>
{
};

class EvalRealDoubleVisitorFinal
    : public EvalRealDoubleVisitor<EvalRealDoubleVisitorFinal>
{
};

class EvalComplexDoubleVisitor
    : public EvalDoubleVisitor<std::complex<double>, EvalComplexDoubleVisitor>
{
public:
    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, LambertW
    // Derivative, ATan2, Gamma

    using EvalDoubleVisitor::bvisit;

    void bvisit(const Complex &x)
    {
        result_
            = std::complex<double>(mp_get_d(x.real_), mp_get_d(x.imaginary_));
    };

    void bvisit(const ComplexDouble &x)
    {
        result_ = x.i;
    };
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &x)
    {
        mpfr_class t(x.get_prec());
        double real, imag;
        mpc_real(t.get_mpfr_t(), x.i.get_mpc_t(), MPFR_RNDN);
        real = mpfr_get_d(t.get_mpfr_t(), MPFR_RNDN);
        mpc_imag(t.get_mpfr_t(), x.i.get_mpc_t(), MPFR_RNDN);
        imag = mpfr_get_d(t.get_mpfr_t(), MPFR_RNDN);
        result_ = std::complex<double>(real, imag);
    }
#endif
};

/*
 * These two seem to be equivalent and about the same fast.
*/
// typedef double (*fn)(const Basic &);
typedef std::function<double(const Basic &)> fn;

std::vector<fn> init_eval_double()
{
    std::vector<fn> table;
    table.assign(TypeID_Count, [](const Basic &x) -> double {
        throw std::runtime_error("Not implemented.");
    });
    table[INTEGER] = [](const Basic &x) {
        double tmp = mp_get_d((static_cast<const Integer &>(x)).i);
        return tmp;
    };
    table[RATIONAL] = [](const Basic &x) {
        double tmp = mp_get_d((static_cast<const Rational &>(x)).i);
        return tmp;
    };
    table[REAL_DOUBLE] = [](const Basic &x) {
        double tmp = (static_cast<const RealDouble &>(x)).i;
        return tmp;
    };
#ifdef HAVE_SYMENGINE_MPFR
    table[REAL_MPFR] = [](const Basic &x) {
        double tmp = mpfr_get_d(static_cast<const RealMPFR &>(x).i.get_mpfr_t(),
                                MPFR_RNDN);
        return tmp;
    };
#endif
    table[ADD] = [](const Basic &x) {
        double tmp = 0;
        for (const auto &p : x.get_args())
            tmp += eval_double_single_dispatch(*p);
        return tmp;
    };
    table[MUL] = [](const Basic &x) {
        double tmp = 1;
        for (const auto &p : x.get_args())
            tmp *= eval_double_single_dispatch(*p);
        return tmp;
    };
    table[POW] = [](const Basic &x) {
        double a = eval_double_single_dispatch(
            *(static_cast<const Pow &>(x)).get_base());
        double b = eval_double_single_dispatch(
            *(static_cast<const Pow &>(x)).get_exp());
        return ::pow(a, b);
    };
    table[SIN] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Sin &>(x)).get_arg());
        return ::sin(tmp);
    };
    table[COS] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Cos &>(x)).get_arg());
        return ::cos(tmp);
    };
    table[TAN] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Tan &>(x)).get_arg());
        return ::tan(tmp);
    };
    table[LOG] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Log &>(x)).get_arg());
        return ::log(tmp);
    };
    table[COT] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Cot &>(x)).get_arg());
        return 1 / ::tan(tmp);
    };
    table[CSC] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Csc &>(x)).get_arg());
        return 1 / ::sin(tmp);
    };
    table[SEC] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Sec &>(x)).get_arg());
        return 1 / ::cos(tmp);
    };
    table[ASIN] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ASin &>(x)).get_arg());
        return ::asin(tmp);
    };
    table[ACOS] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ACos &>(x)).get_arg());
        return ::acos(tmp);
    };
    table[ASEC] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ASec &>(x)).get_arg());
        return ::acos(1 / tmp);
    };
    table[ACSC] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ACsc &>(x)).get_arg());
        return ::asin(1 / tmp);
    };
    table[ATAN] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ATan &>(x)).get_arg());
        return ::atan(tmp);
    };
    table[ACOT] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ACot &>(x)).get_arg());
        return ::atan(1 / tmp);
    };
    table[ATAN2] = [](const Basic &x) {
        double num = eval_double_single_dispatch(
            *(static_cast<const ATan2 &>(x)).get_num());
        double den = eval_double_single_dispatch(
            *(static_cast<const ATan2 &>(x)).get_den());
        return ::atan2(num, den);
    };
    table[SINH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Sinh &>(x)).get_arg());
        return ::sinh(tmp);
    };
    table[CSCH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Csch &>(x)).get_arg());
        return 1 / ::sinh(tmp);
    };
    table[COSH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Cosh &>(x)).get_arg());
        return ::cosh(tmp);
    };
    table[SECH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Sech &>(x)).get_arg());
        return 1 / ::cosh(tmp);
    };
    table[TANH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Tanh &>(x)).get_arg());
        return ::tanh(tmp);
    };
    table[COTH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Coth &>(x)).get_arg());
        return 1 / ::tanh(tmp);
    };
    table[ASINH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ASinh &>(x)).get_arg());
        return ::asinh(tmp);
    };
    table[ACSCH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ACsch &>(x)).get_arg());
        return ::asinh(1 / tmp);
    };
    table[ACOSH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ACosh &>(x)).get_arg());
        return ::acosh(tmp);
    };
    table[ATANH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ATanh &>(x)).get_arg());
        return ::atanh(tmp);
    };
    table[ACOTH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ACoth &>(x)).get_arg());
        return std::atanh(1 / tmp);
    };
    table[ASECH] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const ASech &>(x)).get_arg());
        return ::acosh(1 / tmp);
    };
    table[GAMMA] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Gamma &>(x)).get_args()[0]);
        return ::tgamma(tmp);
    };
    table[LOGGAMMA] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const LogGamma &>(x)).get_args()[0]);
        return ::lgamma(tmp);
    };
    table[ERF] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Erf &>(x)).get_args()[0]);
        return ::erf(tmp);
    };
    table[CONSTANT] = [](const Basic &x) {
        if (eq(x, *pi)) {
            return ::atan2(0, -1);
        } else if (eq(x, *E)) {
            return ::exp(1);
        } else if (eq(x, *EulerGamma)) {
            return 0.5772156649015328606065; // use until polygamma or digamma
                                             // is implemented
        } else {
            throw std::runtime_error(
                "Constant " + static_cast<const Constant &>(x).get_name()
                + " is not implemented.");
        }
    };
    table[ABS] = [](const Basic &x) {
        double tmp = eval_double_single_dispatch(
            *(static_cast<const Abs &>(x)).get_arg());
        return std::abs(tmp);
    };
    table[MAX] = [](const Basic &x) {
        double result;
        result = eval_double_single_dispatch(
            *(static_cast<const Max &>(x).get_args()[0]));
        for (const auto &p : static_cast<const Max &>(x).get_args()) {
            double tmp = eval_double_single_dispatch(*p);
            result = std::max(result, tmp);
        }
        return result;
    };
    table[MIN] = [](const Basic &x) {
        double result;
        result = eval_double_single_dispatch(
            *(static_cast<const Max &>(x).get_args()[0]));
        for (const auto &p : static_cast<const Min &>(x).get_args()) {
            double tmp = eval_double_single_dispatch(*p);
            result = std::min(result, tmp);
        }
        return result;
    };
    return table;
}

const static std::vector<fn> table_eval_double = init_eval_double();

double eval_double(const Basic &b)
{
    EvalRealDoubleVisitorFinal v;
    return v.apply(b);
}

std::complex<double> eval_complex_double(const Basic &b)
{
    EvalComplexDoubleVisitor v;
    return v.apply(b);
}

double eval_double_single_dispatch(const Basic &b)
{
    return table_eval_double[b.get_type_code()](b);
}

double eval_double_visitor_pattern(const Basic &b)
{
    EvalRealDoubleVisitorPattern v;
    return v.apply(b);
}

#define ACCEPT(CLASS)                                                          \
    void CLASS::accept(EvalRealDoubleVisitorFinal &v) const                    \
    {                                                                          \
        v.bvisit(*this);                                                       \
    }

#define SYMENGINE_ENUM(TypeID, Class) ACCEPT(Class)
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM

} // SymEngine
