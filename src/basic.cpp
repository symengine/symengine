#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "mul.h"
#include "pow.h"


namespace CSymPy {

int Basic::__cmp__(const Basic &o) const
{
    auto &a = typeid(*this);
    auto &b = typeid(o);
    if (a == b) {
        return this->compare(o);
    } else {
        // This order is compiler implementation specific:
        return a.before(b) ? -1 : 1;
    }
}

RCP<Basic> expand(const RCP<Basic> &self)
{
    if (is_a<Symbol>(*self)) return self;
    if (is_a_Number(*self)) return self;
    if (is_a<Add>(*self)) return add_expand(rcp_static_cast<Add>(self));
    if (is_a<Mul>(*self)) return mul_expand(rcp_static_cast<Mul>(self));
    if (is_a<Pow>(*self)) return pow_expand(rcp_static_cast<Pow>(self));
    return self;
}

} // CSymPy
