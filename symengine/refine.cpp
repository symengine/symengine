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

void RefineVisitor::bvisit(const Max &x)
{
    // positive > nonpositive
    // nonnegative and positive > negative
    vec_basic nonpositive;
    vec_basic negative;
    vec_basic keep;
    bool have_positive = false;
    bool have_nonnegative = false;
    for (auto arg : x.get_args()) {
        auto newarg = apply(arg);
        if (is_true(is_positive(*newarg, assumptions_))) {
            keep.push_back(newarg);
            have_positive = true;
        } else if (is_true(is_nonnegative(*newarg, assumptions_))) {
            keep.push_back(newarg);
            have_nonnegative = true;
        } else if (is_true(is_negative(*newarg, assumptions_))) {
            negative.push_back(newarg);
        } else if (is_true(is_nonpositive(*newarg, assumptions_))) {
            nonpositive.push_back(newarg);
        } else {
            keep.push_back(newarg);
        }
    }
    if (not have_positive and not nonpositive.empty()) {
        std::copy(nonpositive.begin(), nonpositive.end(),
                  std::back_inserter(keep));
    }
    if (not have_nonnegative and not have_positive and not negative.empty()) {
        std::copy(negative.begin(), negative.end(), std::back_inserter(keep));
    }

    result_ = max(keep);
}

void RefineVisitor::bvisit(const Min &x)
{
    // negative < nonnegative
    // nonpositive and negative < positive
    vec_basic nonnegative;
    vec_basic positive;
    vec_basic keep;
    bool have_negative = false;
    bool have_nonpositive = false;
    for (auto arg : x.get_args()) {
        auto newarg = apply(arg);
        if (is_true(is_negative(*newarg, assumptions_))) {
            keep.push_back(newarg);
            have_negative = true;
        } else if (is_true(is_nonpositive(*newarg, assumptions_))) {
            keep.push_back(newarg);
            have_nonpositive = true;
        } else if (is_true(is_positive(*newarg, assumptions_))) {
            positive.push_back(newarg);
        } else if (is_true(is_nonnegative(*newarg, assumptions_))) {
            nonnegative.push_back(newarg);
        } else {
            keep.push_back(newarg);
        }
    }
    if (not have_negative and not nonnegative.empty()) {
        std::copy(nonnegative.begin(), nonnegative.end(),
                  std::back_inserter(keep));
    }
    if (not have_nonpositive and not have_negative and not positive.empty()) {
        std::copy(positive.begin(), positive.end(), std::back_inserter(keep));
    }

    result_ = min(keep);
}

RCP<const Basic> refine(const RCP<const Basic> &x,
                        const Assumptions *assumptions)
{
    RefineVisitor b(assumptions);
    return b.apply(x);
}

} // namespace SymEngine
