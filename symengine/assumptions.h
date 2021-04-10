#ifndef SYMENGINE_ASSUMPTIONS_H
#define SYMENGINE_ASSUMPTIONS_H

#include <symengine/basic.h>

namespace SymEngine
{

class Assumptions
{
private:
    set_basic real_symbols_;
    set_basic rational_symbols_;
    set_basic integer_symbols_;

public:
    Assumptions(const set_basic &statements);
    tribool is_real(const RCP<const Basic> &symbol) const;
    tribool is_rational(const RCP<const Basic> &symbol) const;
    tribool is_integer(const RCP<const Basic> &symbol) const;
};
}

#endif // SYMENGINE_ASSUMPTIONS_H
