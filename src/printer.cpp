#include <algorithm>

#include "printer.h"

namespace CSymPy {

StrPrinter::StrPrinter(bool ordered) : Printer(this), order_(ordered) {

}
void StrPrinter::print(const Basic &x) {
    std::cout << "Basic" <<std::endl;
}

void StrPrinter::print(const Symbol &x) {
    str_ = x.get_name();
}

void StrPrinter::print(const Integer &x) {
    std::ostringstream s;
    s << x.i;
    str_ = s.str();
}

void StrPrinter::print(const Rational &x) {
    std::ostringstream s;
    s << x.i;
    str_ = s.str();
}

void StrPrinter::print(const Complex &x) {
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

template<class T>
void getKeysValues(vec_basic &keys, vec_basic &values, T &dict, bool order) {

    if(order) {
        for(auto &p: dict) {
            keys.push_back(p.first);
        }
        std::sort(keys.begin(), keys.end(), RCPBasicKeyLessCmp());
        for(auto &p: keys) {
            values.push_back(dict.find(p)->second);
        }
    } else {
        for(auto &p: dict) {
            keys.push_back(p.first);
            values.push_back(p.second);
        }
    }
}

void StrPrinter::print(const Add &x) {
    std::ostringstream o;
    bool first = true;
    vec_basic keys;
    vec_basic values;
    getKeysValues(keys, values, x.dict_, order_);

    if (neq(x.coef_, zero)) {
        o << this->apply(x.coef_);
        first = false;
    }
    auto k = keys.begin();
    auto v = values.begin();
    for (; k != keys.end(); k++, v++) {
        std::string t;
        if (eq(*v, one)) {
            t = this->apply(*k);
        } else if(eq(*v, minus_one)) {
            t = "-" + parenthesizeLT(*k, PrecedenceEnum::Mul);
        } else {
            t = parenthesizeLT(*v, PrecedenceEnum::Mul) + "*" + parenthesizeLT(*k, PrecedenceEnum::Mul);
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

void StrPrinter::print(const Mul &x) {
    std::ostringstream o, o2;
    bool num = false;
    unsigned den = 0;
    vec_basic keys;
    vec_basic values;
    getKeysValues(keys, values, x.dict_, order_);

    if (eq(x.coef_, minus_one)) {
        o << "-";
    } else if (neq(x.coef_, one)) {
        o << parenthesizeLT(x.coef_, PrecedenceEnum::Mul) << "*";
        num = true;
    }

    auto first = keys.begin();
    auto second = values.begin();
    for (; first != keys.end(); first++, second++) {
        if ((is_a<Integer>(*(*second)) &&
             rcp_static_cast<const Integer>(*second)->is_negative()) ||
            (is_a<Rational>(*(*second)) &&
             rcp_static_cast<const Rational>(*second)->is_negative())) {
            if(eq(*second, minus_one)) {
                o2 << parenthesizeLT(*first, PrecedenceEnum::Mul);
            } else {
                o2 << parenthesizeLE(*first, PrecedenceEnum::Pow);
                o2 << "**";
                o2 << parenthesizeLE(neg(*second), PrecedenceEnum::Pow);
            }
            o2 << "*";
            den++;
        } else {
            if(eq(*second, one)) {
                o << parenthesizeLT(*first, PrecedenceEnum::Mul);
            } else {
                o << parenthesizeLE(*first, PrecedenceEnum::Pow);
                o << "**";
                o << parenthesizeLE(*second, PrecedenceEnum::Pow);
            }
            o << "*";
            num = true;
        }
    }

    if (!num) {
        o << "1*";
    }

    std::string s = o.str();
    s = s.substr(0, s.size()-1);

    if (den != 0) {
        std::string s2 = o2.str();
        s2 = s2.substr(0, s2.size()-1);
        if (den > 1) {
            str_ = s + "/(" + s2 + ")";
        } else {
            str_ = s + "/" + s2;
        }
    } else {
        str_ = s;
    }
}

void StrPrinter::print(const Pow &x) {
    std::ostringstream o;
    o << parenthesizeLE(x.base_, PrecedenceEnum::Pow);
    o << "**";
    o << parenthesizeLE(x.exp_, PrecedenceEnum::Pow);
    str_ = o.str();
}

void StrPrinter::print(const Log &x) {
    str_ = "log(" + this->apply(x.get_arg()) + ")";
}

void StrPrinter::print(const Constant &x) {
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
void StrPrinter::print(const Function &x) {
    std::ostringstream o;
    o << names_[x.get_type_code()];
    o << "(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::print(const FunctionSymbol &x) {
    std::ostringstream o;
    o << x.get_name();
    o << "(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::print(const Derivative &x) {
    std::ostringstream o;
    o << "Derivative(";
    vec_basic vec = x.get_args();
    o << this->apply(vec) << ")";
    str_ = o.str();
}

void StrPrinter::print(const Subs &x) {
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
    (*b).accept(*this);
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