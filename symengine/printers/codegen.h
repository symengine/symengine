#ifndef SYMENGINE_CODEGEN_H
#define SYMENGINE_CODEGEN_H

#include <symengine/printers.h>
#include <symengine/visitor.h>
#include <symengine/printers/strprinter.h>
#include <symengine/symengine_exception.h>

namespace SymEngine
{

class CodePrinter : public RewriteTrigVisitor<CodePrinter, StrPrinter>
{
public:
    explicit CodePrinter(CodePrinterPrecision precision
                         = CodePrinterPrecision::Double);
    using StrPrinter::apply;
    using StrPrinter::bvisit;
    using StrPrinter::str_;
    void bvisit(const Basic &x);
    void bvisit(const Complex &x);
    void bvisit(const Dummy &x);
    void bvisit(const Interval &x);
    void bvisit(const Contains &x);
    void bvisit(const Piecewise &x);
    void bvisit(const BooleanAtom &x);
    void bvisit(const And &x);
    void bvisit(const Or &x);
    void bvisit(const Xor &x);
    void bvisit(const Not &x);
    void bvisit(const Integer &x);
    void bvisit(const Rational &x);
    void bvisit(const EmptySet &x);
    void bvisit(const FiniteSet &x);
    void bvisit(const Reals &x);
    void bvisit(const Rationals &x);
    void bvisit(const Integers &x);
    void bvisit(const UniversalSet &x);
    void bvisit(const Abs &x);
    void bvisit(const Ceiling &x);
    void bvisit(const Truncate &x);
    void bvisit(const Max &x);
    void bvisit(const Min &x);
    void bvisit(const Constant &x);
    void bvisit(const NaN &x);
    void bvisit(const Equality &x);
    void bvisit(const Unequality &x);
    void bvisit(const LessThan &x);
    void bvisit(const StrictLessThan &x);
    void bvisit(const Sign &x);
    void bvisit(const UnevaluatedExpr &x);
    void bvisit(const UnivariateSeries &x);
    void bvisit(const Derivative &x);
    void bvisit(const Subs &x);
    void bvisit(const GaloisField &x);
    void bvisit(const Function &x);
    void bvisit(const RealDouble &x);
#ifdef HAVE_SYMENGINE_MPFR
    void bvisit(const RealMPFR &x);
#endif

protected:
    CodePrinterPrecision precision_;
    std::string print_scalar_literal(double d) const;
    std::string print_math_function(const std::string &name) const;
    virtual std::string
    format_codegen_function_name(const std::string &name) const;
    std::string print_binary_reduction(const vec_basic &args,
                                       const std::string &func_name);
    std::string print_binary_reduction_impl(vec_basic::const_iterator begin,
                                            vec_basic::const_iterator end,
                                            const std::string &func_name);
};

class C89CodePrinter : public RewriteTrigVisitor<C89CodePrinter, CodePrinter>
{
public:
    explicit C89CodePrinter(CodePrinterPrecision precision
                            = CodePrinterPrecision::Double);
    using CodePrinter::apply;
    using CodePrinter::bvisit;
    using CodePrinter::str_;
    void bvisit(const Infty &x);
    void _print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                    const RCP<const Basic> &b) override;
};

class C99CodePrinter : public RewriteTrigVisitor<C99CodePrinter, C89CodePrinter>
{
public:
    explicit C99CodePrinter(CodePrinterPrecision precision
                            = CodePrinterPrecision::Double);
    using C89CodePrinter::apply;
    using C89CodePrinter::bvisit;
    using C89CodePrinter::str_;
    void bvisit(const Infty &x);
    void _print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                    const RCP<const Basic> &b) override;
    void bvisit(const Gamma &x);
    void bvisit(const LogGamma &x);
};

class CudaCodePrinter
    : public RewriteTrigVisitor<CudaCodePrinter, C99CodePrinter>
{
public:
    explicit CudaCodePrinter(CodePrinterPrecision precision
                             = CodePrinterPrecision::Double);
    using C99CodePrinter::apply;
    using C99CodePrinter::bvisit;
    using C99CodePrinter::str_;
    void bvisit(const Integer &x);
    void bvisit(const Constant &x);
    void bvisit(const NaN &x);
    void bvisit(const Infty &x);
};

class MetalCodePrinter
    : public RewriteTrigVisitor<MetalCodePrinter, CodePrinter>
{
public:
    explicit MetalCodePrinter(CodePrinterPrecision precision
                              = CodePrinterPrecision::Float);
    using CodePrinter::apply;
    using CodePrinter::bvisit;
    using CodePrinter::str_;
    void bvisit(const Constant &x);
    void bvisit(const NaN &x);
    void bvisit(const Infty &x);
    void bvisit(const Abs &x);
    void bvisit(const Ceiling &x);
    void bvisit(const Truncate &x);
    void bvisit(const Max &x);
    void bvisit(const Min &x);
    void _print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                    const RCP<const Basic> &b) override;

protected:
    std::string
    format_codegen_function_name(const std::string &name) const override;
};

class JSCodePrinter : public RewriteTrigVisitor<JSCodePrinter, CodePrinter>
{
public:
    using CodePrinter::apply;
    using CodePrinter::bvisit;
    using CodePrinter::str_;
    void bvisit(const Constant &x);
    void _print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                    const RCP<const Basic> &b) override;
    void bvisit(const Abs &x);
    void bvisit(const Sin &x);
    void bvisit(const Cos &x);
    void bvisit(const Max &x);
    void bvisit(const Min &x);

protected:
    std::string
    format_codegen_function_name(const std::string &name) const override;
};
} // namespace SymEngine

#endif // SYMENGINE_CODEGEN_H
