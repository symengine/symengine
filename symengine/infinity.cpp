#include <symengine/infinity.h>

namespace SymEngine
{

Infinity::Infinity()
{
	_direction = integer(1);
}

Infinity::Infinity(const RCP<const Number> &direction)
{
	_direction = direction;
	SYMENGINE_ASSERT(is_canonical(_direction));
}

Infinity::Infinity(const int val)
{
	_direction = integer(val);
	SYMENGINE_ASSERT(is_canonical(_direction));
}

RCP<const Infinity> Infinity::from_number(const RCP<const Number> &num)
{
	return make_rcp<Infinity>(num);
}

RCP<const Infinity> Infinity::from_int(const int val)
{
	return make_rcp<Infinity>(val);
}

bool Infinity::is_canonical(const RCP<const Number> &num) const
{
	if(is_a<Complex>(*num) || is_a<ComplexDouble>(*num))
		throw std::runtime_error("Not implemented for all directions");

	return true;
}

std::size_t Infinity::__hash__() const
{
	std::size_t seed = INFINITY;
	hash_combine<Basic>(seed, *_direction);
	return seed;
}

bool Infinity::is_unsigned_infinity() const
{
	return _direction->is_zero();
}

bool Infinity::is_plus_infinity() const
{
	return _direction->is_positive();
}

bool Infinity::is_negative_infinity() const
{
	return _direction->is_negative();
}

} //SymEngine
