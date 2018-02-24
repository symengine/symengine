#include <limits>
#include <symengine/mathml.h>

namespace SymEngine
{

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