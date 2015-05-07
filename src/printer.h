#ifndef SYMENGINE_PRINTER_H
#define SYMENGINE_PRINTER_H

#include "visitor.h"

namespace SymEngine {

enum class PrecedenceEnum {
    Add, Mul, Pow, Atom
};

class Precedence : public BaseVisitor<Precedence> {
public:
    PrecedenceEnum precedence;
    Precedence() : BaseVisitor(this) {

    }
    void bvisit(const Add &x) {
        precedence = PrecedenceEnum::Add;
    }

    void bvisit(const Mul &x) {
        precedence = PrecedenceEnum::Mul;
    }

    void bvisit(const Pow &x) {
        precedence = PrecedenceEnum::Pow;
    }

    void bvisit(const Rational &x) {
        precedence = PrecedenceEnum::Add;
    }

    void bvisit(const Complex &x) {
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

    void bvisit(const Integer &x) {
        if (x.is_negative()) {
            precedence = PrecedenceEnum::Mul;
        } else {
            precedence = PrecedenceEnum::Atom;
        }
    }

    void bvisit(const Basic &x) {
        precedence = PrecedenceEnum::Atom;
    }

    PrecedenceEnum getPrecedence(const RCP<const Basic> &x) {
        (*x).accept(*this);
        return precedence;
    }
};

class StrPrinter : public BaseVisitor<StrPrinter> {
private:
    std::string str_;
public:
    StrPrinter();
    static const std::vector<std::string> names_;
    void bvisit(const Basic &x);
    void bvisit(const Symbol &x);
    void bvisit(const Integer &x);
    void bvisit(const Rational &x);
    void bvisit(const Complex &x);
    void bvisit(const Add &x);
    void bvisit(const Mul &x);
    void bvisit(const Pow &x);
    void bvisit(const Log &x);
    void bvisit(const Constant &x);
    void bvisit(const Function &x);
    void bvisit(const FunctionSymbol &x);
    void bvisit(const Derivative &x);
    void bvisit(const Subs &x);

    std::string parenthesizeLT(const RCP<const Basic> &x, PrecedenceEnum precedenceEnum);
    std::string parenthesizeLE(const RCP<const Basic> &x, PrecedenceEnum precedenceEnum);

    std::string apply(const RCP<const Basic> &b);
    std::string apply(const vec_basic &v);
    std::string apply(const Basic &b);
};

}

#endif //SYMENGINE_PRINTER_H
