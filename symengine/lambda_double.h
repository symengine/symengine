#ifndef SYMENGINE_LAMBDA_DOUBLE_H
#define SYMENGINE_LAMBDA_DOUBLE_H

#include <cmath>
#include <limits>
#include <memory>
#include <symengine/eval_double.h>
#include <symengine/symengine_exception.h>
#include <symengine/visitor.h>

namespace SymEngine
{

template <typename T>
class LambdaDoubleVisitor : public BaseVisitor<LambdaDoubleVisitor<T>>
{
protected:
    /*
       The 'result_' variable is assigned into at the very end of each visit()
       methods below. The only place where these methods are called from is the
       line 'b.accept(*this)' in apply() and the 'result_' is immediately
       returned. Thus no corruption can happen and apply() can be safely called
       recursively.
    */

    typedef std::function<T(const T *x, const std::vector<T> *cse)> fn;
    std::vector<fn> results;
    std::vector<T> cse_intermediate_results;

    std::map<RCP<const Basic>, size_t, RCPBasicKeyLess>
        cse_intermediate_fns_map;
    std::vector<fn> cse_intermediate_fns;
    fn result_;
    vec_basic symbols;

public:
    void init(const vec_basic &x, const Basic &b, bool cse = false)
    {
        vec_basic outputs = {b.rcp_from_this()};
        init(x, outputs, cse);
    }

    void init(const vec_basic &inputs, const vec_basic &outputs,
              bool cse = false)
    {
        results.clear();
        cse_intermediate_fns.clear();
        symbols = inputs;
        if (not cse) {
            for (auto &p : outputs) {
                results.push_back(apply(*p));
            }
        } else {
            vec_basic reduced_exprs;
            vec_pair replacements;
            // cse the outputs
            SymEngine::cse(replacements, reduced_exprs, outputs);
            for (auto &rep : replacements) {
                auto res = apply(*(rep.second));
                // Store the replacement symbol values in a dictionary for
                // faster
                // lookup for initialization
                cse_intermediate_fns_map[rep.first]
                    = cse_intermediate_fns.size();
                // Store it in a vector for faster use in call
                cse_intermediate_fns.push_back(res);
            }
            cse_intermediate_results.resize(cse_intermediate_fns.size());
            // Generate functions for all the reduced exprs and save it
            for (unsigned i = 0; i < outputs.size(); i++) {
                results.push_back(apply(*reduced_exprs[i]));
            }
            // We don't need the cse_intermediate_fns_map anymore
            cse_intermediate_fns_map.clear();
            symbols.clear();
        }
    }

    fn apply(const Basic &b)
    {
        b.accept(*this);
        return result_;
    }

    T call(const std::vector<T> &vec)
    {
        T res;
        call(&res, vec.data());
        return res;
    }

    void call(T *outs, const T *inps)
    {
        if (cse_intermediate_fns.size() > 0) {
            for (unsigned i = 0; i < cse_intermediate_fns.size(); ++i) {
                cse_intermediate_results[i]
                    = cse_intermediate_fns[i](inps, &cse_intermediate_results);
            }
        }
        for (unsigned i = 0; i < results.size(); ++i) {
            outs[i] = results[i](inps, &cse_intermediate_results);
        }
        return;
    }

    void bvisit(const Symbol &x)
    {
        for (unsigned i = 0; i < symbols.size(); ++i) {
            if (eq(x, *symbols[i])) {
                result_ = [=](const T *x, const std::vector<T> *cse) {
                    return x[i];
                };
                return;
            }
        }
        auto it = cse_intermediate_fns_map.find(x.rcp_from_this());
        if (it != cse_intermediate_fns_map.end()) {
            auto index = it->second;
            result_ = [=](const T *x, const std::vector<T> *cse) {
                return (*cse)[index];
            };
            return;
        }
        throw SymEngineException("Symbol not in the symbols vector.");
    };

    void bvisit(const Integer &x)
    {
        T tmp = mp_get_d(x.as_integer_class());
        result_ = [=](const T *x, const std::vector<T> *cse) { return tmp; };
    }

    void bvisit(const Rational &x)
    {
        T tmp = mp_get_d(x.as_rational_class());
        result_ = [=](const T *x, const std::vector<T> *cse) { return tmp; };
    }

    void bvisit(const RealDouble &x)
    {
        T tmp = x.i;
        result_ = [=](const T *x, const std::vector<T> *cse) { return tmp; };
    }

#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x)
    {
        T tmp = mpfr_get_d(x.i.get_mpfr_t(), MPFR_RNDN);
        result_ = [=](const T *x, const std::vector<T> *cse) { return tmp; };
    }
#endif

    void bvisit(const Add &x)
    {
        fn tmp = apply(*x.get_coef());
        fn tmp1, tmp2;
        for (const auto &p : x.get_dict()) {
            tmp1 = apply(*(p.first));
            tmp2 = apply(*(p.second));
            tmp = [=](const T *x, const std::vector<T> *cse) {
                return tmp(x, cse) + tmp1(x, cse) * tmp2(x, cse);
            };
        }
        result_ = tmp;
    }

    void bvisit(const Mul &x)
    {
        fn tmp = apply(*x.get_coef());
        fn tmp1, tmp2;
        for (const auto &p : x.get_dict()) {
            tmp1 = apply(*(p.first));
            tmp2 = apply(*(p.second));
            tmp = [=](const T *x, const std::vector<T> *cse) {
                return tmp(x, cse) * std::pow(tmp1(x, cse), tmp2(x, cse));
            };
        }
        result_ = tmp;
    }

    void bvisit(const Pow &x)
    {
        fn exp_ = apply(*(x.get_exp()));
        if (eq(*(x.get_base()), *E)) {
            result_ = [=](const T *x, const std::vector<T> *cse) {
                return std::exp(exp_(x, cse));
            };
        } else {
            fn base_ = apply(*(x.get_base()));
            result_ = [=](const T *x, const std::vector<T> *cse) {
                return std::pow(base_(x, cse), exp_(x, cse));
            };
        }
    }

    void bvisit(const Sin &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::sin(tmp(x, cse));
        };
    }

    void bvisit(const Cos &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::cos(tmp(x, cse));
        };
    }

    void bvisit(const Tan &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::tan(tmp(x, cse));
        };
    }

    void bvisit(const Log &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::log(tmp(x, cse));
        };
    };

    void bvisit(const Cot &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return 1.0 / std::tan(tmp(x, cse));
        };
    };

    void bvisit(const Csc &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return 1.0 / std::sin(tmp(x, cse));
        };
    };

    void bvisit(const Sec &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return 1.0 / std::cos(tmp(x, cse));
        };
    };

    void bvisit(const ASin &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::asin(tmp(x, cse));
        };
    };

    void bvisit(const ACos &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::acos(tmp(x, cse));
        };
    };

    void bvisit(const ASec &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::acos(1.0 / tmp(x, cse));
        };
    };

    void bvisit(const ACsc &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::asin(1.0 / tmp(x, cse));
        };
    };

    void bvisit(const ATan &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::atan(tmp(x, cse));
        };
    };

    void bvisit(const ACot &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::atan(1.0 / tmp(x, cse));
        };
    };

    void bvisit(const Sinh &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::sinh(tmp(x, cse));
        };
    };

    void bvisit(const Csch &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return 1.0 / std::sinh(tmp(x, cse));
        };
    };

    void bvisit(const Cosh &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::cosh(tmp(x, cse));
        };
    };

    void bvisit(const Sech &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return 1.0 / std::cosh(tmp(x, cse));
        };
    };

    void bvisit(const Tanh &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::tanh(tmp(x, cse));
        };
    };

    void bvisit(const Coth &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return 1.0 / std::tanh(tmp(x, cse));
        };
    };

    void bvisit(const ASinh &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::asinh(tmp(x, cse));
        };
    };

    void bvisit(const ACsch &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::asinh(1.0 / tmp(x, cse));
        };
    };

    void bvisit(const ACosh &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::acosh(tmp(x, cse));
        };
    };

    void bvisit(const ATanh &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::atanh(tmp(x, cse));
        };
    };

    void bvisit(const ACoth &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::atanh(1.0 / tmp(x, cse));
        };
    };

    void bvisit(const ASech &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::acosh(1.0 / tmp(x, cse));
        };
    };

    void bvisit(const Constant &x)
    {
        T tmp = eval_double(x);
        result_ = [=](const T *x, const std::vector<T> *cse) { return tmp; };
    };

    void bvisit(const Abs &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const T *x, const std::vector<T> *cse) {
            return std::abs(tmp(x, cse));
        };
    };

    void bvisit(const Basic &)
    {
        throw NotImplementedError("Not Implemented");
    };

    void bvisit(const UnevaluatedExpr &x)
    {
        apply(*x.get_arg());
    };
};

class LambdaRealDoubleVisitor
    : public BaseVisitor<LambdaRealDoubleVisitor, LambdaDoubleVisitor<double>>
{
public:
    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, FunctionSymbol, LambertW
    // Derivative, Complex, ComplexDouble, ComplexMPC

    using LambdaDoubleVisitor::bvisit;

    void bvisit(const ATan2 &x)
    {
        fn num = apply(*(x.get_num()));
        fn den = apply(*(x.get_den()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::atan2(num(x, cse), den(x, cse));
        };
    };

    void bvisit(const Gamma &x)
    {
        fn tmp = apply(*(x.get_args()[0]));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::tgamma(tmp(x, cse));
        };
    };

    void bvisit(const LogGamma &x)
    {
        fn tmp = apply(*(x.get_args()[0]));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::lgamma(tmp(x, cse));
        };
    };

    void bvisit(const Erf &x)
    {
        fn tmp = apply(*(x.get_args()[0]));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::erf(tmp(x, cse));
        };
    }

    void bvisit(const Erfc &x)
    {
        fn tmp = apply(*(x.get_args()[0]));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::erfc(tmp(x, cse));
        };
    }

    void bvisit(const Equality &x)
    {
        fn lhs_ = apply(*(x.get_arg1()));
        fn rhs_ = apply(*(x.get_arg2()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return (lhs_(x, cse) == rhs_(x, cse));
        };
    }

    void bvisit(const Unequality &x)
    {
        fn lhs_ = apply(*(x.get_arg1()));
        fn rhs_ = apply(*(x.get_arg2()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return (lhs_(x, cse) != rhs_(x, cse));
        };
    }

    void bvisit(const LessThan &x)
    {
        fn lhs_ = apply(*(x.get_arg1()));
        fn rhs_ = apply(*(x.get_arg2()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return (lhs_(x, cse) <= rhs_(x, cse));
        };
    }

    void bvisit(const StrictLessThan &x)
    {
        fn lhs_ = apply(*(x.get_arg1()));
        fn rhs_ = apply(*(x.get_arg2()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return (lhs_(x, cse) < rhs_(x, cse));
        };
    }

    void bvisit(const And &x)
    {
        std::vector<fn> applys;
        for (const auto &p : x.get_args()) {
            applys.push_back(apply(*p));
        }

        result_ = [=](const double *x, const std::vector<double> *cse) {

            bool result = bool(applys[0](x, cse));
            for (unsigned int i = 0; i < applys.size(); i++) {
                result = result && bool(applys[i](x, cse));
            }
            return double(result);
        };
    }

    void bvisit(const Or &x)
    {
        std::vector<fn> applys;
        for (const auto &p : x.get_args()) {
            applys.push_back(apply(*p));
        }

        result_ = [=](const double *x, const std::vector<double> *cse) {

            bool result = bool(applys[0](x, cse));
            for (unsigned int i = 0; i < applys.size(); i++) {
                result = result || bool(applys[i](x, cse));
            }
            return double(result);
        };
    }

    void bvisit(const Xor &x)
    {
        std::vector<fn> applys;
        for (const auto &p : x.get_args()) {
            applys.push_back(apply(*p));
        }

        result_ = [=](const double *x, const std::vector<double> *cse) {

            bool result = bool(applys[0](x, cse));
            for (unsigned int i = 0; i < applys.size(); i++) {
                result = result != bool(applys[i](x, cse));
            }
            return double(result);
        };
    }

    void bvisit(const Not &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return double(not bool(tmp(x, cse)));
        };
    }

    void bvisit(const Max &x)
    {
        std::vector<fn> applys;
        for (const auto &p : x.get_args()) {
            applys.push_back(apply(*p));
        }

        result_ = [=](const double *x, const std::vector<double> *cse) {

            double result = applys[0](x, cse);
            for (unsigned int i = 0; i < applys.size(); i++) {
                result = std::max(result, applys[i](x, cse));
            }
            return result;
        };
    };

    void bvisit(const Min &x)
    {
        std::vector<fn> applys;
        for (const auto &p : x.get_args()) {
            applys.push_back(apply(*p));
        }

        result_ = [=](const double *x, const std::vector<double> *cse) {

            double result = applys[0](x, cse);
            for (unsigned int i = 0; i < applys.size(); i++) {
                result = std::min(result, applys[i](x, cse));
            }
            return result;
        };
    };

    void bvisit(const Sign &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return tmp(x, cse) == 0.0 ? 0.0 : (tmp(x, cse) < 0.0 ? -1.0 : 1.0);
        };
    };

    void bvisit(const Floor &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::floor(tmp(x, cse));
        };
    };

    void bvisit(const Ceiling &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::ceil(tmp(x, cse));
        };
    };

    void bvisit(const Truncate &x)
    {
        fn tmp = apply(*(x.get_arg()));
        result_ = [=](const double *x, const std::vector<double> *cse) {
            return std::trunc(tmp(x, cse));
        };
    };

    void bvisit(const Infty &x)
    {
        if (x.is_negative_infinity()) {
            result_
                = [=](const double * /* x */, const std::vector<double> *cse) {
                      return -std::numeric_limits<double>::infinity();
                  };
        } else if (x.is_positive_infinity()) {
            result_
                = [=](const double * /* x */, const std::vector<double> *cse) {
                      return std::numeric_limits<double>::infinity();
                  };
        } else {
            throw SymEngineException(
                "LambdaDouble can only represent real valued infinity");
        }
    }

    void bvisit(const Contains &cts)
    {
        const auto fn_expr = apply(*cts.get_expr());
        const auto set = cts.get_set();
        if (is_a<Interval>(*set)) {
            const auto &interv = down_cast<const Interval &>(*set);
            const auto fn_start = apply(*interv.get_start());
            const auto fn_end = apply(*interv.get_end());
            const bool left_open = interv.get_left_open();
            const bool right_open = interv.get_right_open();
            result_ = [=](const double *x, const std::vector<double> *cse) {
                const auto val_expr = fn_expr(x, cse);
                const auto val_start = fn_start(x, cse);
                const auto val_end = fn_end(x, cse);
                bool left_ok, right_ok;
                if (val_start == -std::numeric_limits<double>::infinity()) {
                    left_ok = !std::isnan(val_expr);
                } else {
                    left_ok = (left_open) ? (val_start < val_expr)
                                          : (val_start <= val_expr);
                }
                if (val_end == std::numeric_limits<double>::infinity()) {
                    right_ok = !std::isnan(val_expr);
                } else {
                    right_ok = (right_open) ? (val_expr < val_end)
                                            : (val_expr <= val_end);
                }
                return (left_ok && right_ok) ? 1.0 : 0.0;
            };
        } else {
            throw SymEngineException("LambdaDoubleVisitor: only ``Interval`` "
                                     "implemented for ``Contains``.");
        }
    }

    void bvisit(const BooleanAtom &ba)
    {
        const bool val = ba.get_val();
        result_ = [=](const double * /* x */, const std::vector<double> *cse) {
            return (val) ? 1.0 : 0.0;
        };
    }

    void bvisit(const Piecewise &pw)
    {
        SYMENGINE_ASSERT_MSG(
            eq(*pw.get_vec().back().second, *boolTrue),
            "LambdaDouble requires a (Expr, True) at the end of Piecewise");

        std::vector<fn> applys;
        std::vector<fn> preds;
        for (const auto &expr_pred : pw.get_vec()) {
            applys.push_back(apply(*expr_pred.first));
            preds.push_back(apply(*expr_pred.second));
        }
        result_ = [=](const double *x, const std::vector<double> *cse) {
            for (size_t i = 0;; ++i) {
                if (preds[i](x, cse) == 1.0) {
                    return applys[i](x, cse);
                }
            }
            throw SymEngineException(
                "Unexpectedly reached end of Piecewise function.");
        };
    }
};

class LambdaComplexDoubleVisitor
    : public BaseVisitor<LambdaComplexDoubleVisitor,
                         LambdaDoubleVisitor<std::complex<double>>>
{
public:
    // Classes not implemented are
    // Subs, UpperGamma, LowerGamma, Dirichlet_eta, Zeta
    // LeviCivita, KroneckerDelta, FunctionSymbol, LambertW
    // Derivative, ATan2, Gamma

    using LambdaDoubleVisitor::bvisit;

    void bvisit(const Complex &x)
    {
        double t1 = mp_get_d(x.real_), t2 = mp_get_d(x.imaginary_);
        result_ = [=](const std::complex<double> *x,
                      const std::vector<std::complex<double>> *cse) {
            return std::complex<double>(t1, t2);
        };
    };

    void bvisit(const ComplexDouble &x)
    {
        std::complex<double> tmp = x.i;
        result_
            = [=](const std::complex<double> *x,
                  const std::vector<std::complex<double>> *cse) { return tmp; };
    };
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &x)
    {
        mpfr_class t(x.get_prec());
        double real, imag;
        mpc_real(t.get_mpfr_t(), x.as_mpc().get_mpc_t(), MPFR_RNDN);
        real = mpfr_get_d(t.get_mpfr_t(), MPFR_RNDN);
        mpc_imag(t.get_mpfr_t(), x.as_mpc().get_mpc_t(), MPFR_RNDN);
        imag = mpfr_get_d(t.get_mpfr_t(), MPFR_RNDN);
        std::complex<double> tmp(real, imag);
        result_
            = [=](const std::complex<double> *x,
                  const std::vector<std::complex<double>> *cse) { return tmp; };
    }
#endif
};
}
#endif // SYMENGINE_LAMBDA_DOUBLE_H
