#include <limits>
#include <symengine/printer.h>

namespace SymEngine
{

//! Less operator `(<)` using cmp:
struct PrinterBasicCmp {
    //! true if `x < y`, false otherwise
    bool operator()(const RCP<const Basic> &x, const RCP<const Basic> &y) const
    {
        if (x->__eq__(*y))
            return false;
        return x->__cmp__(*y) == -1;
    }
};

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

void StrPrinter::bvisit(const Infty &x)
{
    std::ostringstream s;
    if (x.is_negative_infinity())
        s << "-oo";
    else if (x.is_positive_infinity())
        s << "oo";
    else
        s << "zoo";
    str_ = s.str();
}

void StrPrinter::bvisit(const NaN &x)
{
    std::ostringstream s;
    s << "nan";
    str_ = s.str();
}

void StrPrinter::bvisit(const Integer &x)
{
    std::ostringstream s;
    s << x.as_integer_class();
    str_ = s.str();
}

void StrPrinter::bvisit(const Rational &x)
{
    std::ostringstream s;
    s << x.as_rational_class();
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

std::string print_double(double d)
{
    std::ostringstream s;
    s.precision(std::numeric_limits<double>::digits10);
    s << d;
    auto str_ = s.str();
    if (str_.find(".") == std::string::npos
        and str_.find("e") == std::string::npos) {
        if (std::numeric_limits<double>::digits10 - str_.size() > 0) {
            str_ += ".0";
        } else {
            str_ += ".";
        }
    }
    return str_;
}

void StrPrinter::bvisit(const RealDouble &x)
{
    str_ = print_double(x.i);
}

void StrPrinter::bvisit(const ComplexDouble &x)
{
    str_ = print_double(x.i.real());
    if (x.i.imag() < 0) {
        str_ += " - " + print_double(-x.i.imag()) + "*I";
    } else {
        str_ += " + " + print_double(x.i.imag()) + "*I";
    }
}

void StrPrinter::bvisit(const Equality &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " == " << apply(x.get_arg2());
    str_ = s.str();
}

void StrPrinter::bvisit(const Unequality &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " != " << apply(x.get_arg2());
    str_ = s.str();
}

void StrPrinter::bvisit(const LessThan &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " <= " << apply(x.get_arg2());
    str_ = s.str();
}

void StrPrinter::bvisit(const StrictLessThan &x)
{
    std::ostringstream s;
    s << apply(x.get_arg1()) << " < " << apply(x.get_arg2());
    str_ = s.str();
}

void StrPrinter::bvisit(const Interval &x)
{
    std::ostringstream s;
    if (x.get_left_open())
        s << "(";
    else
        s << "[";
    s << *x.get_start() << ", " << *x.get_end();
    if (x.get_right_open())
        s << ")";
    else
        s << "]";
    str_ = s.str();
}

void StrPrinter::bvisit(const BooleanAtom &x)
{
    if (x.get_val()) {
        str_ = "True";
    } else {
        str_ = "False";
    }
}

void StrPrinter::bvisit(const And &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    s << "And(";
    s << apply(*container.begin());
    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << ", " << apply(*it);
    }
    s << ")";
    str_ = s.str();
}

void StrPrinter::bvisit(const Or &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    s << "Or(";
    s << apply(*container.begin());
    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << ", " << apply(*it);
    }
    s << ")";
    str_ = s.str();
}

void StrPrinter::bvisit(const Xor &x)
{
    std::ostringstream s;
    auto container = x.get_container();
    s << "Xor(";
    s << apply(*container.begin());
    for (auto it = ++(container.begin()); it != container.end(); ++it) {
        s << ", " << apply(*it);
    }
    s << ")";
    str_ = s.str();
}

void StrPrinter::bvisit(const Not &x)
{
    std::ostringstream s;
    s << "Not(" << *x.get_arg() << ")";
    str_ = s.str();
}

void StrPrinter::bvisit(const Contains &x)
{
    std::ostringstream s;
    s << "Contains(" << apply(x.get_expr()) << ", " << apply(x.get_set())
      << ")";
    str_ = s.str();
}

void StrPrinter::bvisit(const Piecewise &x)
{
    std::ostringstream s;
    auto vec = x.get_vec();
    auto it = vec.begin();
    s << "Piecewise(";
    while (true) {
        s << "(";
        s << apply((*it).first);
        s << ", ";
        s << apply((*it).second);
        s << ")";
        ++it;
        if (it != vec.end()) {
            s << ", ";
        } else {
            break;
        }
    }
    s << ")";
    str_ = s.str();
}

void StrPrinter::bvisit(const EmptySet &x)
{
    str_ = "EmptySet";
}

void StrPrinter::bvisit(const Union &x)
{
    std::ostringstream s;
    s << apply(*x.get_container().begin());
    for (auto it = ++(x.get_container().begin()); it != x.get_container().end();
         ++it) {
        s << " U " << apply(*it);
    }
    str_ = s.str();
}

void StrPrinter::bvisit(const Complement &x)
{
    std::ostringstream s;
    s << apply(*x.get_universe());
    s << " \\ " << apply(*x.get_container());
    str_ = s.str();
}

void StrPrinter::bvisit(const ImageSet &x)
{
    std::ostringstream s;
    s << "{" << apply(*x.get_expr()) << " | ";
    s << apply(*x.get_symbol());
    s << " in " << apply(*x.get_baseset()) << "}";
    str_ = s.str();
}

void StrPrinter::bvisit(const UniversalSet &x)
{
    str_ = "UniversalSet";
}

void StrPrinter::bvisit(const FiniteSet &x)
{
    std::ostringstream s;
    s << x.get_container();
    str_ = s.str();
}

void StrPrinter::bvisit(const ConditionSet &x)
{
    std::ostringstream s;
    s << "{" << apply(*x.get_symbol());
    s << " | " << apply(x.get_condition()) << "}";
    str_ = s.str();
}

#ifdef HAVE_SYMENGINE_MPFR
void StrPrinter::bvisit(const RealMPFR &x)
{
    mpfr_exp_t ex;
    // mpmath.libmp.libmpf.prec_to_dps
    long digits
        = std::max(long(1), std::lround(static_cast<double>(x.i.get_prec())
                                        / 3.3219280948873626)
                                - 1);
    char *c
        = mpfr_get_str(nullptr, &ex, 10, digits, x.i.get_mpfr_t(), MPFR_RNDN);
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
    std::map<RCP<const Basic>, RCP<const Number>, PrinterBasicCmp> dict(
        x.get_dict().begin(), x.get_dict().end());

    if (neq(*(x.get_coef()), *zero)) {
        o << this->apply(x.get_coef());
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

void StrPrinter::_print_pow(std::ostringstream &o, const RCP<const Basic> &a,
                            const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << "exp(" << apply(b) << ")";
    } else if (eq(*b, *rational(1, 2))) {
        o << "sqrt(" << apply(a) << ")";
    } else {
        o << parenthesizeLE(a, PrecedenceEnum::Pow);
        o << "**";
        o << parenthesizeLE(b, PrecedenceEnum::Pow);
    }
}

void StrPrinter::bvisit(const Mul &x)
{
    std::ostringstream o, o2;
    bool num = false;
    unsigned den = 0;

    if (eq(*(x.get_coef()), *minus_one)) {
        o << "-";
    } else if (neq(*(x.get_coef()), *one)) {
        o << parenthesizeLT(x.get_coef(), PrecedenceEnum::Mul) << "*";
        num = true;
    }

    for (const auto &p : x.get_dict()) {
        if ((is_a<Integer>(*p.second) or is_a<Rational>(*p.second))
            and down_cast<const Number &>(*p.second).is_negative()
            and neq(*(p.first), *E)) {
            if (eq(*(p.second), *minus_one)) {
                o2 << parenthesizeLT(p.first, PrecedenceEnum::Mul);
            } else {
                _print_pow(o2, p.first, neg(p.second));
            }
            o2 << "*";
            den++;
        } else {
            if (eq(*(p.second), *one)) {
                o << parenthesizeLT(p.first, PrecedenceEnum::Mul);
            } else {
                _print_pow(o, p.first, p.second);
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
    _print_pow(o, x.get_base(), x.get_exp());
    str_ = o.str();
}

template <typename T>
char _print_sign(const T &i)
{
    if (i < 0) {
        return '-';
    } else {
        return '+';
    }
}

void StrPrinter::bvisit(const GaloisField &x)
{
    std::ostringstream s;
    // bool variable needed to take care of cases like -5, -x, -3*x etc.
    bool first = true;
    // we iterate over the map in reverse order so that highest degree gets
    // printed first
    auto dict = x.get_dict();
    if (x.get_dict().size() == 0)
        s << "0";
    else {
        for (auto it = dict.size(); it-- != 0;) {
            if (dict[it] == 0)
                continue;
            // if exponent is 0, then print only coefficient
            if (it == 0) {
                if (first) {
                    s << dict[it];
                } else {
                    s << " " << _print_sign(dict[it]) << " "
                      << mp_abs(dict[it]);
                }
                first = false;
                break;
            }
            // if the coefficient of a term is +1 or -1
            if (mp_abs(dict[it]) == 1) {
                // in cases of -x, print -x
                // in cases of x**2 - x, print - x
                if (first) {
                    if (dict[it] == -1)
                        s << "-";
                    s << detail::poly_print(x.get_var());
                } else {
                    s << " " << _print_sign(dict[it]) << " "
                      << detail::poly_print(x.get_var());
                }
            }
            // same logic is followed as above
            else {
                // in cases of -2*x, print -2*x
                // in cases of x**2 - 2*x, print - 2*x
                if (first) {
                    s << dict[it] << "*" << detail::poly_print(x.get_var());
                } else {
                    s << " " << _print_sign(dict[it]) << " " << mp_abs(dict[it])
                      << "*" << detail::poly_print(x.get_var());
                }
            }
            // if exponent is not 1, print the exponent;
            if (it != 1) {
                s << "**" << it;
            }
            // corner cases of only first term handled successfully, switch the
            // bool
            first = false;
        }
    }
    str_ = s.str();
}

// Printing of Integer and Rational Polynomials, tests taken
// from SymPy and printing ensures that there is compatibility
template <typename P>
std::string upoly_print(const P &x)
{
    std::ostringstream s;
    // bool variable needed to take care of cases like -5, -x, -3*x etc.
    bool first = true;
    // we iterate over the map in reverse order so that highest degree gets
    // printed first
    for (auto it = x.obegin(); it != x.oend(); ++it) {
        auto m = it->second;
        // if exponent is 0, then print only coefficient
        if (it->first == 0) {
            if (first) {
                s << m;
            } else {
                s << " " << _print_sign(m) << " " << mp_abs(m);
            }
            first = false;
            continue;
        }
        // if the coefficient of a term is +1 or -1
        if (mp_abs(m) == 1) {
            // in cases of -x, print -x
            // in cases of x**2 - x, print - x
            if (first) {
                if (m == -1)
                    s << "-";
                s << detail::poly_print(x.get_var());
            } else {
                s << " " << _print_sign(m) << " "
                  << detail::poly_print(x.get_var());
            }
        }
        // same logic is followed as above
        else {
            // in cases of -2*x, print -2*x
            // in cases of x**2 - 2*x, print - 2*x
            if (first) {
                s << m << "*" << detail::poly_print(x.get_var());
            } else {
                s << " " << _print_sign(m) << " " << mp_abs(m) << "*"
                  << detail::poly_print(x.get_var());
            }
        }
        // if exponent is not 1, print the exponent;
        if (it->first != 1) {
            s << "**" << it->first;
        }
        // corner cases of only first term handled successfully, switch the bool
        first = false;
    }
    if (x.size() == 0)
        s << "0";
    return s.str();
}

void StrPrinter::bvisit(const UIntPoly &x)
{
    str_ = upoly_print<UIntPoly>(x);
}

void StrPrinter::bvisit(const URatPoly &x)
{
    str_ = upoly_print<URatPoly>(x);
}

#ifdef HAVE_SYMENGINE_FLINT
void StrPrinter::bvisit(const UIntPolyFlint &x)
{
    str_ = upoly_print<UIntPolyFlint>(x);
}
void StrPrinter::bvisit(const URatPolyFlint &x)
{
    str_ = upoly_print<URatPolyFlint>(x);
}
#endif

#ifdef HAVE_SYMENGINE_PIRANHA
void StrPrinter::bvisit(const UIntPolyPiranha &x)
{
    str_ = upoly_print<UIntPolyPiranha>(x);
}
void StrPrinter::bvisit(const URatPolyPiranha &x)
{
    str_ = upoly_print<URatPolyPiranha>(x);
}
#endif

// UExprPoly printing, tests taken from SymPy and printing ensures
// that there is compatibility
void StrPrinter::bvisit(const UExprPoly &x)
{
    std::ostringstream s;
    if (x.get_dict().size() == 0)
        s << "0";
    else
        s << x.get_poly().__str__(detail::poly_print(x.get_var()));
    str_ = s.str();
}

void StrPrinter::bvisit(const UnivariateSeries &x)
{
    std::ostringstream o;
    o << x.get_poly().__str__(x.get_var()) << " + O(" << x.get_var() << "**"
      << x.get_degree() << ")";
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
    auto m1 = x.get_symbols();
    for (const auto &elem : m1) {
        o << ", " << this->apply(elem);
    }
    o << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const Subs &x)
{
    std::ostringstream o, vars, point;
    for (auto p = x.get_dict().begin(); p != x.get_dict().end(); p++) {
        if (p != x.get_dict().begin()) {
            vars << ", ";
            point << ", ";
        }
        vars << apply(p->first);
        point << apply(p->second);
    }
    o << "Subs(" << apply(x.get_arg()) << ", (" << vars.str() << "), ("
      << point.str() << "))";
    str_ = o.str();
}

void StrPrinter::bvisit(const NumberWrapper &x)
{
    str_ = x.__str__();
}

void StrPrinter::bvisit(const MIntPoly &x)
{
    std::ostringstream s;
    bool first = true; // is this the first term being printed out?
    // To change the ordering in which the terms will print out, change
    // vec_uint_compare in dict.h
    std::vector<vec_uint> v = sorted_keys(x.get_poly().dict_);

    for (vec_uint exps : v) {
        integer_class c = x.get_poly().dict_.find(exps)->second;
        if (!first) {
            s << " " << _print_sign(c) << " ";
        } else if (c < 0) {
            s << "-";
        }

        unsigned int i = 0;
        std::ostringstream expr;
        bool first_var = true;
        for (auto it : x.get_vars()) {
            if (exps[i] != 0) {
                if (!first_var) {
                    expr << "*";
                }
                expr << it->__str__();
                if (exps[i] > 1)
                    expr << "**" << exps[i];
                first_var = false;
            }
            i++;
        }
        if (mp_abs(c) != 1) {
            s << mp_abs(c);
            if (!expr.str().empty()) {
                s << "*";
            }
        } else if (expr.str().empty()) {
            s << "1";
        }
        s << expr.str();
        first = false;
    }

    if (s.str().empty())
        s << "0";
    str_ = s.str();
}

void StrPrinter::bvisit(const MExprPoly &x)
{
    std::ostringstream s;
    bool first = true; // is this the first term being printed out?
    // To change the ordering in which the terms will print out, change
    // vec_uint_compare in dict.h
    std::vector<vec_int> v = sorted_keys(x.get_poly().dict_);

    for (vec_int exps : v) {
        Expression c = x.get_poly().dict_.find(exps)->second;
        std::string t = parenthesizeLT(c.get_basic(), PrecedenceEnum::Mul);
        if ('-' == t[0] && !first) {
            s << " - ";
            t = t.substr(1);
        } else if (!first) {
            s << " + ";
        }
        unsigned int i = 0;
        std::ostringstream expr;
        bool first_var = true;
        for (auto it : x.get_vars()) {
            if (exps[i] != 0) {
                if (!first_var) {
                    expr << "*";
                }
                expr << it->__str__();
                if (exps[i] > 1 or exps[i] < 0)
                    expr << "**" << exps[i];
                first_var = false;
            }
            i++;
        }
        if (c != 1 && c != -1) {
            s << t;
            if (!expr.str().empty()) {
                s << "*";
            }
        } else if (expr.str().empty()) {
            s << "1";
        }
        s << expr.str();
        first = false;
    }

    if (s.str().empty())
        s << "0";
    str_ = s.str();
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
    names[FLOOR] = "floor";
    names[CEILING] = "ceiling";
    names[ERF] = "erf";
    names[ERFC] = "erfc";
    names[LOWERGAMMA] = "lowergamma";
    names[UPPERGAMMA] = "uppergamma";
    names[UPPERGAMMA] = "beta";
    names[LOGGAMMA] = "loggamma";
    names[POLYGAMMA] = "polygamma";
    names[GAMMA] = "gamma";
    names[ABS] = "abs";
    names[MAX] = "max";
    names[MIN] = "min";
    names[SIGN] = "sign";
    names[CONJUGATE] = "conjugate";
    return names;
}

const std::vector<std::string> StrPrinter::names_ = init_str_printer_names();

void JuliaStrPrinter::_print_pow(std::ostringstream &o,
                                 const RCP<const Basic> &a,
                                 const RCP<const Basic> &b)
{
    if (eq(*a, *E)) {
        o << "exp(" << apply(b) << ")";
    } else if (eq(*b, *rational(1, 2))) {
        o << "sqrt(" << apply(a) << ")";
    } else {
        o << parenthesizeLE(a, PrecedenceEnum::Pow);
        o << "^";
        o << parenthesizeLE(b, PrecedenceEnum::Pow);
    }
}

void JuliaStrPrinter::bvisit(const Constant &x)
{
    if (eq(x, *E)) {
        str_ = "exp(1)";
    } else {
        str_ = x.get_name();
        std::transform(str_.begin(), str_.end(), str_.begin(), ::tolower);
    }
}

void JuliaStrPrinter::bvisit(const NaN &x)
{
    std::ostringstream s;
    s << "NaN";
    str_ = s.str();
}

void JuliaStrPrinter::bvisit(const Infty &x)
{
    std::ostringstream s;
    if (x.is_negative_infinity())
        s << "-Inf";
    else if (x.is_positive_infinity())
        s << "Inf";
    else
        s << "zoo";
    str_ = s.str();
}
}
