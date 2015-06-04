#include <algorithm>

#include "printer.h"

namespace SymEngine {

StrPrinter::StrPrinter() : BaseVisitor(this) {

}
void StrPrinter::bvisit(const Basic &x) {
    std::ostringstream s;
    s << "<" << typeName<Basic>(x) << " instance at " << (const void*)this << ">";
    str_ = s.str();
}

void StrPrinter::bvisit(const Symbol &x) {
    str_ = x.get_name();
}

void StrPrinter::bvisit(const Integer &x) {
    std::ostringstream s;
    s << x.i;
    str_ = s.str();
}

void StrPrinter::bvisit(const Rational &x) {
    std::ostringstream s;
    s << x.i;
    str_ = s.str();
}

void StrPrinter::bvisit(const Complex &x) {
    std::ostringstream s;
    if (x.real_ != 0) {
        s << x.real_;
        // Since Complex is in canonical form, imaginary_ is not 0.
        if (sgn(x.imaginary_) == 1) {
            s << " + ";
        } else {
            s << " - ";
        }
        // If imaginary_ is not 1 or -1, print the absolute value
        if (x.imaginary_ != sgn(x.imaginary_)) {
            s << abs(x.imaginary_);
            s << "*I";
        } else {
            s << "I";
        }
    } else {
        if (x.imaginary_ != sgn(x.imaginary_)) {
            s << x.imaginary_;
            s << "*I";
        } else {
            if (sgn(x.imaginary_) == 1) {
                s << "I";
            } else {
                s << "-I";
            }
        }
    }
    str_ = s.str();
}

void StrPrinter::bvisit(const RealDouble &x) {
    std::ostringstream s;
    s.precision(std::numeric_limits< double >::digits10);
    s << std::scientific << x.i;
    str_ = s.str();
}

void StrPrinter::bvisit(const Add &x) {
    std::ostringstream o;
    bool first = true;
    std::map<RCP<const Basic>, RCP<const Number>,
            RCPBasicKeyLessCmp> dict(x.dict_.begin(), x.dict_.end());

    if (neq(x.coef_, zero)) {
        o << this->apply(x.coef_);
        first = false;
    }
    for (auto &p: dict) {
        std::string t;
        if (eq(p.second, one)) {
            t = this->apply(p.first);
        } else if(eq(p.second, minus_one)) {
            t = "-" + parenthesizeLT(p.first, PrecedenceEnum::Mul);
        } else {
            t = parenthesizeLT(p.second, PrecedenceEnum::Mul) + "*" + parenthesizeLT(p.first, PrecedenceEnum::Mul);
        }

        if (!first) {
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

void StrPrinter::bvisit(const Mul &x) {
    std::ostringstream o, o2;
    bool num = false;
    unsigned den = 0;
    std::map<RCP<const Basic>, RCP<const Basic>,
            RCPBasicKeyLessCmp> dict(x.dict_.begin(), x.dict_.end());

    if (eq(x.coef_, minus_one)) {
        o << "-";
    } else if (neq(x.coef_, one)) {
        o << parenthesizeLT(x.coef_, PrecedenceEnum::Mul) << "*";
        num = true;
    }

    for (auto &p: dict) {
        if ((is_a<Integer>(*p.second) &&
             rcp_static_cast<const Integer>(p.second)->is_negative()) ||
            (is_a<Rational>(*p.second) &&
             rcp_static_cast<const Rational>(p.second)->is_negative())) {
            if(eq(p.second, minus_one)) {
                o2 << parenthesizeLT(p.first, PrecedenceEnum::Mul);
            } else {
                o2 << parenthesizeLE(p.first, PrecedenceEnum::Pow);
                o2 << "**";
                o2 << parenthesizeLE(neg(p.second), PrecedenceEnum::Pow);
            }
            o2 << "*";
            den++;
        } else {
            if(eq(p.second, one)) {
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

    if (!num) {
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

void StrPrinter::bvisit(const Pow &x) {
    std::ostringstream o;
    o << parenthesizeLE(x.base_, PrecedenceEnum::Pow);
    o << "**";
    o << parenthesizeLE(x.exp_, PrecedenceEnum::Pow);
    str_ = o.str();
}

//Polynomial printing, tests taken from SymPy and printing ensures that there is compatibility
void StrPrinter::bvisit(const Polynomial &x) {
    std::ostringstream s;
    //bool variable needed to take care of cases like -5, -x, -3*x etc.
    bool first = true;
    //we iterate over the map in reverse order so that highest degree gets printed first
    for (auto it = x.dict_.rbegin(); it != x.dict_.rend();) {
        //given a term in polynomial, if coefficient is zero, print nothing
        if (it->second == 0) {
            //except when it is the only term, say "0"
            if (it->first == 0) 
                s << "0";
            ++it;
        }
        //if the coefficient of a term is +1 or -1
        else if (abs(it->second) == 1) {
            //if exponent is 0, then print only coefficient
            //in cases of -7, it is the only term, hence we print -7
            //in cases of x - 7, the '-' is considered earlier, hence print only 7
            if (it->first == 0) {
                if (first) 
                    s << it->second;
                else 
                    s << abs(it->second);
            }
            //if exponent is 1, print x instead of x**1 
            else if (it->first == 1) {
                //in cases of -x, print -x
                //in cases of x**2 - x, print x, the '-' is considered earlier
                if (first && it->second == -1) {
                    s << "-" << x.var_->get_name();
                } else {
                    s << x.var_->get_name();
                }
            } else {
                if (first && it->second == -1) {
                    s << "-" << x.var_->get_name() << "**"  << it->first;
                } else {
                    s << x.var_->get_name() << "**"  << it->first;
                }
            }
            //if next term is going to be 0, don't print +, so that x**3 + 0 + x becomes x**3 + x
            //also consider that sign of term here itself to avoid prints like x + -1
            if ((++it != x.dict_.rend()) && (it->second != 0)) {
                if (it->second < 0) {
                    s << " - ";
                } else {
                    s << " + "; 
                }
            }
        }
        //same logic is followed as above
        else {
            if (it->first == 0) {
                if (first) 
                    s << it->second;
                else 
                    s << abs(it->second);
            } else if (it->first == 1) {
                if (first && it->second < 0) {
                    s << it->second << "*" << x.var_->get_name();
                } else {
                    s << abs(it->second) << "*" << x.var_->get_name();
                }
            } else {
                s << abs(it->second) << "*" << x.var_->get_name() << "**"  << it->first;
            }
            if ((++it != x.dict_.rend()) && (it->second != 0)) {
                if (it->second < 0) {
                    s << " - ";
                } else {
                    s << " + "; 
                }
            }
        }
        //corner cases of only first term handled successfully, switch the bool
        first = false;
    }
    str_ = s.str();
}

void StrPrinter::bvisit(const Log &x) {
    str_ = "log(" + this->apply(x.get_arg()) + ")";
}

void StrPrinter::bvisit(const Constant &x) {
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
void StrPrinter::bvisit(const Function &x) {
    std::ostringstream o;
    o << names_[x.get_type_code()];
    o << "(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const FunctionSymbol &x) {
    std::ostringstream o;
    o << x.get_name();
    o << "(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const Derivative &x) {
    std::ostringstream o;
    o << "Derivative(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::bvisit(const Subs &x) {
    std::ostringstream o, vars, point;
    for (auto p = x.dict_.begin(); p != x.dict_.end(); p++) {
        if (p != x.dict_.begin()) {
            vars << ", ";
            point << ", ";
        }
        vars << *(p->first);
        point << *(p->second);
    }
    o << "Subs(" << this->apply(x.arg_) << ", (" << vars.str() << "), (" << point.str() << "))";
    str_ = o.str();
}

std::string StrPrinter::parenthesizeLT(const RCP<const Basic> &x, PrecedenceEnum precedenceEnum) {
    Precedence prec;
    if(prec.getPrecedence(x) < precedenceEnum) {
        return "(" + this->apply(x) + ")";
    } else {
        return this->apply(x);
    }
}

std::string StrPrinter::parenthesizeLE(const RCP<const Basic> &x, PrecedenceEnum precedenceEnum) {
    Precedence prec;
    if(prec.getPrecedence(x) <= precedenceEnum) {
        return "(" + this->apply(x) + ")";
    } else {
        return this->apply(x);
    }
}

std::string StrPrinter::apply(const RCP<const Basic> &b) {
    b->accept(*this);
    return str_;
}

std::string StrPrinter::apply(const Basic &b) {
    b.accept(*this);
    return str_;
}

std::vector<std::string> init_str_printer_names() {
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
    names[COSH] = "cosh";
    names[TANH] = "tanh";
    names[COTH] = "coth";
    names[ASINH] = "asinh";
    names[ACOSH] = "acosh";
    names[ATANH] = "atanh";
    names[ACOTH] = "acoth";
    names[ASECH] = "asech";
    names[LAMBERTW] = "lambertw";
    names[ZETA] = "zeta";
    names[DIRICHLET_ETA] = "dirichlet_eta";
    names[KRONECKERDELTA] = "kroneckerdelta";
    names[LEVICIVITA] = "levicivita";
    names[LOWERGAMMA] = "lowergamma";
    names[UPPERGAMMA] = "uppergamma";
    names[ABS] = "abs";
    return names;
}

const std::vector<std::string> StrPrinter::names_ = init_str_printer_names();

}