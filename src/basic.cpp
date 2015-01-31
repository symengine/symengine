#include "basic.h"
#include "symbol.h"
#include "add.h"
#include "integer.h"
#include "rational.h"
#include "complex.h"
#include "mul.h"
#include "pow.h"
#include "constants.h"
#include "functions.h"

namespace CSymPy {

int Basic::__cmp__(const Basic &o) const
{
    auto a = this->get_type_code();
    auto b = o.get_type_code();
    if (a == b) {
        return this->compare(o);
    } else {
        // We return the order given by the numerical value of the TypeID enum type.
        // The types don't need to be ordered in any given way, they just need to be ordered.
        return a < b ? -1 : 1;
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
    typedef RCP<const Basic> (*fn)(const RCP<const Basic> &);
    std::vector<fn> table;
    table.assign(100, NULL);
    table[ADD] = add_expand;
    table[MUL] = mul_expand;
    table[POW] = pow_expand;
    fn f = table[self->get_type_code()];
    if (f == NULL) {
        return self;
    } else {
        return f(self);
    }
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

RCP<const Basic> Basic::diff(const RCP<const Symbol> &x) const
{
    return rcp(new Derivative(rcp(this), {x}));
}

} // CSymPy

