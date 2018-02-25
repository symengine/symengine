#include <limits>
#include <symengine/latex.h>
#include <symengine/basic-inl.h>

namespace SymEngine
{

void print_rational_class(const rational_class &r, std::ostringstream &s)
{
    s << "\\frac{" << get_num(r) << "}{" << get_den(r) << "}";
}

std::string latex(const Basic &x)
{
    LatexPrinter p;
    return p.apply(x);
}

void LatexPrinter::bvisit(const Basic &x)
{
    std::ostringstream s;
    s << "<" << typeName<Basic>(x) << " instance at " << (const void *)this
      << ">";
    str_ = s.str();
}

void LatexPrinter::bvisit(const Rational &x)
{
    const auto &rational = x.as_rational_class();
    std::ostringstream s;
    print_rational_class(rational, s);
    str_ = s.str();
}

void LatexPrinter::bvisit(const Complex &x)
{
    std::ostringstream s;
    if (x.real_ != 0) {
        print_rational_class(x.real_, s);
        // Since Complex is in canonical form, imaginary_ is not 0.
        if (mp_sign(x.imaginary_) == 1) {
            s << " + ";
        } else {
            s << " - ";
        }
        // If imaginary_ is not 1 or -1, print the absolute value
        if (x.imaginary_ != mp_sign(x.imaginary_)) {
            s << mp_abs(x.imaginary_);
            s << "j";
        } else {
            s << "j";
        }
    } else {
        if (x.imaginary_ != mp_sign(x.imaginary_)) {
            s << x.imaginary_;
            s << "j";
        } else {
            if (mp_sign(x.imaginary_) == 1) {
                s << "j";
            } else {
                s << "-j";
            }
        }
    }
    str_ = s.str();
}

void LatexPrinter::bvisit(const ComplexDouble &x)
{
    str_ = print_double(x.i.real());
    if (x.i.imag() < 0) {
        str_ += " - " + print_double(-x.i.imag()) + "j";
    } else {
        str_ += " + " + print_double(x.i.imag()) + "j";
    }
}

void LatexPrinter::bvisit(const Equality &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " = " << apply(x.get_arg2());
    str_ = s.str();
}

void LatexPrinter::bvisit(const Unequality &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " \\neq " << apply(x.get_arg2());
    str_ = s.str();
}

void LatexPrinter::bvisit(const LessThan &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " \\leq " << apply(x.get_arg2());
    str_ = s.str();
}

void LatexPrinter::bvisit(const Interval &x)
{
    std::ostringstream s;
    if (x.get_left_open())
        s << "\\left(";
    else
        s << "\\left[";
    s << *x.get_start() << ", " << *x.get_end();
    if (x.get_right_open())
        s << "\\right)";
    else
        s << "\\right]";
    str_ = s.str();
}

void LatexPrinter::bvisit(const BooleanAtom &x)
{
    if (x.get_val()) {
        str_ = "\\mathrm{True}";
    } else {
        str_ = "\\mathrm{False}";
    }
}

void LatexPrinter::bvisit(const And &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    if (is_a<Or>(**container.begin())) {
        s << "\\left(";
    }
    s << apply(*container.begin());
    if (is_a<Or>(**container.begin())) {
        s << "\\right)";
    }
    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << " \\wedge ";
        if (is_a<Or>(**it)) {
            s << "\\left(";
        }
        s << apply(*it);
        if (is_a<Or>(**it)) {
            s << "\\right)";
        }
    }
    str_ = s.str();
}

void LatexPrinter::bvisit(const Or &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    if (is_a<And>(**container.begin())) {
        s << "\\left(";
    }
    s << apply(*container.begin());
    if (is_a<And>(**container.begin())) {
        s << "\\right)";
    }
    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << " \\vee ";
        if (is_a<Or>(**it)) {
            s << "\\left(";
        }
        s << apply(*it);
        if (is_a<Or>(**it)) {
            s << "\\right)";
        }
    }
    str_ = s.str();
}
}
