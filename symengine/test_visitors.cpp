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
    tribool result = is_zero_;
    if (not zero_ and not neither_) {
        result = not_tribool(result);
    }
    return result;
}

tribool is_zero(const Basic &b)
{
    ZeroVisitor visitor(true);
    return visitor.apply(b);
}

tribool is_nonzero(const Basic &b)
{
    ZeroVisitor visitor(false);
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

void IntegerVisitor::bvisit(const Symbol &x)
{
    if (assumptions_) {
        is_integer_ = assumptions_->is_integer(x.rcp_from_this());
    } else {
        is_integer_ = tribool::indeterminate;
    }
}

void IntegerVisitor::bvisit(const Constant &x)
{
    if (eq(x, *pi) or eq(x, *E) or eq(x, *EulerGamma) or eq(x, *Catalan)
        or eq(x, *GoldenRatio)) {
        is_integer_ = tribool::trifalse;
    } else {
        is_integer_ = tribool::indeterminate;
    }
}

void IntegerVisitor::bvisit(const Add &x)
{
    for (const auto &arg : x.get_args()) {
        arg->accept(*this);
        if (not is_true(is_integer_)) {
            is_integer_ = tribool::indeterminate;
            return;
        }
    }
}

void IntegerVisitor::bvisit(const Mul &x)
{
    for (const auto &arg : x.get_args()) {
        arg->accept(*this);
        if (not is_true(is_integer_)) {
            is_integer_ = tribool::indeterminate;
            return;
        }
    }
}

tribool IntegerVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_integer_;
}

tribool is_integer(const Basic &b, const Assumptions *assumptions)
{
    IntegerVisitor visitor(assumptions);
    return visitor.apply(b);
}

void RealVisitor::bvisit(const Symbol &x)
{
    if (assumptions_) {
        is_real_ = assumptions_->is_real(x.rcp_from_this());
    } else {
        is_real_ = tribool::indeterminate;
    }
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

tribool is_real(const Basic &b, const Assumptions *assumptions)
{
    RealVisitor visitor(assumptions);
    return visitor.apply(b);
}

void ComplexVisitor::bvisit(const Symbol &x)
{
    if (assumptions_) {
        is_complex_ = assumptions_->is_complex(x.rcp_from_this());
    } else {
        is_complex_ = tribool::indeterminate;
    }
}

void ComplexVisitor::bvisit(const Number &x)
{
    if (is_a<Infty>(x) or is_a<NaN>(x)) {
        is_complex_ = tribool::trifalse;
    } else {
        is_complex_ = tribool::tritrue;
    }
}

void ComplexVisitor::bvisit(const Add &x)
{
    tribool b = tribool::tritrue;
    for (const auto &arg : x.get_args()) {
        arg->accept(*this);
        b = andwk_tribool(b, is_complex_);
        if (is_indeterminate(b) or is_false(b))
            return;
    }
}

void ComplexVisitor::bvisit(const Mul &x)
{
    tribool b = tribool::tritrue;
    for (const auto &p : x.get_dict()) {
        this->check_power(*p.first, *p.second);
        b = andwk_tribool(b, is_complex_);
        if (is_indeterminate(b) or is_false(b))
            return;
    }
}

void ComplexVisitor::check_power(const Basic &base, const Basic &exp)
{
    base.accept(*this);
    if (is_true(is_complex_)) {
        exp.accept(*this);
    }
}

void ComplexVisitor::bvisit(const Pow &x)
{
    check_power(*x.get_base(), *x.get_exp());
}

void ComplexVisitor::bvisit(const Log &x)
{
    complex_arg_not_zero(x, *x.get_arg());
}

void ComplexVisitor::bvisit(const Tan &x)
{
    complex_arg_not_zero(x, *cos(x.get_arg()));
}

void ComplexVisitor::complex_arg_not_zero(const OneArgFunction &x,
                                          const Basic &not_zero)
{
    // Check if function argument is complex and then if 'not_zero' is not zero
    x.get_arg()->accept(*this);
    if (is_true(is_complex_)) {
        tribool zero = is_zero(not_zero);
        if (not is_false(zero)) {
            is_complex_ = not_tribool(zero);
        }
    }
}

void ComplexVisitor::complex_arg_not_pm(const OneArgFunction &x, bool one)
{
    // Check if function argument is complex but not plus/minus 1 (one=True) or
    // i (one=False)
    x.get_arg()->accept(*this);
    if (not is_true(is_complex_))
        return;
    RCP<const Number> i1;
    if (one)
        i1 = integer(1);
    else
        i1 = Complex::from_two_nums(*integer(0), *integer(1));
    tribool zi1 = is_zero(*sub(x.get_arg(), i1));
    if (not is_false(zi1)) {
        is_complex_ = not_tribool(zi1);
        return;
    }
    RCP<const Number> mi1;
    if (one)
        mi1 = integer(-1);
    else
        mi1 = Complex::from_two_nums(*integer(0), *integer(-1));
    tribool zmi1 = is_zero(*sub(x.get_arg(), mi1));
    is_complex_ = not_tribool(zmi1);
}

void ComplexVisitor::bvisit(const ATan &x)
{
    complex_arg_not_pm(x, false);
}

void ComplexVisitor::bvisit(const ATanh &x)
{
    complex_arg_not_pm(x, true);
}

void ComplexVisitor::bvisit(const ACot &x)
{
    complex_arg_not_pm(x, false);
}

void ComplexVisitor::bvisit(const ACoth &x)
{
    complex_arg_not_pm(x, true);
}

void ComplexVisitor::bvisit(const Cot &x)
{
    complex_arg_not_zero(x, *sin(x.get_arg()));
}

void ComplexVisitor::bvisit(const Sec &x)
{
    complex_arg_not_zero(x, *cos(x.get_arg()));
}

void ComplexVisitor::bvisit(const ASec &x)
{
    complex_arg_not_zero(x, *x.get_arg());
}

void ComplexVisitor::bvisit(const ASech &x)
{
    complex_arg_not_zero(x, *x.get_arg());
}

void ComplexVisitor::bvisit(const Csc &x)
{
    complex_arg_not_zero(x, *sin(x.get_arg()));
}

void ComplexVisitor::bvisit(const ACsc &x)
{
    complex_arg_not_zero(x, *x.get_arg());
}

void ComplexVisitor::bvisit(const ACsch &x)
{
    complex_arg_not_zero(x, *x.get_arg());
}

tribool ComplexVisitor::apply(const Basic &b)
{
    b.accept(*this);
    return is_complex_;
}

tribool is_complex(const Basic &b, const Assumptions *assumptions)
{
    ComplexVisitor visitor(assumptions);
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

void RationalVisitor::bvisit(const Number &x)
{
    is_rational_ = tribool::trifalse;
    if (is_a_Complex(x) or is_a<Infty>(x) or is_a<NaN>(x)) {
        neither_ = true;
    }
}

void RationalVisitor::bvisit(const Constant &x)
{
    if (eq(x, *pi) or eq(x, *E) or eq(x, *GoldenRatio)) {
        // It is currently (2021) not known whether Catalan's constant
        // or Euler's constant are rational or irrational
        is_rational_ = tribool::trifalse;
    } else {
        is_rational_ = tribool::indeterminate;
    }
}

void RationalVisitor::bvisit(const Add &x)
{
    tribool b = tribool::tritrue;
    for (const auto &arg : x.get_args()) {
        arg->accept(*this);
        b = andwk_tribool(b, is_rational_);
        if (is_indeterminate(b))
            return;
    }
}

tribool RationalVisitor::apply(const Basic &b)
{
    b.accept(*this);
    tribool result = is_rational_;
    if (not rational_ and not neither_) {
        result = not_tribool(result);
    }
    return result;
}

tribool is_rational(const Basic &b)
{
    RationalVisitor visitor(true);
    return visitor.apply(b);
}

tribool is_irrational(const Basic &b)
{
    RationalVisitor visitor(false);
    return visitor.apply(b);
}
} // namespace SymEngine
