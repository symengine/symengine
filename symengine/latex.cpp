#include <limits>
#include <symengine/latex.h>
#include <symengine/basic-inl.h>

namespace SymEngine
{

std::string latex(const Basic &x)
{
    LatexPrinter p;
    return p.apply(x);
}

void print_rational_class(const rational_class &r, std::ostringstream &s)
{
    if (get_den(r) == 1) {
        s << get_num(r);
    } else {
        s << "\\frac{" << get_num(r) << "}{" << get_den(r) << "}";
    }
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
            print_rational_class(mp_abs(x.imaginary_), s);
            s << "j";
        } else {
            s << "j";
        }
    } else {
        if (x.imaginary_ != mp_sign(x.imaginary_)) {
            print_rational_class(x.imaginary_, s);
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

void LatexPrinter::bvisit(const ComplexBase &x)
{
    RCP<const Number> imag = x.imaginary_part();
    if (imag->is_negative()) {
        std::string str = apply(imag);
        str = str.substr(1, str.length() - 1);
        str_ = apply(x.real_part()) + " - " + str + "j";
    } else {
        str_ = apply(x.real_part()) + " + " + apply(imag) + "j";
    }
}
void LatexPrinter::bvisit(const ComplexDouble &x)
{
    bvisit(static_cast<const ComplexBase &>(x));
}

#ifdef HAVE_SYMENGINE_MPC
void LatexPrinter::bvisit(const ComplexMPC &x)
{
    bvisit(static_cast<const ComplexBase &>(x));
}
#endif
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

void LatexPrinter::bvisit(const StrictLessThan &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " < " << apply(x.get_arg2());
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
    if (is_a<Or>(**container.begin()) or is_a<Xor>(**container.begin())) {
        s << parenthesize(apply(*container.begin()));
    } else {
        s << apply(*container.begin());
    }

    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << " \\wedge ";
        if (is_a<Or>(**it) or is_a<Xor>(**it)) {
            s << parenthesize(apply(*it));
        } else {
            s << apply(*it);
        }
    }
    str_ = s.str();
}

void LatexPrinter::bvisit(const Or &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    if (is_a<And>(**container.begin()) or is_a<Xor>(**container.begin())) {
        s << parenthesize(apply(*container.begin()));
    } else {
        s << apply(*container.begin());
    }

    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << " \\vee ";
        if (is_a<And>(**it) or is_a<Xor>(**it)) {
            s << parenthesize(apply(*it));
        } else {
            s << apply(*it);
        }
    }
    str_ = s.str();
}

void LatexPrinter::bvisit(const Xor &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    if (is_a<Or>(**container.begin()) or is_a<And>(**container.begin())) {
        s << parenthesize(apply(*container.begin()));
    } else {
        s << apply(*container.begin());
    }

    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << " \\veebar ";
        if (is_a<Or>(**it) or is_a<And>(**it)) {
            s << parenthesize(apply(*it));
        } else {
            s << apply(*it);
        }
    }
    str_ = s.str();
}

void LatexPrinter::print_with_args(const Basic &x, const std::string &join,
                                   std::ostringstream &s)
{
    vec_basic v = x.get_args();
    s << apply(*v.begin());

    for (auto it = ++(v.begin()); it != v.end(); ++it) {
        s << " " << join << " " << apply(*it);
    }
}

void LatexPrinter::bvisit(const Not &x)
{
    str_ = "\\neg " + apply(*x.get_arg());
}

void LatexPrinter::bvisit(const Union &x)
{
    std::ostringstream s;
    print_with_args(x, "\\cup", s);
    str_ = s.str();
}

void LatexPrinter::bvisit(const Complement &x)
{
    std::ostringstream s;
    s << apply(x.get_universe()) << " \\setminus " << apply(x.get_container());
    str_ = s.str();
}

void LatexPrinter::bvisit(const ImageSet &x)
{
    std::ostringstream s;
    s << "\\left\\{" << apply(*x.get_expr()) << "\\; |\\; ";
    s << apply(*x.get_symbol());
    s << " \\in " << apply(*x.get_baseset()) << "\\right\\}";
    str_ = s.str();
}

std::string LatexPrinter::print_mul()
{
    return " ";
}

std::vector<std::string> init_latex_printer_names()
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

const std::vector<std::string> LatexPrinter::names_
    = init_latex_printer_names();

void LatexPrinter::bvisit(const Function &x)
{
    std::ostringstream o;
    o << names_[x.get_type_code()];
    vec_basic vec = x.get_args();
    o << parenthesize(apply(vec));
    str_ = o.str();
}

std::string LatexPrinter::parenthesize(const std::string &expr)
{
    return "\\left(" + expr + "\\right)";
}

void LatexPrinter::_print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                              const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << "e^\\{" << apply(b) << "}";
    } else if (eq(*b, *rational(1, 2))) {
        o << "\\sqrt{" << apply(b) << "}";
    } else if (is_a<Rational>(*b)
               and eq(*static_cast<const Rational &>(*b).get_num(), *one)) {
        o << "\\sqrt[" << apply(static_cast<const Rational &>(*b).get_num())
          << "]{" << apply(b) << "}";
    } else {
        o << parenthesizeLE(a, PrecedenceEnum::Pow);
        Precedence prec;
        if (prec.getPrecedence(b) <= PrecedenceEnum::Pow) {
            o << "{" << apply(b) << "}";
        } else {
            o << apply(b);
        }
    }
}

std::string LatexPrinter::print_div(const std::string &num,
                                    const std::string &den)
{
    return "\\frac{" + num + "}{" + den + "}";
}
}
