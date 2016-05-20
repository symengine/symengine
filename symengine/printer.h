#ifndef SYMENGINE_PRINTER_H
#define SYMENGINE_PRINTER_H

#include <symengine/visitor.h>

namespace SymEngine
{

std::string ascii_art();

enum class PrecedenceEnum { Add, Mul, Pow, Atom };

class Precedence : public BaseVisitor<Precedence>
{
public:
    PrecedenceEnum precedence;

    void bvisit(const Add &x)
    {
        precedence = PrecedenceEnum::Add;
    }

    void bvisit(const Mul &x)
    {
        precedence = PrecedenceEnum::Mul;
    }

    void bvisit(const Pow &x)
    {
        precedence = PrecedenceEnum::Pow;
    }

    void bvisit(const UnivariateIntPolynomial &x)
    {
        if (x.get_dict().size() == 1) {
            auto it = x.get_dict().begin();
            if (it->second == 1) {
                if (it->first == 1) {
                    precedence = PrecedenceEnum::Atom;
                } else {
                    precedence = PrecedenceEnum::Pow;
                }
            } else {
                precedence = PrecedenceEnum::Mul;
            }
        } else if (x.get_dict().size() == 0) {
            precedence = PrecedenceEnum::Atom;
        } else {
            precedence = PrecedenceEnum::Add;
        }
    }

    void bvisit(const UnivariatePolynomial &x)
    {
        if (x.get_dict().size() == 1) {
            auto it = x.get_dict().begin();
            precedence = PrecedenceEnum::Atom;
            if (it->second == 1) {
                if (it->first == 0 or it->first == 1) {
                    precedence = PrecedenceEnum::Atom;
                } else {
                    precedence = PrecedenceEnum::Pow;
                }
            } else {
                if (it->first == 0) {
                    it->second.get_basic()->accept(*this);
                } else {
                    precedence = PrecedenceEnum::Mul;
                }
            }
        } else {
            precedence = PrecedenceEnum::Add;
        }
    }

    void bvisit(const MultivariateIntPolynomial &x)
    {
        if (0 == x.dict_.size()) {
            precedence = PrecedenceEnum::Atom;
        } else if (1 == x.dict_.size()) {
            auto iter = x.dict_.begin();
            precedence = PrecedenceEnum::Atom;
            bool first = true; // true if there are no nonzero exponents, false
                               // otherwise
            for (unsigned int exp : iter->first) {
                if (exp > 0) {
                    if (first && exp > 1)
                        precedence = PrecedenceEnum::Pow;
                    if (!first)
                        precedence = PrecedenceEnum::Mul;
                    first = false;
                }
            }
            if (!first) {
                if (iter->second != 1)
                    precedence = PrecedenceEnum::Mul;
            }
        } else {
            precedence = PrecedenceEnum::Add;
        }
    }

    void bvisit(const MultivariatePolynomial &x)
    {
        if (0 == x.dict_.size()) {
            precedence = PrecedenceEnum::Atom;
        } else if (1 == x.dict_.size()) {
            auto iter = x.dict_.begin();
            precedence = PrecedenceEnum::Atom;
            bool first = true; // true if there are no nonzero exponents, false
                               // otherwise
            for (unsigned int exp : iter->first) {
                if (exp > 0) {
                    if (first && exp > 1)
                        precedence = PrecedenceEnum::Pow;
                    if (!first)
                        precedence = PrecedenceEnum::Mul;
                    first = false;
                }
            }
            if (first) {
                iter->second.get_basic()->accept(*this);
            } else {
                if (iter->second != 1)
                    precedence = PrecedenceEnum::Mul;
            }
        } else {
            precedence = PrecedenceEnum::Add;
        }
    }

    void bvisit(const Rational &x)
    {
        precedence = PrecedenceEnum::Add;
    }

    void bvisit(const Complex &x)
    {
        if (x.is_re_zero()) {
            if (x.imaginary_ == 1) {
                precedence = PrecedenceEnum::Atom;
            } else {
                precedence = PrecedenceEnum::Mul;
            }
        } else {
            precedence = PrecedenceEnum::Add;
        }
    }

    void bvisit(const Integer &x)
    {
        if (x.is_negative()) {
            precedence = PrecedenceEnum::Mul;
        } else {
            precedence = PrecedenceEnum::Atom;
        }
    }

    void bvisit(const RealDouble &x)
    {
        if (x.is_negative()) {
            precedence = PrecedenceEnum::Mul;
        } else {
            precedence = PrecedenceEnum::Atom;
        }
    }

#ifdef HAVE_SYMENGINE_PIRANHA
    void bvisit(const URatPSeriesPiranha &x)
    {
        precedence = PrecedenceEnum::Add;
    }

    void bvisit(const UPSeriesPiranha &x)
    {
        precedence = PrecedenceEnum::Add;
    }
#endif
    void bvisit(const ComplexDouble &x)
    {
        precedence = PrecedenceEnum::Add;
    }
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x)
    {
        if (x.is_negative()) {
            precedence = PrecedenceEnum::Mul;
        } else {
            precedence = PrecedenceEnum::Atom;
        }
    }
#endif
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &x)
    {
        precedence = PrecedenceEnum::Add;
    }
#endif

    void bvisit(const Basic &x)
    {
        precedence = PrecedenceEnum::Atom;
    }

    PrecedenceEnum getPrecedence(const RCP<const Basic> &x)
    {
        (*x).accept(*this);
        return precedence;
    }
};

class StrPrinter : public BaseVisitor<StrPrinter>
{
protected:
    std::string str_;

public:
    static const std::vector<std::string> names_;
    void bvisit(const Basic &x);
    void bvisit(const Symbol &x);
    void bvisit(const Integer &x);
    void bvisit(const Rational &x);
    void bvisit(const Complex &x);
    void bvisit(const Interval &x);
    void bvisit(const EmptySet &x);
    void bvisit(const UniversalSet &x);
    void bvisit(const Add &x);
    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    void bvisit(const UnivariateIntPolynomial &x);
    void bvisit(const MultivariateIntPolynomial &x);
    void bvisit(const MultivariatePolynomial &x);
    void bvisit(const UnivariatePolynomial &x);
    void bvisit(const UnivariateSeries &x);
    void bvisit(const MultivariateSeries &x);
#ifdef HAVE_SYMENGINE_PIRANHA
    void bvisit(const URatPSeriesPiranha &x);
    void bvisit(const UPSeriesPiranha &x);
#endif
    void bvisit(const Log &x);
    void bvisit(const Constant &x);
    void bvisit(const Function &x);
    void bvisit(const FunctionSymbol &x);
    void bvisit(const Derivative &x);
    void bvisit(const Subs &x);
    void bvisit(const RealDouble &x);
    void bvisit(const ComplexDouble &x);
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x);
#endif
#ifdef HAVE_SYMENGINE_MPC
    void bvisit(const ComplexMPC &x);
#endif
    void bvisit(const NumberWrapper &x);

    std::string parenthesizeLT(const RCP<const Basic> &x,
                               PrecedenceEnum precedenceEnum);
    std::string parenthesizeLE(const RCP<const Basic> &x,
                               PrecedenceEnum precedenceEnum);

    std::string apply(const RCP<const Basic> &b);
    std::string apply(const vec_basic &v);
    std::string apply(const Basic &b);
};
}

#endif // SYMENGINE_PRINTER_H
