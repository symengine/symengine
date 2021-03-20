#include <symengine/test_visitors.h>

namespace SymEngine
{
void ZeroVisitor::bvisit(const Number &x)
{
    if (bool(x.is_zero())) {
        is_zero_ = tribool::tritrue;
    } else {
        is_zero_ = tribool::trifalse;
    }
}

tribool ZeroVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_zero_;
}

tribool is_zero(const Basic &b)
{
    ZeroVisitor visitor;
    return visitor.apply(b);
}

void PositiveVisitor::bvisit(const Number &x)
{
    if (is_a_Complex(x)) {
        is_positive_ = tribool::trifalse;
    } else if (bool(x.is_positive())) {
        is_positive_ = tribool::tritrue;
    } else {
        is_positive_ = tribool::trifalse;
    }
}

tribool PositiveVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_positive_;
}

tribool is_positive(const Basic &b)
{
    PositiveVisitor visitor;
    return visitor.apply(b);
}

void NonPositiveVisitor::bvisit(const Number &x)
{
    if (is_a_Complex(x)) {
        is_nonpositive_ = tribool::trifalse;
    } else if (bool(x.is_positive())) {
        is_nonpositive_ = tribool::trifalse;
    } else {
        is_nonpositive_ = tribool::tritrue;
    }
}

tribool NonPositiveVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_nonpositive_;
}

tribool is_nonpositive(const Basic &b)
{
    NonPositiveVisitor visitor;
    return visitor.apply(b);
}

void NegativeVisitor::bvisit(const Number &x)
{
    if (is_a_Complex(x)) {
        is_negative_ = tribool::trifalse;
    } else if (bool(x.is_negative())) {
        is_negative_ = tribool::tritrue;
    } else {
        is_negative_ = tribool::trifalse;
    }
}

tribool NegativeVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_negative_;
}

tribool is_negative(const Basic &b)
{
    NegativeVisitor visitor;
    return visitor.apply(b);
}

void NonNegativeVisitor::bvisit(const Number &x)
{
    if (is_a_Complex(x)) {
        is_nonnegative_ = tribool::trifalse;
    } else if (bool(x.is_negative())) {
        is_nonnegative_ = tribool::trifalse;
    } else {
        is_nonnegative_ = tribool::tritrue;
    }
}

tribool NonNegativeVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_nonnegative_;
}

tribool is_nonnegative(const Basic &b)
{
    NonNegativeVisitor visitor;
    return visitor.apply(b);
}

void RealVisitor::bvisit(const Number &x)
{
    if (is_a_Complex(x) or is_a<Infty>(x) or is_a<NaN>(x)) {
        is_real_ = tribool::trifalse;
    } else {
        is_real_ = tribool::tritrue;
    }
}

void RealVisitor::bvisit(const Constant &x)
{
    if (eq(x, *pi) or eq(x, *E) or eq(x, *EulerGamma) or eq(x, *Catalan)
        or eq(x, *GoldenRatio)) {
        is_real_ = tribool::tritrue;
    } else {
        is_real_ = tribool::indeterminate;
    }
}

void RealVisitor::bvisit(const Add &x)
{
    tribool b = tribool::tritrue;
    for (const auto &arg : x.get_args()) {
        arg->accept(*this);
        b = andwk_tribool(b, is_real_);
        if (is_indeterminate(b))
            return;
    }
}

tribool RealVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_real_;
}

tribool is_real(const Basic &b)
{
    RealVisitor visitor;
    return visitor.apply(b);
}

void PolynomialVisitor::bvisit(const Basic &x)
{
    auto old_allowed = variables_allowed_;
    variables_allowed_ = false;
    for (const auto &p : x.get_args()) {
        p->accept(*this);
        if (!is_polynomial_) {
            variables_allowed_ = old_allowed;
            return;
        }
    }
    variables_allowed_ = old_allowed;
}

void PolynomialVisitor::bvisit(const Add &x)
{
    for (const auto &arg : x.get_args()) {
        arg->accept(*this);
        if (!is_polynomial_)
            return;
    }
}

void PolynomialVisitor::bvisit(const Mul &x)
{
    for (const auto &p : x.get_dict()) {
        this->check_power(*p.first, *p.second);
        if (!is_polynomial_)
            return;
    }
}

void PolynomialVisitor::check_power(const Basic &base, const Basic &exp)
{
    if (variables_allowed_) {
        variables_allowed_ = false;
        exp.accept(*this);
        if (!is_polynomial_) {
            variables_allowed_ = true;
            return;
        }
        base.accept(*this);
        variables_allowed_ = true;
        if (!is_polynomial_) {
            is_polynomial_ = true;
            base.accept(*this);
            is_polynomial_ = is_polynomial_ and is_a<Integer>(exp)
                             and down_cast<const Integer &>(exp).is_positive();
        }
    } else {
        base.accept(*this);
        if (!is_polynomial_)
            return;
        exp.accept(*this);
    }
}

void PolynomialVisitor::bvisit(const Pow &x)
{
    check_power(*x.get_base(), *x.get_exp());
}

void PolynomialVisitor::bvisit(const Symbol &x)
{
    if (variables_allowed_)
        return;

    if (variables_.empty()) { // All symbols are variables
        is_polynomial_ = false;
    } else {
        for (const auto &elem : variables_) {
            if (x.__eq__(*elem)) {
                is_polynomial_ = false;
                return;
            }
        }
    }
}

bool PolynomialVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_polynomial_;
}

bool is_polynomial(const Basic &b, const set_basic &variables)
{
    PolynomialVisitor visitor(variables);
    return visitor.apply(b);
}
}
