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
}
