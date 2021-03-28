#include <symengine/logic.h>
#include <symengine/assumptions.h>

namespace SymEngine
{

Assumptions::Assumptions(const set_basic &statements)
{
    // Convert a set of statements into a faster and easier internal form
    for (const auto &s : statements) {
        if (is_a<Contains>(*s)) {
            const Contains &contains = down_cast<const Contains &>(*s);
            const auto expr = contains.get_expr();
            const auto set = contains.get_set();
            if (is_a<Symbol>(*expr)) {
                if (is_a<Reals>(*set)) {
                    complex_symbols_.insert(expr);
                    real_symbols_.insert(expr);
                } else if (is_a<Rationals>(*set)) {
                    complex_symbols_.insert(expr);
                    real_symbols_.insert(expr);
                    rational_symbols_.insert(expr);
                } else if (is_a<Integers>(*set)) {
                    complex_symbols_.insert(expr);
                    real_symbols_.insert(expr);
                    rational_symbols_.insert(expr);
                    integer_symbols_.insert(expr);
                }
            }
        }
    }
}

tribool Assumptions::is_complex(const RCP<const Basic> &symbol) const
{
    bool cmplx = complex_symbols_.find(symbol) != complex_symbols_.end();
    return cmplx ? tribool::tritrue : tribool::indeterminate;
}

tribool Assumptions::is_real(const RCP<const Basic> &symbol) const
{
    bool real = real_symbols_.find(symbol) != real_symbols_.end();
    return real ? tribool::tritrue : tribool::indeterminate;
}

tribool Assumptions::is_rational(const RCP<const Basic> &symbol) const
{
    bool rational = rational_symbols_.find(symbol) != rational_symbols_.end();
    return rational ? tribool::tritrue : tribool::indeterminate;
}

tribool Assumptions::is_integer(const RCP<const Basic> &symbol) const
{
    bool integer = integer_symbols_.find(symbol) != integer_symbols_.end();
    return integer ? tribool::tritrue : tribool::indeterminate;
}
}
