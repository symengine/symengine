#include <limits>
#include <symengine/mathml.h>
#include <symengine/eval_double.h>
#include <symengine/printer.h>

namespace SymEngine
{

std::vector<std::string> init_mathml_printer_names()
{
    std::vector<std::string> names = init_str_printer_names();
    names[ASIN] = "arcsin";
    names[ACOS] = "arccos";
    names[ASEC] = "arcsec";
    names[ACSC] = "arccsc";
    names[ATAN] = "arctan";
    names[ACOT] = "arccot";
    names[ASINH] = "arcsinh";
    names[ACSCH] = "arccsch";
    names[ACOSH] = "arccosh";
    names[ATANH] = "arctanh";
    names[ACOTH] = "arccoth";
    names[ASECH] = "arcsech";
    return names;
}
const std::vector<std::string> MathMLPrinter::names_ = init_mathml_printer_names();

void MathMLPrinter::bvisit(const Basic &x)
{
    throw SymEngineException("Error: not supported");
}

void MathMLPrinter::bvisit(const Symbol &x)
{
    s << "<ci>" << x.get_name() << "</ci>";
}

void MathMLPrinter::bvisit(const Integer &x)
{
    s << "<cn>" << x.as_integer_class() << "</cn>";
}

void MathMLPrinter::bvisit(const Rational &x)
{
    const auto &rational = x.as_rational_class();
    s << "<cn type=\"rational\">" << get_num(rational) << "<sep/>"
      << get_den(rational) << "</cn>";
}

void MathMLPrinter::bvisit(const Complex &x)
{
    s << "<cn type=\"complex-cartesian\">";
    x.real_part()->accept(*this);
    x.imaginary_part()->accept(*this);
    s << "</cn>";
}

void MathMLPrinter::bvisit(const Interval &x)
{
    s << "<interval closure=";
    if (x.get_left_open()) {
        if (x.get_right_open()) {
            s << "\"open\">";
        } else {
            s << "\"open-closed\">";
        }
    } else {
        if (x.get_right_open()) {
            s << "\"closed-open\">";
        } else {
            s << "\"closed\">";
        }
    }
    x.get_start()->accept(*this);
    x.get_end()->accept(*this);
    s << "</interval>";
}

void MathMLPrinter::bvisit(const Piecewise &x)
{
    s << "<piecewise>";
    const auto &equations = x.get_vec();
    for (const auto &equation : equations) {
        s << "<piece>";
        equation.first->accept(*this);
        equation.second->accept(*this);
        s << "</piece>";
    }
    s << "</piecewise>";
}

void MathMLPrinter::bvisit(const BooleanAtom &x)
{
    if (x.get_val()) {
        s << "<true/>";
    } else {
        s << "<false/>";
    }
}

void MathMLPrinter::bvisit(const And &x)
{
    s << "<apply><and/>";
    const auto &conditions = x.get_args();
    for (const auto &condition : conditions) {
        condition->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Or &x)
{
    s << "<apply><or/>";
    const auto &conditions = x.get_args();
    for (const auto &condition : conditions) {
        condition->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Xor &x)
{
    s << "<apply><xor/>";
    const auto &conditions = x.get_args();
    for (const auto &condition : conditions) {
        condition->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Not &x)
{
    s << "<apply><not/>";
    x.get_arg()->accept(*this);
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Add &x)
{
    s << "<apply><plus/>";
    auto args = x.get_args();
    for (auto arg : args) {
        arg->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Mul &x)
{
    s << "<apply><times/>";
    auto args = x.get_args();
    for (auto arg : args) {
        arg->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Pow &x)
{
    s << "<apply><power/>";
    x.get_base()->accept(*this);
    x.get_exp()->accept(*this);
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Constant &x)
{
    s << "<";
    if (eq(x, *pi)) {
        s << "pi/";
    } else if (eq(x, *E)) {
        s << "exponentiale/";
    } else if (eq(x, *EulerGamma)) {
        s << "eulergamma/";
    } else {
        s << "cn type=\"real\">" << eval_double(x) << "</cn";
    }
    s << ">";
}

void MathMLPrinter::bvisit(const Function &x)
{
    s << "<apply>";
    s << "<" << names_[x.get_type_code()] << "/>";
    const auto &args = x.get_args();
    for (const auto &arg : args) {
        arg->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const FunctionSymbol &x)
{
    s << "<apply><ci>" << x.get_name() << "</ci>";
    const auto &args = x.get_args();
    for (const auto &arg : args) {
        arg->accept(*this);
    }
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Equality &x)
{
    s << "<apply><eq/>";
    x.get_arg1()->accept(*this);
    x.get_arg2()->accept(*this);
    s << "</apply>";
}

void MathMLPrinter::bvisit(const Unequality &x)
{
    s << "<apply><neq/>";
    x.get_arg1()->accept(*this);
    x.get_arg2()->accept(*this);
    s << "</apply>";
}

void MathMLPrinter::bvisit(const LessThan &x)
{
    s << "<apply><leq/>";
    x.get_arg1()->accept(*this);
    x.get_arg2()->accept(*this);
    s << "</apply>";
}

void MathMLPrinter::bvisit(const StrictLessThan &x)
{
    s << "<apply><lt/>";
    x.get_arg1()->accept(*this);
    x.get_arg2()->accept(*this);
    s << "</apply>";
}

std::string MathMLPrinter::apply(const Basic &b)
{
    b.accept(*this);
    return s.str();
}

std::string mathml(const Basic &x)
{
    MathMLPrinter m;
    return m.apply(x);
}
}