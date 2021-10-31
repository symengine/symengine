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
    } else if (is_true(is_zero(*newarg, assumptions_))) {
        result_ = integer(0);
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

void RefineVisitor::bvisit(const Pow &x)
{
    auto exp = x.get_exp();
    auto newexp = apply(exp);
    auto base = x.get_base();
    auto newbase = apply(base);
    // Handle cases when (x**k)**n = x**(k*n) or = abs(x)**(k*n)
    if (is_a<Pow>(*newbase) and is_a_Number(*newexp)) {
        const Pow &inner_pow = down_cast<const Pow &>(*newbase);
        auto inner_exp = inner_pow.get_exp();
        auto inner_base = inner_pow.get_base();
        if (is_true(is_real(*inner_base, assumptions_))
            and is_a_Number(*inner_exp)
            and not down_cast<const Number &>(*inner_exp).is_complex()
            and not down_cast<const Number &>(*newexp).is_complex()) {
            if (is_true(is_positive(*inner_base, assumptions_))) {
                result_ = pow(inner_base, mul(newexp, inner_exp));
            } else {
                result_ = pow(abs(inner_base), mul(newexp, inner_exp));
            }
            return;
        }
    }
    result_ = pow(newbase, newexp);
}

RCP<const Basic> refine(const RCP<const Basic> &x,
                        const Assumptions *assumptions)
{
    RefineVisitor b(assumptions);
    return b.apply(x);
}

} // namespace SymEngine
