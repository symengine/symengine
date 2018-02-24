#ifndef SYMENGINE_MATHML_H
#define SYMENGINE_MATHML_H

#include <symengine/visitor.h>
#include <symengine/printer.h>

namespace SymEngine
{
class MathMLPrinter : public BaseVisitor<MathMLPrinter, StrPrinter>
{
protected:
    std::ostringstream s;

public:
    static const std::vector<std::string> names_;
    void bvisit(const Basic &x);
    void bvisit(const Symbol &x);
    void bvisit(const Integer &x);
    void bvisit(const Rational &x);
    // void bvisit(const Complex &x);
    // void bvisit(const Interval &x);
    // void bvisit(const Piecewise &x);
    // void bvisit(const EmptySet &x);
    // void bvisit(const FiniteSet &x);
    // void bvisit(const UniversalSet &x);
    // void bvisit(const ConditionSet &x);
    // void bvisit(const Contains &x);
    // void bvisit(const BooleanAtom &x);
    // void bvisit(const And &x);
    // void bvisit(const Or &x);
    // void bvisit(const Xor &x);
    // void bvisit(const Not &x);
    // void bvisit(const Union &x);
    // void bvisit(const Complement &x);
    // void bvisit(const ImageSet &x);
    void bvisit(const Add &x);
    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    // void bvisit(const UIntPoly &x);
    // void bvisit(const MIntPoly &x);
    //     void bvisit(const URatPoly &x);
    // #ifdef HAVE_SYMENGINE_FLINT
    //     void bvisit(const UIntPolyFlint &x);
    //     void bvisit(const URatPolyFlint &x);
    // #endif
    //     void bvisit(const UExprPoly &x);
    //     void bvisit(const MExprPoly &x);
    //     void bvisit(const GaloisField &x);
    //     void bvisit(const Infty &x);
    //     void bvisit(const NaN &x);
    //     void bvisit(const UnivariateSeries &x);
    // #ifdef HAVE_SYMENGINE_PIRANHA
    //     void bvisit(const URatPSeriesPiranha &x);
    //     void bvisit(const UPSeriesPiranha &x);
    //     void bvisit(const UIntPolyPiranha &x);
    //     void bvisit(const URatPolyPiranha &x);
    // // #endif
    //     void bvisit(const Log &x);
    //     void bvisit(const Constant &x);
    //     void bvisit(const Function &x);
    //     void bvisit(const FunctionSymbol &x);
    //     void bvisit(const Derivative &x);
    //     void bvisit(const Subs &x);
    //     void bvisit(const RealDouble &x);
    //     void bvisit(const ComplexDouble &x);
    //     void bvisit(const Equality &x);
    //     void bvisit(const Unequality &x);
    //     void bvisit(const LessThan &x);
    //     void bvisit(const StrictLessThan &x);
    // #ifdef HAVE_SYMENGINE_MPFR
    //     void bvisit(const RealMPFR &x);
    // #endif
    // #ifdef HAVE_SYMENGINE_MPC
    //     void bvisit(const ComplexMPC &x);
    // #endif
    //     void bvisit(const NumberWrapper &x);

    // virtual void _print_pow(std::ostringstream &o, const RCP<const Basic> &a,
    // const RCP<const Basic> &b);

    // std::string parenthesizeLT(const RCP<const Basic> &x,
    //                            PrecedenceEnum precedenceEnum);
    // std::string parenthesizeLE(const RCP<const Basic> &x,
    //                            PrecedenceEnum precedenceEnum);

    // std::string apply(const RCP<const Basic> &b);
    // std::string apply(const vec_basic &v);
    std::string apply(const Basic &b);
};

std::string mathml(const Basic &x);
}

#endif // SYMENGINE_MATHML_H
