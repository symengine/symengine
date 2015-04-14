#ifndef CSYMPY_PRINTER_H
#define CSYMPY_PRINTER_H

#include "visitor.h"

namespace CSymPy {

enum class PrecedenceEnum {
    Add, Mul, Pow, Atom
};

class Precedence : public Printer<Precedence> {
public:
    PrecedenceEnum precedence;
    Precedence() : Printer(this) {

    }
    void print(const Add &x) {
        precedence = PrecedenceEnum::Add;
    }

    void print(const Mul &x) {
        precedence = PrecedenceEnum::Mul;
    }

    void print(const Pow &x) {
        precedence = PrecedenceEnum::Pow;
    }

    void print(const Rational &x) {
        precedence = PrecedenceEnum::Add;
    }

    void print(const Complex &x) {
        if(x.is_re_zero()) {
            if(x.imaginary_ == 1) {
                precedence = PrecedenceEnum::Atom;
            } else {
                precedence = PrecedenceEnum::Mul;
            }
        } else {
            precedence = PrecedenceEnum::Add;
        }
    }

    void print(const Integer &x) {
        if (x.is_negative()) {
            precedence = PrecedenceEnum::Mul;
        } else {
            precedence = PrecedenceEnum::Atom;
        }
    }

    void print(const Basic &x) {
        precedence = PrecedenceEnum::Atom;
    }

    PrecedenceEnum getPrecedence(const RCP<const Basic> &x) {
        (*x).accept(*this);
        return precedence;
    }
};

class StrPrinter : public Printer<StrPrinter> {
private:
    std::string str_;
    bool order_;
public:
    StrPrinter(bool ordered = true);
    static const std::vector<std::string> names_;
    void print(const Basic &x);
    void print(const Symbol &x);
    void print(const Integer &x);
    void print(const Rational &x);
    void print(const Complex &x);
    void print(const Add &x);
    void print(const Mul &x);
    void print(const Pow &x);
    void print(const Log &x);
    void print(const Constant &x);
    void print(const Function &x);
    void print(const FunctionSymbol &x);
    void print(const Derivative &x);
    void print(const Subs &x);

    std::string parenthesizeLT(const RCP<const Basic> &x, PrecedenceEnum precedenceEnum);
    std::string parenthesizeLE(const RCP<const Basic> &x, PrecedenceEnum precedenceEnum);

    std::string apply(const RCP<const Basic> &b);
    std::string apply(const vec_basic &v);
    std::string apply(const Basic &b);
};

}

#endif //CSYMPY_PRINTER_H
