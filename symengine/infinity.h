/**
 *  \file infinity.h
 *
 **/

#ifndef SYMENGINE_INFINITY_H
#define SYMENGINE_INFINITY_H

#include <symengine/basic.h>
#include <symengine/complex.h>
#include <symengine/complex_double.h>
#include <symengine/number.h>

namespace SymEngine
{

/** This class holds "infinity"
 *  It includes a direction (like -infinity).
 **/
class Infinity : public Basic
{
	RCP<const Number> _direction;

public:
	IMPLEMENT_TYPEID(INFINITY)
	//! Constructors

	//! Default Constructor gives +infinity(direction = +1)
	Infinity();
	//! Constructs Infinity using the sign of `_direction`
	Infinity(const RCP<const Number> &direction);
	Infinity(const int val);
	static RCP<const Infinity> from_number(const RCP<const Number> &num);
	//! Constructs Infinity using sign of `val`
	static RCP<const Infinity> from_int(const int val);

	//! \return true if canonical
	bool is_canonical(const RCP<const Number> &num) const;
	//! \return size of the hash
	std::size_t __hash__() const;

	/*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    // Implement these
    bool __eq__(const Basic &o) const{return true;}
    int compare(const Basic &o) const{return 0;}

    virtual vec_basic get_args() const
    {
    	return {_direction};
    }

    inline RCP<const Number> get_direction() const
    {
    	return _direction;
    }

    inline RCP<const Infinity> infinity()
    {
    	return make_rcp<Infinity>();
    }
	bool is_unsigned_infinity() const;
	bool is_plus_infinity() const;
	bool is_negative_infinity() const;

	// const infinity & operator *= (const Basic& rhs);
	// const infinity & operator += (const Basic& rhs);
};

} //SymEngine
#endif
