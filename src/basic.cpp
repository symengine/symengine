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

std::string Basic::__str__() const
{
    std::ostringstream s;
    s << "<" << typeName<Basic>(*this)
        << " instance at " << (const void*)this << ">";
    return s.str();
}

RCP<const Basic> expand(const RCP<const Basic> &self)
{
    if (is_a<Symbol>(*self)) return self;
    if (is_a_Number(*self)) return self;
    if (is_a<Add>(*self)) return add_expand(rcp_static_cast<const Add>(self));
    if (is_a<Mul>(*self)) return mul_expand(rcp_static_cast<const Mul>(self));
    if (is_a<Pow>(*self)) return pow_expand(rcp_static_cast<const Pow>(self));
    return self;
}

RCP<const Basic> Basic::subs(const map_basic_basic &subs_dict) const
{
    RCP<const Basic> self = rcp_const_cast<Basic>(rcp(this));
    auto it = subs_dict.find(self);
    if (it == subs_dict.end())
        return self;
    else
        return it->second;
}

} // CSymPy

