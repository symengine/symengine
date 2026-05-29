#include <symengine/printers/codegen.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/visitor.h>
#include <symengine/printers.h>
#include <symengine/symengine_exception.h>

namespace SymEngine
{

namespace
{

const char *print_precision_suffix(CodePrinterPrecision precision)
{
    switch (precision) {
        case CodePrinterPrecision::Double:
            return "";
        case CodePrinterPrecision::Float:
            return "f";
        case CodePrinterPrecision::Half:
            return "h";
        default:
            throw SymEngineException("Unknown code printer precision");
    }
}

CodePrinterPrecision
normalize_code_printer_precision(CodePrinterPrecision precision)
{
    switch (precision) {
        case CodePrinterPrecision::Double:
        case CodePrinterPrecision::Float:
        case CodePrinterPrecision::Half:
            return precision;
        default:
            throw SymEngineException("Unknown code printer precision");
    }
}

} // namespace

CodePrinter::CodePrinter(CodePrinterPrecision precision)
    : precision_{normalize_code_printer_precision(precision)}
{
}

std::string CodePrinter::print_scalar_literal(double d) const
{
    return print_double(d) + print_precision_suffix(precision_);
}

std::string CodePrinter::print_math_function(const std::string &name) const
{
    if (precision_ == CodePrinterPrecision::Float) {
        return name + "f";
    }
    return name;
}

std::string
CodePrinter::format_codegen_function_name(const std::string &name) const
{
    return print_math_function(name);
}

std::string CodePrinter::print_binary_reduction(const vec_basic &args,
                                                const std::string &func_name)
{
    if (args.size() < 2) {
        throw SymEngineException("Impossible");
    }

    return print_binary_reduction_impl(args.begin(), args.end(), func_name);
}

std::string
CodePrinter::print_binary_reduction_impl(vec_basic::const_iterator begin,
                                         vec_basic::const_iterator end,
                                         const std::string &func_name)
{
    const auto size = static_cast<std::size_t>(std::distance(begin, end));
    if (size == 1) {
        return apply(*begin);
    }

    const auto mid = begin + size / 2;
    std::ostringstream s;
    s << func_name << "(" << print_binary_reduction_impl(begin, mid, func_name)
      << ", " << print_binary_reduction_impl(mid, end, func_name) << ")";
    return s.str();
}

void CodePrinter::bvisit(const Basic &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const Complex &x)
{
    throw NotImplementedError("Not implemented");
}
void CodePrinter::bvisit(const Dummy &x)
{
    std::ostringstream s;
    s << x.get_name() << '_' << x.get_index();
    str_ = s.str();
}
void CodePrinter::bvisit(const Interval &x)
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
void CodePrinter::bvisit(const Contains &x)
{
    x.get_expr()->accept(*this);
    x.get_set()->accept(*this);
}
void CodePrinter::bvisit(const Piecewise &x)
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
void CodePrinter::bvisit(const BooleanAtom &x)
{
    str_ = print_scalar_literal(x.get_val() ? 1.0 : 0.0);
}
void CodePrinter::bvisit(const Integer &x)
{
    if (precision_ != CodePrinterPrecision::Double) {
        str_ = print_scalar_literal(mp_get_d(x.as_integer_class()));
    } else {
        StrPrinter::bvisit(x);
    }
}
void CodePrinter::bvisit(const And &x)
{
    std::ostringstream s;
    const auto &container = x.get_container();
    s << "(";
    for (auto it = container.begin(); it != container.end(); ++it) {
        if (it != container.begin()) {
            s << " && ";
        }
        s << "(" << apply(*(*it)) << ")";
    }
    s << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Or &x)
{
    std::ostringstream s;
    const auto &container = x.get_container();
    s << "(";
    for (auto it = container.begin(); it != container.end(); ++it) {
        if (it != container.begin()) {
            s << " || ";
        }
        s << "(" << apply(*(*it)) << ")";
    }
    s << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Xor &x)
{
    std::ostringstream s;
    const auto &container = x.get_container();
    s << "(";
    for (auto it = container.begin(); it != container.end(); ++it) {
        if (it != container.begin()) {
            s << " != ";
        }
        s << "((" << apply(*(*it)) << ") != 0)";
    }
    s << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Not &x)
{
    std::ostringstream s;
    s << "!(" << apply(*x.get_arg()) << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Rational &x)
{
    std::ostringstream o;
    double n = mp_get_d(get_num(x.as_rational_class()));
    double d = mp_get_d(get_den(x.as_rational_class()));
    o << print_scalar_literal(n) << "/" << print_scalar_literal(d);
    str_ = o.str();
}
void CodePrinter::bvisit(const Reals &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const Rationals &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const Integers &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const EmptySet &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const FiniteSet &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const UniversalSet &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const Abs &x)
{
    std::ostringstream s;
    s << print_math_function("fabs") << "(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Ceiling &x)
{
    std::ostringstream s;
    s << print_math_function("ceil") << "(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Truncate &x)
{
    std::ostringstream s;
    s << print_math_function("trunc") << "(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void CodePrinter::bvisit(const Max &x)
{
    str_ = print_binary_reduction(x.get_args(), print_math_function("fmax"));
}
void CodePrinter::bvisit(const Min &x)
{
    str_ = print_binary_reduction(x.get_args(), print_math_function("fmin"));
}
void CodePrinter::bvisit(const Constant &x)
{
    if (eq(x, *E)) {
        str_ = precision_ != CodePrinterPrecision::Double
                   ? print_math_function("exp") + "("
                         + print_scalar_literal(1.0) + ")"
                   : "exp(1)";
    } else if (eq(x, *pi)) {
        str_ = precision_ != CodePrinterPrecision::Double
                   ? print_math_function("acos") + "("
                         + print_scalar_literal(-1.0) + ")"
                   : "acos(-1)";
    } else {
        str_ = x.get_name();
    }
}
void CodePrinter::bvisit(const NaN &x)
{
    std::ostringstream s;
    s << "NAN";
    str_ = s.str();
}
void CodePrinter::bvisit(const Equality &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " == " << apply(x.get_arg2());
    str_ = s.str();
}
void CodePrinter::bvisit(const Unequality &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " != " << apply(x.get_arg2());
    str_ = s.str();
}
void CodePrinter::bvisit(const LessThan &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " <= " << apply(x.get_arg2());
    str_ = s.str();
}
void CodePrinter::bvisit(const StrictLessThan &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " < " << apply(x.get_arg2());
    str_ = s.str();
}
void CodePrinter::bvisit(const Sign &x)
{
    const std::string arg = apply(x.get_arg());
    const std::string zero = print_scalar_literal(0.0);
    const std::string one = print_scalar_literal(1.0);
    const std::string minus_one = print_scalar_literal(-1.0);
    std::ostringstream s;
    s << "((" << arg << " == " << zero << ") ? (" << zero << ") : ((" << arg
      << " < " << zero << ") ? (" << minus_one << ") : (" << one << ")))";
    str_ = s.str();
}
void CodePrinter::bvisit(const UnevaluatedExpr &x)
{
    str_ = apply(x.get_arg());
}
void CodePrinter::bvisit(const UnivariateSeries &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const Derivative &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const Subs &x)
{
    throw SymEngineException("Not supported");
}
void CodePrinter::bvisit(const GaloisField &x)
{
    throw SymEngineException("Not supported");
}

void CodePrinter::bvisit(const Function &x)
{
    static const std::vector<std::string> names_ = init_str_printer_names();
    std::ostringstream o;
    o << format_codegen_function_name(names_[x.get_type_code()]);
    vec_basic vec = x.get_args();
    o << parenthesize(apply(vec));
    str_ = o.str();
}

void CodePrinter::bvisit(const RealDouble &x)
{
    if (precision_ != CodePrinterPrecision::Double) {
        str_ = print_scalar_literal(x.i);
    } else {
        StrPrinter::bvisit(x);
    }
}

#ifdef HAVE_SYMENGINE_MPFR
void CodePrinter::bvisit(const RealMPFR &x)
{
    StrPrinter::bvisit(x);
    if (precision_ != CodePrinterPrecision::Double) {
        str_ += print_precision_suffix(precision_);
    }
}
#endif

C89CodePrinter::C89CodePrinter(CodePrinterPrecision precision)
    : RewriteTrigVisitor<C89CodePrinter, CodePrinter>(precision)
{
    if (precision_ == CodePrinterPrecision::Half) {
        throw SymEngineException(
            "C-family code printers do not support half precision");
    }
}

void C89CodePrinter::bvisit(const Infty &x)
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
void C89CodePrinter::_print_pow(std::ostringstream &o,
                                const RCP<const Basic> &a,
                                const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << print_math_function("exp") << "(" << apply(b) << ")";
    } else if (eq(*b, *minus_one)) {
        o << apply(*one) << "/" << parenthesizeLE(a, PrecedenceEnum::Mul);
    } else if (eq(*b, *rational(1, 2))) {
        o << print_math_function("sqrt") << "(" << apply(a) << ")";
    } else {
        o << print_math_function("pow") << "(" << apply(a) << ", " << apply(b)
          << ")";
    }
}

C99CodePrinter::C99CodePrinter(CodePrinterPrecision precision)
    : RewriteTrigVisitor<C99CodePrinter, C89CodePrinter>(precision)
{
}

void C99CodePrinter::bvisit(const Infty &x)
{
    std::ostringstream s;
    if (x.is_negative_infinity())
        s << "-INFINITY";
    else if (x.is_positive_infinity())
        s << "INFINITY";
    else
        throw SymEngineException("Not supported");
    str_ = s.str();
}
void C99CodePrinter::_print_pow(std::ostringstream &o,
                                const RCP<const Basic> &a,
                                const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << print_math_function("exp") << "(" << apply(b) << ")";
    } else if (eq(*b, *minus_one)) {
        o << apply(*one) << "/" << parenthesizeLE(a, PrecedenceEnum::Mul);
    } else if (eq(*b, *rational(1, 2))) {
        o << print_math_function("sqrt") << "(" << apply(a) << ")";
    } else if (eq(*b, *rational(1, 3))) {
        o << print_math_function("cbrt") << "(" << apply(a) << ")";
    } else {
        o << print_math_function("pow") << "(" << apply(a) << ", " << apply(b)
          << ")";
    }
}
void C99CodePrinter::bvisit(const Gamma &x)
{
    std::ostringstream s;
    s << print_math_function("tgamma") << "(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void C99CodePrinter::bvisit(const LogGamma &x)
{
    std::ostringstream s;
    s << print_math_function("lgamma") << "(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}

CudaCodePrinter::CudaCodePrinter(CodePrinterPrecision precision)
    : RewriteTrigVisitor<CudaCodePrinter, C99CodePrinter>(precision)
{
}

void CudaCodePrinter::bvisit(const Integer &x)
{
    str_ = print_scalar_literal(mp_get_d(x.as_integer_class()));
}
void CudaCodePrinter::bvisit(const Constant &x)
{
    if (eq(x, *E)) {
        str_ = print_math_function("exp") + "(" + print_scalar_literal(1.0)
               + ")";
    } else if (eq(x, *pi)) {
        str_ = print_math_function("acos") + "(" + print_scalar_literal(-1.0)
               + ")";
    } else {
        str_ = x.get_name();
    }
}

void CudaCodePrinter::bvisit(const NaN &x)
{
    str_ = precision_ == CodePrinterPrecision::Float ? "CUDART_NAN_F"
                                                     : "CUDART_NAN";
}

void CudaCodePrinter::bvisit(const Infty &x)
{
    if (x.is_negative_infinity())
        str_ = precision_ == CodePrinterPrecision::Float ? "-CUDART_INF_F"
                                                         : "-CUDART_INF";
    else if (x.is_positive_infinity())
        str_ = precision_ == CodePrinterPrecision::Float ? "CUDART_INF_F"
                                                         : "CUDART_INF";
    else
        throw SymEngineException("Not supported");
}

MetalCodePrinter::MetalCodePrinter(CodePrinterPrecision precision)
    : RewriteTrigVisitor<MetalCodePrinter, CodePrinter>(precision)
{
    if (precision_ == CodePrinterPrecision::Double) {
        throw SymEngineException(
            "Metal code printer currently only supports float and half "
            "precision");
    }
}

void MetalCodePrinter::bvisit(const Constant &x)
{
    if (eq(x, *E)) {
        str_ = "exp(" + print_scalar_literal(1.0) + ")";
    } else if (eq(x, *pi)) {
        str_ = "acos(" + print_scalar_literal(-1.0) + ")";
    } else {
        str_ = x.get_name();
    }
}

void MetalCodePrinter::bvisit(const NaN &x)
{
    if (precision_ == CodePrinterPrecision::Half) {
        str_ = "half(NAN)";
    } else {
        str_ = "NAN";
    }
}

void MetalCodePrinter::bvisit(const Infty &x)
{
    if (x.is_negative_infinity()) {
        str_ = precision_ == CodePrinterPrecision::Half ? "-HUGE_VALH"
                                                        : "-INFINITY";
    } else if (x.is_positive_infinity()) {
        str_ = precision_ == CodePrinterPrecision::Half ? "HUGE_VALH"
                                                        : "INFINITY";
    } else {
        throw SymEngineException("Not supported");
    }
}

void MetalCodePrinter::bvisit(const Abs &x)
{
    std::ostringstream s;
    s << "fabs(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}

void MetalCodePrinter::bvisit(const Ceiling &x)
{
    std::ostringstream s;
    s << "ceil(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}

void MetalCodePrinter::bvisit(const Truncate &x)
{
    std::ostringstream s;
    s << "trunc(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}

void MetalCodePrinter::bvisit(const Max &x)
{
    str_ = print_binary_reduction(x.get_args(), "fmax");
}

void MetalCodePrinter::bvisit(const Min &x)
{
    str_ = print_binary_reduction(x.get_args(), "fmin");
}

std::string
MetalCodePrinter::format_codegen_function_name(const std::string &name) const
{
    return name;
}

void MetalCodePrinter::_print_pow(std::ostringstream &o,
                                  const RCP<const Basic> &a,
                                  const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << "exp(" << apply(b) << ")";
    } else if (eq(*b, *minus_one)) {
        o << apply(*one) << "/" << parenthesizeLE(a, PrecedenceEnum::Mul);
    } else if (eq(*b, *rational(1, 2))) {
        o << "sqrt(" << apply(a) << ")";
    } else {
        o << "pow(" << apply(a) << ", " << apply(b) << ")";
    }
}

void JSCodePrinter::bvisit(const Constant &x)
{
    if (eq(x, *E)) {
        str_ = "Math.E";
    } else if (eq(x, *pi)) {
        str_ = "Math.PI";
    } else {
        str_ = x.get_name();
    }
}
void JSCodePrinter::_print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                               const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << "Math.exp(" << apply(b) << ")";
    } else if (eq(*b, *minus_one)) {
        o << apply(*one) << "/" << parenthesizeLE(a, PrecedenceEnum::Mul);
    } else if (eq(*b, *rational(1, 2))) {
        o << "Math.sqrt(" << apply(a) << ")";
    } else if (eq(*b, *rational(1, 3))) {
        o << "Math.cbrt(" << apply(a) << ")";
    } else {
        o << "Math.pow(" << apply(a) << ", " << apply(b) << ")";
    }
}
void JSCodePrinter::bvisit(const Abs &x)
{
    std::ostringstream s;
    s << "Math.abs(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void JSCodePrinter::bvisit(const Sin &x)
{
    std::ostringstream s;
    s << "Math.sin(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void JSCodePrinter::bvisit(const Cos &x)
{
    std::ostringstream s;
    s << "Math.cos(" << apply(x.get_arg()) << ")";
    str_ = s.str();
}
void JSCodePrinter::bvisit(const Max &x)
{
    const auto &args = x.get_args();
    std::ostringstream s;
    s << "Math.max(";
    for (size_t i = 0; i < args.size(); ++i) {
        s << apply(args[i]);
        s << ((i == args.size() - 1) ? ")" : ", ");
    }
    str_ = s.str();
}
void JSCodePrinter::bvisit(const Min &x)
{
    const auto &args = x.get_args();
    std::ostringstream s;
    s << "Math.min(";
    for (size_t i = 0; i < args.size(); ++i) {
        s << apply(args[i]);
        s << ((i == args.size() - 1) ? ")" : ", ");
    }
    str_ = s.str();
}

std::string
JSCodePrinter::format_codegen_function_name(const std::string &name) const
{
    return "Math." + name;
}

std::string ccode(const Basic &x, CodePrinterPrecision precision)
{
    C99CodePrinter c(precision);
    return c.apply(x);
}

std::string cudacode(const Basic &x, CodePrinterPrecision precision)
{
    CudaCodePrinter p(precision);
    return p.apply(x);
}

std::string metalcode(const Basic &x, CodePrinterPrecision precision)
{
    MetalCodePrinter p(precision);
    return p.apply(x);
}

std::string jscode(const Basic &x)
{
    JSCodePrinter p;
    return p.apply(x);
}

std::string inline c89code(const Basic &x)
{
    C89CodePrinter p;
    return p.apply(x);
}

std::string inline c99code(const Basic &x)
{
    C99CodePrinter p;
    return p.apply(x);
}

} // namespace SymEngine
