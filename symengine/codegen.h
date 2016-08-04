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
        bool is_inf = eq(*x.start_, *NegInf);
        if (not is_inf) {
            s << var;
            if (x.left_open_) {
                s << " > ";
            } else {
                s << " >= ";
            }
            s << apply(x.start_);
        }
        if (neq(*x.end_, *Inf)) {
            if (not is_inf) {
                s << " && ";
            }
            s << var;
            if (x.right_open_) {
                s << " < ";
            } else {
                s << " <= ";
            }
            s << apply(x.end_);
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
        double n = mp_get_d(get_num(x.i));
        double d = mp_get_d(get_den(x.i));
        o << n << "/" << d;
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
    void bvisit(const UnivariateSeries &x)
    {
        throw SymEngineException("Not supported");
    }
    void bvisit(const FunctionSymbol &x)
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
