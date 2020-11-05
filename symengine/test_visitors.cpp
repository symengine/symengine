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
}
