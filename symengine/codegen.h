#ifndef SYMENGINE_CODEGEN_H
#define SYMENGINE_CODEGEN_H

#include <symengine/visitor.h>
#include <symengine/printer.h>
#include <symengine/symengine_exception.h>

namespace SymEngine
{

class CodePrinter : public BaseVisitor<CodePrinter, StrPrinter>
{
public:
    using StrPrinter::str_;
    using StrPrinter::apply;
    using StrPrinter::bvisit;
    void bvisit(const Basic &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const Complex &x)
    {
        throw NotImplementedError("Not implemented");
    }
    void bvisit(const Interval &x)
    {
        std::string var = str_;
        std::ostringstream s;
        bool is_inf = eq(*x.get_start(), *NegInf);
        if (not is_inf) {
            s << var;
            if (x.get_left_open()) {
                s << " > ";
            } else {
                s << " >= ";
            }
            s << apply(x.get_start());
        }
        if (neq(*x.get_end(), *Inf)) {
            if (not is_inf) {
                s << " && ";
            }
            s << var;
            if (x.get_right_open()) {
                s << " < ";
            } else {
                s << " <= ";
            }
            s << apply(x.get_end());
        }
        str_ = s.str();
    }
    void bvisit(const Contains &x)
    {
        x.get_expr()->accept(*this);
        x.get_set()->accept(*this);
    }
    void bvisit(const Piecewise &x)
    {
        std::ostringstream s;
        auto vec = x.get_vec();
        for (size_t i = 0;; ++i) {
            if (i == vec.size() - 1) {
                if (neq(*vec[i].second, *boolTrue)) {
                    throw SymEngineException(
                        "Code generation requires a (Expr, True) at the end");
                }
                s << "(\n   " << apply(vec[i].first) << "\n";
                break;
            } else {
                s << "((";
                s << apply(vec[i].second);
                s << ") ? (\n   ";
                s << apply(vec[i].first);
                s << "\n)\n: ";
            }
        }
        for (size_t i = 0; i < vec.size(); i++) {
            s << ")";
        }
        str_ = s.str();
    }
    void bvisit(const Rational &x)
    {
        std::ostringstream o;
        double n = mp_get_d(get_num(x.as_rational_class()));
        double d = mp_get_d(get_den(x.as_rational_class()));
        o << print_double(n) << "/" << print_double(d);
        str_ = o.str();
    }
    void bvisit(const EmptySet &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const FiniteSet &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const UniversalSet &x)
    {
        throw SymEngineException("Not supported");
    }
    void _print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                    const RCP<const Basic> &b)
    {
        if (eq(*b, *rational(1, 2))) {
            o << "sqrt(" << apply(a) << ")";
        } else {
            o << "pow(" << apply(a) << ", " << apply(b) << ")";
        }
    }
    void bvisit(const Abs &x)
    {
        std::ostringstream s;
        s << "fabs(" << apply(x.get_arg()) << ")";
        str_ = s.str();
    }
    void bvisit(const Ceiling &x)
    {
        std::ostringstream s;
        s << "ceil(" << apply(x.get_arg()) << ")";
        str_ = s.str();
    }
    void bvisit(const Gamma &x)
    {
        std::ostringstream s;
        s << "tgamma(" << apply(x.get_arg()) << ")";
        str_ = s.str();
    }
    void bvisit(const LogGamma &x)
    {
        std::ostringstream s;
        s << "lgamma(" << apply(x.get_arg()) << ")";
        str_ = s.str();
    }
    void bvisit(const Max &x)
    {
        std::ostringstream s;
        const auto &args = x.get_args();
        switch (args.size()) {
            case 0:
            case 1:
                throw SymEngineException("Impossible");
            case 2:
                s << "fmax(" << apply(args[0]) << ", " << apply(args[1]) << ")";
                break;
            default: {
                vec_basic inner_args(args.begin() + 1, args.end());
                auto inner = max(inner_args);
                s << "fmax(" << apply(args[0]) << ", " << apply(inner) << ")";
                break;
            }
        }
        str_ = s.str();
    }
    void bvisit(const Min &x)
    {
        std::ostringstream s;
        const auto &args = x.get_args();
        switch (args.size()) {
            case 0:
            case 1:
                throw SymEngineException("Impossible");
            case 2:
                s << "fmin(" << apply(args[0]) << ", " << apply(args[1]) << ")";
                break;
            default: {
                vec_basic inner_args(args.begin() + 1, args.end());
                auto inner = min(inner_args);
                s << "fmin(" << apply(args[0]) << ", " << apply(inner) << ")";
                break;
            }
        }
        str_ = s.str();
    }
    void bvisit(const Constant &x)
    {
        if (eq(x, *E)) {
            str_ = "M_E";
        } else if (eq(x, *pi)) {
            str_ = "M_PI";
        } else {
            str_ = x.get_name();
        }
    }
    void bvisit(const Infty &x)
    {
        std::ostringstream s;
        if (x.is_negative_infinity())
            s << "-HUGE_VAL";
        else if (x.is_positive_infinity())
            s << "HUGE_VAL";
        else
            throw SymEngineException("Not supported");
        str_ = s.str();
    }
    void bvisit(const NaN &x)
    {
        std::ostringstream s;
        s << "NAN";
        str_ = s.str();
    }
    void bvisit(const Equality &x)
    {
        std::ostringstream s;
        s << apply(x.get_arg1()) << " == " << apply(x.get_arg2());
        str_ = s.str();
    }
    void bvisit(const Unequality &x)
    {
        std::ostringstream s;
        s << apply(x.get_arg1()) << " != " << apply(x.get_arg2());
        str_ = s.str();
    }
    void bvisit(const LessThan &x)
    {
        std::ostringstream s;
        s << apply(x.get_arg1()) << " <= " << apply(x.get_arg2());
        str_ = s.str();
    }
    void bvisit(const StrictLessThan &x)
    {
        std::ostringstream s;
        s << apply(x.get_arg1()) << " < " << apply(x.get_arg2());
        str_ = s.str();
    }
    void bvisit(const UnivariateSeries &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const Derivative &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const Subs &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const GaloisField &x)
    {
        throw SymEngineException("Not supported");
    }
};

std::string ccode(const Basic &x)
{
    CodePrinter c;
    return c.apply(x);
}
}

#endif // SYMENGINE_CODEGEN_H
