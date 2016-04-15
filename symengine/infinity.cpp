#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <symengine/constants.h>
#include <symengine/infinity.h>

namespace SymEngine
{

Infinit::Infinit()
{
    _direction = integer(1);
}

Infinit::Infinit(const RCP<const Number> &direction)
{
    _direction = direction;
    SYMENGINE_ASSERT(is_canonical(_direction));
}

Infinit::Infinit(const int val)
{
    _direction = integer(val);
    SYMENGINE_ASSERT(is_canonical(_direction));
}

Infinit::Infinit(const Infinit &inf)
{
    _direction = inf.get_direction();
    SYMENGINE_ASSERT(is_canonical(_direction))
}

RCP<const Infinit> Infinit::from_direction(const RCP<const Number> &direction)
{
    return make_rcp<Infinit>(direction);
}

RCP<const Infinit> Infinit::from_int(const int val)
{
    SYMENGINE_ASSERT(val >= -1 && val <= 1)
    return make_rcp<Infinit>(val);
}

//! Canonical when the direction is -1, 0 or 1.
bool Infinit::is_canonical(const RCP<const Number> &num) const
{
    if (is_a<Complex>(*num) || is_a<ComplexDouble>(*num))
        throw std::runtime_error("Not implemented for all directions");

    if (num->is_one() || num->is_zero() || num->is_minus_one())
        return true;

    return false;
}

std::size_t Infinit::__hash__() const
{
    std::size_t seed = INFINIT;
    hash_combine<Basic>(seed, *_direction);
    return seed;
}

bool Infinit::__eq__(const Basic &o) const
{
    if (is_unsigned_infinity())
        return false;

    if (is_a<Infinit>(o)) {
        const Infinit &s = static_cast<const Infinit &>(o);
        return eq(*_direction, *(s.get_direction()));
    }

    return false;
}

int Infinit::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Infinit>(o))
    const Infinit &s = static_cast<const Infinit &>(o);
    return _direction->compare(*(s.get_direction()));
}

bool Infinit::is_unsigned_infinity() const
{
    return _direction->is_zero();
}

bool Infinit::is_positive_infinity() const
{
    return _direction->is_positive();
}

bool Infinit::is_negative_infinity() const
{
    return _direction->is_negative();
}

RCP<const Number> Infinit::add(const Number &other) const
{
    if (not is_a<Infinit>(other))
        return make_rcp<const Infinit>(*this);

    const Infinit &s = static_cast<const Infinit &>(other);

    if (not eq(*s.get_direction(), *_direction)) {
        if (is_unsigned_infinity() or s.is_unsigned_infinity())
            throw std::runtime_error("Indeterminate Expression: "
                                     "`unsigned_infinity +- infinity` "
                                     "encountered");
        else
            throw std::runtime_error("Indeterminate Expression: `infinity +- "
                                     "infinity` encountered. Directions don't "
                                     "match");
    } else
        return make_rcp<const Infinit>(*this);
}

RCP<const Number> Infinit::sub(const Number &other) const
{
    return this->add(*other.mul(*minus_one));
}

RCP<const Number> Infinit::mul(const Number &other) const
{
    if (is_a<Complex>(other))
        throw std::runtime_error("Multiplation with Complex not implemented");

    if (is_a<Infinit>(other)) {
        const Infinit &s = static_cast<const Infinit &>(other);

        return make_rcp<const Infinit>(this->_direction->mul(*(s._direction)));
    } else {
        if (other.is_positive())
            return make_rcp<const Infinit>(this->_direction);
        else if (other.is_negative())
            return make_rcp<const Infinit>(this->_direction->mul(*minus_one));
        else
            throw std::runtime_error(
                "Indeterminate Expression: `0 * Infinity` encountered");
    }
}

RCP<const Number> Infinit::div(const Number &other) const
{
    return zero;
}
RCP<const Number> Infinit::pow(const Number &other) const
{
    return zero;
}
RCP<const Number> Infinit::rpow(const Number &other) const
{
    return zero;
}
// RCP<const Number> Infinit::pow(const Number &other) const
// {

// }

} // SymEngine
