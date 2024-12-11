#include "integral.h"
#include <symengine/basic.h>
#include <symengine/visitor.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>
#include <symengine/functions.h>
#include <symengine/number.h>
#include <symengine/constants.h>
#include <symengine/printers/strprinter.h>

namespace SymEngine {



// Helper function to check if an expression depends on a symbol
bool IntegralVisitor::depends_on(const RCP<const Basic> &expr, const RCP<const Symbol> &sym) {
    if (is_a<Symbol>(*expr)) {
        return expr->__eq__(*sym);
    } else if (is_a<Add>(*expr) || is_a<Mul>(*expr)) {
        for (const auto &arg : expr->get_args()) {
            if (depends_on(arg, sym)) {
                return true;
            }
        }
    } else if (is_a<Pow>(*expr)) {
        const auto &pow = rcp_static_cast<const Pow>(expr);
        return depends_on(pow->get_base(), sym) || depends_on(pow->get_exp(), sym);
    }
    return false;
}

// Integer: ∫n dx = n * x
void IntegralVisitor::bvisit(const Integer &x) {
    result_ = mul(x.rcp_from_this(), var_);
}

// Rational: ∫r dx = r * x
void IntegralVisitor::bvisit(const Rational &x) {
    result_ = mul(x.rcp_from_this(), var_);
}

// Symbol: ∫x dx = x^2 / 2
void IntegralVisitor::bvisit(const Symbol &x) {
    if (x.__eq__(*var_)) {
        result_ = div(pow(var_, integer(2)), integer(2));
    } else {
        result_ = mul(x.rcp_from_this(), var_);
    }
}

// Add: ∫(a + b) dx = ∫a dx + ∫b dx
void IntegralVisitor::bvisit(const Add &x) {
    RCP<const Basic> acc = zero;
    for (const auto &arg : x.get_args()) {
        IntegralVisitor v;
        v.set_var(var_);
        arg->accept(v);
        acc = add(acc, v.result_);
    }
    result_ = acc;
}

// Mul: ∫(c * f(x)) dx = c * ∫f(x) dx if c is constant
void IntegralVisitor::bvisit(const Mul &x) {
    RCP<const Basic> constant = one;
    RCP<const Basic> variable_part = one;

    for (const auto &arg : x.get_args()) {
        if (is_a_Number(*arg) || !depends_on(arg, var_)) {
            constant = mul(constant, arg);
        } else {
            variable_part = mul(variable_part, arg);
        }
    }

    IntegralVisitor v;
    v.set_var(var_);
    variable_part->accept(v);
    result_ = mul(constant, v.result_);
}

// Pow: ∫x^n dx = x^(n+1)/(n+1) if n != -1
void IntegralVisitor::bvisit(const Pow &x) {
    if ( is_a<Symbol>(*x.get_base()) && x.get_base()->__eq__(*var_)) {
        RCP<const Number> exp = rcp_static_cast<const Number>(x.get_exp());
        if (!exp->is_minus_one()) {
            result_ = div(pow(var_, add(exp, one)), add(exp, one));
        } else {
            result_ = log(var_);
        }
    } else {
        throw std::runtime_error("Integration of this power is not implemented.");
    }
}

// Sin: ∫sin(x) dx = -cos(x)
void IntegralVisitor::bvisit(const Sin &x) {
    if (x.get_arg()->__eq__(*var_)) {
        result_ = mul(minus_one, cos(var_));
    } else {
        throw std::runtime_error("Integration of sin(f(x)) is not implemented.");
    }
}

// Cos: ∫cos(x) dx = sin(x)
void IntegralVisitor::bvisit(const Cos &x) {
    if (x.get_arg()->__eq__(*var_)) {
        result_ = sin(var_);
    } else {
        throw std::runtime_error("Integration of cos(f(x)) is not implemented.");
    }
}

// Constant: ∫c dx = c * x
void IntegralVisitor::bvisit(const Constant &x) {
    result_ = mul(x.rcp_from_this(), var_);
}

// Fallback for unimplemented types
void IntegralVisitor::bvisit(const Basic &) {
    throw std::runtime_error("Integration not implemented for this type.");
}

// Integration entry point
RCP<const Basic> integrate(const RCP<const Basic> &expr, const RCP<const Symbol> &var) {
    IntegralVisitor v;
    v.set_var(var);
    expr->accept(v);
    return v.result_;
}

} // namespace SymEngine
