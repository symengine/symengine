#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <symengine/constants.h>
#include <symengine/infinity.h>
#include <symengine/functions.h>
#include <symengine/symengine_exception.h>

namespace SymEngine
{

Infty::Infty(const RCP<const Number> &direction)
{
    _direction = direction;
    SYMENGINE_ASSERT(is_canonical(_direction));
}

Infty::Infty(const Infty &inf)
{
    _direction = inf.get_direction();
    SYMENGINE_ASSERT(is_canonical(_direction))
}

RCP<const Infty> Infty::from_direction(const RCP<const Number> &direction)
{
    return make_rcp<Infty>(direction);
}

RCP<const Infty> Infty::from_int(const int val)
{
    SYMENGINE_ASSERT(val >= -1 && val <= 1)
    return make_rcp<Infty>(integer(val));
}

//! Canonical when the direction is -1, 0 or 1.
bool Infty::is_canonical(const RCP<const Number> &num) const
{
    if (is_a<Complex>(*num) || is_a<ComplexDouble>(*num))
        throw NotImplementedError("Not implemented for all directions");

    if (num->is_one() || num->is_zero() || num->is_minus_one())
        return true;

    return false;
}

hash_t Infty::__hash__() const
{
    hash_t seed = INFTY;
    hash_combine<Basic>(seed, *_direction);
    return seed;
}

bool Infty::__eq__(const Basic &o) const
{
    if (is_a<Infty>(o)) {
        const Infty &s = static_cast<const Infty &>(o);
        return eq(*_direction, *(s.get_direction()));
    }

    return false;
}

int Infty::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Infty>(o))
    const Infty &s = static_cast<const Infty &>(o);
    return _direction->compare(*(s.get_direction()));
}

bool Infty::is_unsigned_infinity() const
{
    return _direction->is_zero();
}

bool Infty::is_positive_infinity() const
{
    return _direction->is_positive();
}

bool Infty::is_negative_infinity() const
{
    return _direction->is_negative();
}

RCP<const Number> Infty::add(const Number &other) const
{
    if (not is_a<Infty>(other))
        return rcp_from_this_cast<Number>();

    const Infty &s = static_cast<const Infty &>(other);

    if (not eq(*s.get_direction(), *_direction)) {
        if (is_unsigned_infinity() or s.is_unsigned_infinity())
            throw SymEngineException("Indeterminate Expression: "
                                     "`unsigned_Infty +- Infty` "
                                     "encountered");
        else
            throw SymEngineException("Indeterminate Expression: `Infty +- "
                                     "Infty` encountered. Directions don't "
                                     "match");
    } else if (is_unsigned_infinity()) {
        throw SymEngineException("Indeterminate Expression: "
                                 "`unsigned_Infty +- unsigned Infty` "
                                 "encountered");
    } else
        return rcp_from_this_cast<Number>();
}

RCP<const Number> Infty::mul(const Number &other) const
{
    if (is_a<Complex>(other))
        throw NotImplementedError("Multiplation with Complex not implemented");

    if (is_a<Infty>(other)) {
        const Infty &s = static_cast<const Infty &>(other);

        return make_rcp<const Infty>(this->_direction->mul(*(s._direction)));
    } else {
        if (other.is_positive())
            return rcp_from_this_cast<Number>();
        else if (other.is_negative())
            return make_rcp<const Infty>(this->_direction->mul(*minus_one));
        else
            throw SymEngineException(
                "Indeterminate Expression: `0 * Infty` encountered");
    }
}

// TODO
RCP<const Number> Infty::div(const Number &other) const
{
    return zero;
}
// TODO
RCP<const Number> Infty::pow(const Number &other) const
{
    return zero;
}
// TODO
RCP<const Number> Infty::rpow(const Number &other) const
{
    return zero;
}

inline RCP<const Infty> infty(const RCP<const Number> &direction)
{
    return make_rcp<Infty>(direction);
}

} // SymEngine
