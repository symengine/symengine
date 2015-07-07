/**
 *  \file expression.h
 *  Includes the Expression class
 *
 **/

#ifndef SYMENGINE_EXPRESSION_H
#define SYMENGINE_EXPRESSION_H

#include <iostream>
#include <type_traits>

#include <symengine/add.h>
#include <symengine/basic.h>
#include <symengine/symengine_rcp.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>

namespace SymEngine
{

class Expression
{
private:
	RCP<const Basic> m_basic;

public:
	Expression():m_basic(new Integer(0)) {}

	template <typename T, typename = typename std::enable_if<std::is_constructible<RCP<const Basic>,T &&>::value>::type>
	Expression(T &&o):m_basic(std::forward<T>(o)) {}

	Expression(int n):m_basic(new Integer(n)) {}

	Expression(const Expression &) = default;

	Expression(Expression &&other) noexcept : m_basic(std::move(other.m_basic)) {}

	Expression &operator=(const Expression &) = default;
	Expression &operator=(Expression &&other) noexcept
	{
		if (this != &other) {
			*this = std::move(other);
		}
		return *this;
	}

	~Expression() noexcept {}

	friend std::ostream &operator<<(std::ostream &os, const Expression &expr)
	{
		os << (*expr.m_basic.get());
		return os;
	}

	friend Expression operator+(const Expression &a, const Expression &b)
	{
		return Expression(add(a.m_basic,b.m_basic));
	}

	Expression &operator+=(const Expression &other)
	{
		m_basic = add(m_basic,other.m_basic);
		return *this;
	}

	friend Expression operator-(const Expression &a, const Expression &b)
	{
		return Expression(sub(a.m_basic,b.m_basic));
	}

	Expression operator-() const
	{
		Expression retval(*this);
		retval *= -1;
		return retval;
	}

	Expression &operator-=(const Expression &other)
	{
		m_basic = sub(m_basic,other.m_basic);
		return *this;
	}

	friend Expression operator*(const Expression &a, const Expression &b)
	{
		return Expression(mul(a.m_basic,b.m_basic));
	}

	Expression &operator*=(const Expression &other)
	{
		m_basic = mul(m_basic,other.m_basic);
		return *this;
	}

	bool operator==(const Expression &other) const
	{
		return eq(m_basic,other.m_basic);
	}

	bool operator!=(const Expression &other) const
	{
		return !(*this == other);
	}

	const RCP<const Basic> &get_basic() const
	{
		return m_basic;
	}
};

inline Expression pow(const Expression &base, const Expression &exp)
{
	return pow(base.get_basic(),exp.get_basic());
}

inline Expression expand(const Expression &arg)
{
	return expand(arg.get_basic());
}

} // SymEngine

#endif
