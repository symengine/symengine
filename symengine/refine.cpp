#include <symengine/refine.h>

namespace SymEngine
{

void RefineVisitor::bvisit(const Abs &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    if (is_true(is_nonnegative(*newarg, assumptions_))) {
        result_ = newarg;
    } else if (is_true(is_nonpositive(*newarg, assumptions_))) {
        result_ = neg(newarg);
    } else if (is_a<Conjugate>(*newarg)) {
        result_ = abs(down_cast<const Conjugate &>(*newarg).get_arg());
    } else {
        result_ = abs(newarg);
    }
}

void RefineVisitor::bvisit(const Sign &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    if (is_true(is_positive(*newarg, assumptions_))) {
        result_ = integer(1);
    } else if (is_true(is_negative(*newarg, assumptions_))) {
        result_ = integer(-1);
        //    } else if (is_true(is_zero(*newarg, assumptions_))) {
        //        result_ = integer(0);
    } else {
        result_ = sign(newarg);
    }
}

void RefineVisitor::bvisit(const Floor &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    if (is_true(is_integer(*newarg, assumptions_))) {
        result_ = newarg;
    } else if (could_extract_minus(*newarg)) {
        result_ = neg(ceiling(neg(newarg)));
    } else {
        result_ = floor(newarg);
    }
}

void RefineVisitor::bvisit(const Ceiling &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    if (is_true(is_integer(*newarg, assumptions_))) {
        result_ = newarg;
    } else if (could_extract_minus(*newarg)) {
        result_ = neg(floor(neg(newarg)));
    } else {
        result_ = ceiling(newarg);
    }
}

void RefineVisitor::bvisit(const Conjugate &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    if (is_true(is_real(*newarg, assumptions_))) {
        result_ = newarg;
    } else {
        result_ = conjugate(newarg);
    }
}

RCP<const Basic> refine(const RCP<const Basic> &x,
                        const Assumptions *assumptions)
{
    RefineVisitor b(assumptions);
    return b.apply(x);
}

} // namespace SymEngine
