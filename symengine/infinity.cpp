#include <symengine/infinity.h>
#include <symengine/constants.h>

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

RCP<const Infinit> Infinit::from_number(const RCP<const Number> &num)
{
	return make_rcp<Infinit>(num);
}

RCP<const Infinit> Infinit::from_int(const int val)
{
	return make_rcp<Infinit>(val);
}

bool Infinit::is_canonical(const RCP<const Number> &num) const
{
	if(is_a<Complex>(*num) || is_a<ComplexDouble>(*num))
		throw std::runtime_error("Not implemented for all directions");

	return true;
}

std::size_t Infinit::__hash__() const
{
	std::size_t seed = INFINITY;
	hash_combine<Basic>(seed, *_direction);
	return seed;
}

bool Infinit::__eq__(const Basic &o) const
{
	if(eq(*_direction, *zero))
		return false;

	if (is_a<Infinit>(o))
	{
		const Infinit &s = static_cast<const Infinit &>(o);
		if(eq(*_direction, *(s.get_direction())))
			return true;
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

} //SymEngine
