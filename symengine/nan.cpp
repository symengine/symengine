#include <symengine/nan.h>

namespace SymEngine
{

NaN::NaN()
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t NaN::__hash__() const
{
    hash_t seed = NOT_A_NUMBER;
    return seed;
}

bool NaN::__eq__(const Basic &o) const
{
    if (is_a<NaN>(o))
        return true;
    else
        return false;
}

int NaN::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<NaN>(o))
    return 0;
}

RCP<const Number> NaN::add(const Number &other) const
{
    return rcp_from_this_cast<Number>();
}

RCP<const Number> NaN::mul(const Number &other) const
{
    return rcp_from_this_cast<Number>();
}

RCP<const Number> NaN::div(const Number &other) const
{
    return rcp_from_this_cast<Number>();
}

RCP<const Number> NaN::pow(const Number &other) const
{
    return rcp_from_this_cast<Number>();
}

RCP<const Number> NaN::rpow(const Number &other) const
{
    return rcp_from_this_cast<Number>();
}

} // SymEngine
