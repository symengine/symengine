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
    if (is_a<Pow>(*arg)) {
        // Int[(a_./x_)^p_,x_Symbol] := -a*(a/x)^(p-1)/(p-1) /;
        // FreeQ[{a,p},x] && Not[IntegerQ[p]]
        auto base = down_cast<const Pow &>(*arg).get_base();
        auto p = down_cast<const Pow &>(*arg).get_exp();
        if (is_a<Mul>(*base) and not has_symbol(*p, *x)
            and not is_a<Integer>(*p)) {
            map_basic_basic factor_out;
            bool found = false;
            for (const auto &pair : down_cast<const Mul &>(*base).get_dict()) {
                if (eq(*pair.first, *x) and eq(*pair.second, *integer(-1))) {
                    found = true;
                } else {
                    factor_out.insert(pair);
                }
            }
            if (found) {
                auto a
                    = Mul::from_dict(down_cast<const Mul &>(*base).get_coef(),
                                     std::move(factor_out));
                auto pm1 = sub(p, integer(1));
                return div(mul(mul(a, integer(-1)), pow(base, pm1)), pm1);
            }
        }
    }

    return make_rcp<const Integral>(arg, x);
}

} // namespace SymEngine
