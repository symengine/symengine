#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/symbol.h>
#include <symengine/pow.h>
#include <symengine/rational.h>
#include <symengine/functions.h>
#include <symengine/integer.h>

namespace SymEngine
{

RCP<const Number> Number::sub(const Number &other) const
{
    return add(*other.mul(*integer(-1)));
}

RCP<const Number> Number::rsub(const Number &other) const
{
    return mul(*integer(-1))->add(other);
}

RCP<const Number> Number::div(const Number &other) const
{
    return mul(*other.pow(*integer(-1)));
}

RCP<const Number> Number::rdiv(const Number &other) const
{
    return other.mul(*pow(*integer(-1)));
}

} // SymEngine
