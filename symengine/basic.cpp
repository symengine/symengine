#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/constants.h>
#include <symengine/functions.h>
#include <symengine/polynomial.h>
#include <symengine/printer.h>
#include <symengine/subs.h>

namespace SymEngine
{

int Basic::__cmp__(const Basic &o) const
{
    auto a = this->get_type_code();
    auto b = o.get_type_code();
    if (a == b) {
        return this->compare(o);
    } else {
        // We return the order given by the numerical value of the TypeID enum
        // type.
        // The types don't need to be ordered in any given way, they just need
        // to be ordered.
        return a < b ? -1 : 1;
    }
}

std::string Basic::__str__() const
{
    StrPrinter strPrinter;
    return strPrinter.apply(*this);
}

RCP<const Basic> Basic::subs(const map_basic_basic &subs_dict) const
{
    return SymEngine::subs(this->rcp_from_this(), subs_dict);
}

RCP<const Basic> Basic::diff(const RCP<const Symbol> &x) const
{
    return Derivative::create(rcp_from_this(), {x});
}

} // SymEngine
