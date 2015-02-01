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

template <class T>
RCP<const T> rcp_static_cast_T(const RCP<const Basic> &self) {
    return rcp_static_cast<const T>(self);
}

RCP<const Basic> expand(const RCP<const Basic> &self)
{
    typedef std::function<RCP<const Basic>(const RCP<const Basic>)> fn;
    using std::placeholders::_1;
    std::vector<fn> table;
    table.assign(100, NULL);
    table[ADD] = std::bind(add_expand, std::bind(rcp_static_cast_T<Add>, _1));
    table[MUL] = std::bind(mul_expand, std::bind(rcp_static_cast_T<Mul>, _1));
    table[POW] = std::bind(pow_expand, std::bind(rcp_static_cast_T<Pow>, _1));
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

