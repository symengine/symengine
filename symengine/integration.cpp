#include <symengine/visitor.h>
#include <symengine/subs.h>
#include <symengine/symengine_casts.h>
#include <symengine/integration.h>

namespace SymEngine
{

// This is integration based on Rubi-5
// Rubi-5 is not a finished product, but a prototype is available
// as of March 2021 at https://github.com/RuleBasedIntegration/Rubi-5
// Rubi-5 will rely on less than 50 pattern matching rules and below these
// a tree of if-then-else functions.
// The plan is to manually implement the pattern matching rules in symengine
// and to automatically generate code for the if-then-else functions from
// the Rubi-5 mathematica code.

RCP<const Basic> integrate(const RCP<const Basic> &arg,
                           const RCP<const Symbol> &x)
{
    if (not has_symbol(*arg, *x)) {
        // Int[a_,x_Symbol] := a*x /; FreeQ[a,x]
        return mul(arg, x);
    }
    if (is_a<Mul>(*arg)) {
        // Int[a_*Fx_,x_Symbol] := a \[Star] Int[Fx,x] /; FreeQ[a,x]
        map_basic_basic factor_out;
        map_basic_basic keep;
        for (const auto &p : down_cast<const Mul &>(*arg).get_dict()) {
            if (has_symbol(*p.first, *x) or has_symbol(*p.second, *x)) {
                keep.insert(p);
            } else {
                factor_out.insert(p);
            }
        }
        // Could be optimized by remembering factor_out and continuing here
        return mul(Mul::from_dict(down_cast<const Mul &>(*arg).get_coef(),
                                  std::move(factor_out)),
                   integrate(Mul::from_dict(one, std::move(keep)), x));
    }

    return make_rcp<const Integral>(arg, x);
}

} // namespace SymEngine
