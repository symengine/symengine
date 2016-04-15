#include <algorithm>
#include <limits>
#include <symengine/printer.h>

namespace SymEngine
{

std::string ascii_art()
{
    std::string a = " _____           _____         _         \n"
                    "|   __|_ _ _____|   __|___ ___|_|___ ___ \n"
                    "|__   | | |     |   __|   | . | |   | -_|\n"
                    "|_____|_  |_|_|_|_____|_|_|_  |_|_|_|___|\n"
                    "      |___|               |___|          \n";
    return a;
}

void StrPrinter::bvisit(const Basic &x)
{
    std::ostringstream s;
    s << "<" << typeName<Basic>(x) << " instance at " << (const void *)this
      << ">";
    str_ = s.str();
}

void StrPrinter::bvisit(const Symbol &x)
{
    str_ = x.get_name();
}

void StrPrinter::bvisit(const Integer &x)
{
    std::ostringstream s;
    s << x.i;
    str_ = s.str();
}

void StrPrinter::bvisit(const Rational &x)
{
    std::ostringstream s;
    s << x.i;
    str_ = s.str();
}

void StrPrinter::bvisit(const Complex &x)
{
    std::ostringstream s;
    if (x.real_ != 0) {
        s << x.real_;
        // Since Complex is in canonical form, imaginary_ is not 0.
        if (mp_sign(x.imaginary_) == 1) {
            s << " + ";
        } else {
            s << " - ";
        }
        // If imaginary_ is not 1 or -1, print the absolute value
        if (x.imaginary_ != mp_sign(x.imaginary_)) {
            s << mp_abs(x.imaginary_);
            s << "*I";
        } else {
            s << "I";
        }
    } else {
        if (x.imaginary_ != mp_sign(x.imaginary_)) {
            s << x.imaginary_;
            s << "*I";
        } else {
            if (mp_sign(x.imaginary_) == 1) {
                s << "I";
            } else {
                s << "-I";
            }
        }
    }
    str_ = s.str();
}

void StrPrinter::bvisit(const RealDouble &x)
{
    std::ostringstream s;
    s.precision(std::numeric_limits<double>::digits10);
    s << x.i;
    str_ = s.str();
    if (str_.find(".") == std::string::npos) {
        s << ".0";
        str_ = s.str();
    }
}

void StrPrinter::bvisit(const ComplexDouble &x)
{
    std::ostringstream s;
    s.precision(std::numeric_limits<double>::digits10);
    s << x.i.real();
    if (s.str().find(".") == std::string::npos) {
        s << ".0";
    }
    if (x.i.imag() < 0) {
        s << " - " << -x.i.imag();
    } else {
        s << " + " << x.i.imag();
    }
    str_ = s.str();
    if (str_.find(".") == str_.find_last_of(".")) {
        str_ += ".0*I";
    } else {
        str_ += "*I";
    }
}

void StrPrinter::bvisit(const Interval &x)
{
    std::ostringstream s;
    if (x.left_open_)
        s << "(";
    else
        s << "[";
    s << *x.start_ << ", " << *x.end_;
    if (x.right_open_)
        s << ")";
    else
        s << "]";
    str_ = s.str();
}

void StrPrinter::bvisit(const EmptySet &x)
{
    std::ostringstream s;
    s << "EmptySet";
    str_ = s.str();
}

#ifdef HAVE_SYMENGINE_MPFR
void StrPrinter::bvisit(const RealMPFR &x)
{
    mpfr_exp_t ex;
    char *c = mpfr_get_str(nullptr, &ex, 10, 0, x.i.get_mpfr_t(), MPFR_RNDN);
    std::ostringstream s;
    str_ = std::string(c);
    if (str_.at(0) == '-') {
        s << '-';
        str_ = str_.substr(1, str_.length() - 1);
    }
    if (ex > 6) {
        s << str_.at(0) << '.' << str_.substr(1, str_.length() - 1) << 'e'
          << (ex - 1);
    } else if (ex > 0) {
        s << str_.substr(0, (unsigned long)ex) << ".";
        s << str_.substr((unsigned long)ex, str_.length() - ex);
    } else if (ex > -5) {
        s << "0.";
        for (int i = 0; i < -ex; ++i) {
            s << '0';
        }
        s << str_;
    } else {
        s << str_.at(0) << '.' << str_.substr(1, str_.length() - 1) << 'e'
          << (ex - 1);
    }
    mpfr_free_str(c);
    str_ = s.str();
}
#endif
#ifdef HAVE_SYMENGINE_MPC
void StrPrinter::bvisit(const ComplexMPC &x)
{
    RCP<const Number> imag = x.imaginary_part();
    if (imag->is_negative()) {
        std::string str = this->apply(imag);
        str = str.substr(1, str.length() - 1);
        str_ = this->apply(x.real_part()) + " - " + str + "*I";
    } else {
        str_ = this->apply(x.real_part()) + " + " + this->apply(imag) + "*I";
    }
}
#endif
void StrPrinter::bvisit(const Add &x)
{
    std::ostringstream o;
    bool first = true;
    std::map<RCP<const Basic>, RCP<const Number>, RCPBasicKeyLessCmp> dict(
        x.dict_.begin(), x.dict_.end());

    if (neq(*(x.coef_), *zero)) {
        o << this->apply(x.coef_);
        first = false;
    }
    for (const auto &p : dict) {
        std::string t;
        if (eq(*(p.second), *one)) {
            t = this->apply(p.first);
        } else if (eq(*(p.second), *minus_one)) {
            t = "-" + parenthesizeLT(p.first, PrecedenceEnum::Mul);
        } else {
            t = parenthesizeLT(p.second, PrecedenceEnum::Mul) + "*"
                + parenthesizeLT(p.first, PrecedenceEnum::Mul);
        }

        if (not first) {
            if (t[0] == '-') {
                o << " - " << t.substr(1);
            } else {
                o << " + " << t;
            }
        } else {
            o << t;
            first = false;
        }
    }
    str_ = o.str();
}

void StrPrinter::bvisit(const Mul &x)
{
    std::ostringstream o, o2;
    bool num = false;
    unsigned den = 0;
    std::map<RCP<const Basic>, RCP<const Basic>, RCPBasicKeyLessCmp> dict(
        x.dict_.begin(), x.dict_.end());

    if (eq(*(x.coef_), *minus_one)) {
        o << "-";
    } else if (neq(*(x.coef_), *one)) {
        o << parenthesizeLT(x.coef_, PrecedenceEnum::Mul) << "*";
        num = true;
    }

    for (const auto &p : dict) {
        if ((is_a<Integer>(*p.second)
             and rcp_static_cast<const Integer>(p.second)->is_negative())
            || (is_a<Rational>(*p.second)
                and rcp_static_cast<const Rational>(p.second)->is_negative())) {
            if (eq(*(p.second), *minus_one)) {
                o2 << parenthesizeLT(p.first, PrecedenceEnum::Mul);
            } else {
                o2 << parenthesizeLE(p.first, PrecedenceEnum::Pow);
                o2 << "**";
                o2 << parenthesizeLE(neg(p.second), PrecedenceEnum::Pow);
            }
            o2 << "*";
            den++;
        } else {
            if (eq(*(p.second), *one)) {
                o << parenthesizeLT(p.first, PrecedenceEnum::Mul);
            } else {
                o << parenthesizeLE(p.first, PrecedenceEnum::Pow);
                o << "**";
                o << parenthesizeLE(p.second, PrecedenceEnum::Pow);
            }
            o << "*";
            num = true;
        }
    }

    if (not num) {
        o << "1*";
    }

    std::string s = o.str();
    s = s.substr(0, s.size() - 1);

    if (den != 0) {
        std::string s2 = o2.str();
        s2 = s2.substr(0, s2.size() - 1);
        if (den > 1) {
            str_ = s + "/(" + s2 + ")";
        } else {
            str_ = s + "/" + s2;
        }
    } else {
        str_ = s;
    }
}

void StrPrinter::bvisit(const Pow &x)
{
    std::ostringstream o;
    o << parenthesizeLE(x.get_base(), PrecedenceEnum::Pow);
    o << "**";
    o << parenthesizeLE(x.get_exp(), PrecedenceEnum::Pow);
    str_ = o.str();
}

char _print_sign(const integer_class &i)
{
    if (i < 0) {
        return '-';
    } else {
        return '+';
    }
}

// UnivariateIntPolynomial printing, tests taken from SymPy and printing ensures
// that there is compatibility
void StrPrinter::bvisit(const UnivariateIntPolynomial &x)
{
    std::ostringstream s;
    // bool variable needed to take care of cases like -5, -x, -3*x etc.
    bool first = true;
    // we iterate over the map in reverse order so that highest degree gets
    // printed first
    for (auto it = x.get_dict().rbegin(); it != x.get_dict().rend(); ++it) {
        // if exponent is 0, then print only coefficient
        if (it->first == 0) {
            if (first) {
                s << it->second;
            } else {
                s << " " << _print_sign(it->second) << " "
                  << mp_abs(it->second);
            }
            first = false;
            continue;
        }
        // if the coefficient of a term is +1 or -1
        if (mp_abs(it->second) == 1) {
            // in cases of -x, print -x
            // in cases of x**2 - x, print - x
            if (first) {
                if (it->second == -1)
                    s << "-";
                s << x.get_var()->get_name();
            } else {
                s << " " << _print_sign(it->second) << " "
                  << x.get_var()->get_name();
            }
        }
        // same logic is followed as above
        else {
            // in cases of -2*x, print -2*x
            // in cases of x**2 - 2*x, print - 2*x
            if (first) {
                s << it->second << "*" << x.get_var()->get_name();
            } else {
                s << " " << _print_sign(it->second) << " " << mp_abs(it->second)
                  << "*" << x.get_var()->get_name();
            }
        }
        // if exponent is not 1, print the exponent;
        if (it->first != 1) {
            s << "**" << it->first;
        }
        // corner cases of only first term handled successfully, switch the bool
        first = false;
    }
    if (x.get_dict().size() == 0)
        s << "0";
    str_ = s.str();
}

// UnivariatePolynomial printing, tests taken from SymPy and printing ensures
// that there is compatibility
void StrPrinter::bvisit(const UnivariatePolynomial &x)
{
    std::ostringstream s;
    // bool variable needed to take care of cases like -5, -x, -3*x etc.
    bool first = true;
    // we iterate over the map in reverse order so that highest degree gets
    // printed first
    for (auto it = x.get_dict().rbegin(); it != x.get_dict().rend(); ++it) {
        std::string t;
        // if exponent is 0, then print only coefficient
        if (it->first == 0) {
            if (first) {
                s << it->second;
            } else {
                t = parenthesizeLT(it->second.get_basic(), PrecedenceEnum::Mul);
                if (t[0] == '-') {
                    s << " - " << t.substr(1);
                } else {
                    s << " + " << t;
                }
            }
            first = false;
            continue;
        }
        // if the coefficient of a term is +1 or -1
        if (it->second == 1 or it->second == -1) {
            // in cases of -x, print -x
            // in cases of x**2 - x, print - x
            if (first) {
                if (it->second == -1)
                    s << "-";
            } else {
                s << " " << _print_sign(static_cast<const Integer &>(
                                            *it->second.get_basic())
                                            .as_mpz())
                  << " ";
            }
        }
        // same logic is followed as above
        else {
            // in cases of -2*x, print -2*x
            // in cases of x**2 - 2*x, print - 2*x
            if (first) {
                s << parenthesizeLT(it->second.get_basic(), PrecedenceEnum::Mul)
                  << "*";
            } else {
                t = parenthesizeLT(it->second.get_basic(), PrecedenceEnum::Mul);
                if (t[0] == '-') {
                    s << " - " << t.substr(1);
                } else {
                    s << " + " << t;
                }
                s << "*";
            }
        }
        s << x.get_var()->get_name();
        // if exponent is not 1, print the exponent;
        if (it->first > 1) {
            s << "**" << it->first;
        } else if (it->first < 0) {
            s << "**(" << it->first << ")";
        }
        // corner cases of only first term handled successfully, switch the bool
        first = false;
    }
    if (x.get_dict().size() == 0)
        s << "0";
    str_ = s.str();
}

void StrPrinter::bvisit(const UnivariateSeries &x)
{
    std::ostringstream o;
    o << x.get_poly() << " + O(" << x.get_var() << "**" << x.get_degree()
      << ")";
    str_ = o.str();
}

#ifdef HAVE_SYMENGINE_PIRANHA
void StrPrinter::bvisit(const URatPSeriesPiranha &x)
{
    std::ostringstream o;
    o << x.get_poly() << " + O(" << x.get_var() << "**" << x.get_degree()
      << ")";
    str_ = o.str();
}
void StrPrinter::bvisit(const UPSeriesPiranha &x)
{
    std::ostringstream o;
    o << x.get_poly() << " + O(" << x.get_var() << "**" << x.get_degree()
      << ")";
    str_ = o.str();
}
#endif

void StrPrinter::bvisit(const Log &x)
{
    str_ = "log(" + this->apply(x.get_arg()) + ")";
}

void StrPrinter::bvisit(const Constant &x)
{
    str_ = x.get_name();
}

std::string StrPrinter::apply(const vec_basic &d)
{
    std::ostringstream o;
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin()) {
            o << ", ";
        }
        o << this->apply(*p);
    }
    return o.str();
}
void StrPrinter::bvisit(const Function &x)
{
    std::ostringstream o;
    o << names_[x.get_type_code()];
    o << "(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const FunctionSymbol &x)
{
    std::ostringstream o;
    o << x.get_name();
    o << "(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const Derivative &x)
{
    std::ostringstream o;
    o << "Derivative(" << this->apply(x.get_arg());
    multiset_basic m1 = x.get_symbols();
    std::multiset<RCP<const Basic>, RCPBasicKeyLessCmp> m2(m1.begin(),
                                                           m1.end());
    for (const auto &elem : m2) {
        o << ", " << this->apply(elem);
    }
    o << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const Subs &x)
{
    std::ostringstream o, vars, point;
    for (auto p = x.get_dict().begin(); p != x.dict_.end(); p++) {
        if (p != x.get_dict().begin()) {
            vars << ", ";
            point << ", ";
        }
        vars << apply(p->first);
        point << apply(p->second);
    }
    o << "Subs(" << apply(x.arg_) << ", (" << vars.str() << "), ("
      << point.str() << "))";
    str_ = o.str();
}

void StrPrinter::bvisit(const NumberWrapper &x)
{
    str_ = x.__str__();
}

std::string StrPrinter::parenthesizeLT(const RCP<const Basic> &x,
                                       PrecedenceEnum precedenceEnum)
{
    Precedence prec;
    if (prec.getPrecedence(x) < precedenceEnum) {
        return "(" + this->apply(x) + ")";
    } else {
        return this->apply(x);
    }
}

std::string StrPrinter::parenthesizeLE(const RCP<const Basic> &x,
                                       PrecedenceEnum precedenceEnum)
{
    Precedence prec;
    if (prec.getPrecedence(x) <= precedenceEnum) {
        return "(" + this->apply(x) + ")";
    } else {
        return this->apply(x);
    }
}

std::string StrPrinter::apply(const RCP<const Basic> &b)
{
    b->accept(*this);
    return str_;
}

std::string StrPrinter::apply(const Basic &b)
{
    b.accept(*this);
    return str_;
}

std::vector<std::string> init_str_printer_names()
{
    std::vector<std::string> names;
    names.assign(TypeID_Count, "");
    names[SIN] = "sin";
    names[COS] = "cos";
    names[TAN] = "tan";
    names[COT] = "cot";
    names[CSC] = "csc";
    names[SEC] = "sec";
    names[ASIN] = "asin";
    names[ACOS] = "acos";
    names[ASEC] = "asec";
    names[ACSC] = "acsc";
    names[ATAN] = "atan";
    names[ACOT] = "acot";
    names[ATAN2] = "atan2";
    names[SINH] = "sinh";
    names[CSCH] = "csch";
    names[COSH] = "cosh";
    names[SECH] = "sech";
    names[TANH] = "tanh";
    names[COTH] = "coth";
    names[ASINH] = "asinh";
    names[ACSCH] = "acsch";
    names[ACOSH] = "acosh";
    names[ATANH] = "atanh";
    names[ACOTH] = "acoth";
    names[ASECH] = "asech";
    names[LAMBERTW] = "lambertw";
    names[ZETA] = "zeta";
    names[DIRICHLET_ETA] = "dirichlet_eta";
    names[KRONECKERDELTA] = "kroneckerdelta";
    names[LEVICIVITA] = "levicivita";
    names[ERF] = "erf";
    names[LOWERGAMMA] = "lowergamma";
    names[UPPERGAMMA] = "uppergamma";
    names[UPPERGAMMA] = "beta";
    names[LOGGAMMA] = "loggamma";
    names[POLYGAMMA] = "polygamma";
    names[GAMMA] = "gamma";
    names[ABS] = "abs";
    names[MAX] = "max";
    names[MIN] = "min";
    return names;
}

const std::vector<std::string> StrPrinter::names_ = init_str_printer_names();
}
