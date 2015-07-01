/**
 *  \file expression.h
 *  Includes the expression class
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

class expression
{
	public:
		expression():m_basic(new Integer(0)) {}
		template <typename T, typename = typename std::enable_if<std::is_constructible<RCP<const Basic>,T &&>::value>::type>
		expression(T &&o):m_basic(std::forward<T>(o)) {}
		expression(int n):m_basic(new Integer(n)) {}
		expression(const expression &) = default;
		expression(expression &&other) noexcept : m_basic(std::move(other.m_basic)) {}
		expression &operator=(const expression &) = default;
		expression &operator=(expression &&other) noexcept
		{
			if (this != &other) {
				*this = std::move(other);
			}
			return *this;
		}
		~expression() noexcept {}
		friend std::ostream &operator<<(std::ostream &os, const expression &expr)
		{
			os << (*expr.m_basic.get());
			return os;
		}
		friend expression operator+(const expression &a, const expression &b)
		{
			return expression(add(a.m_basic,b.m_basic));
		}
		expression &operator+=(const expression &other)
		{
			m_basic = add(m_basic,other.m_basic);
			return *this;
		}
		friend expression operator-(const expression &a, const expression &b)
		{
			return expression(sub(a.m_basic,b.m_basic));
		}
		expression operator-() const
		{
			expression retval(*this);
			retval *= -1;
			return retval;
		}
		expression &operator-=(const expression &other)
		{
			m_basic = sub(m_basic,other.m_basic);
			return *this;
		}
		friend expression operator*(const expression &a, const expression &b)
		{
			return expression(mul(a.m_basic,b.m_basic));
		}
		expression &operator*=(const expression &other)
		{
			m_basic = mul(m_basic,other.m_basic);
			return *this;
		}
		bool operator==(const expression &other) const
		{
			return eq(m_basic,other.m_basic);
		}
		bool operator!=(const expression &other) const
		{
			return !(*this == other);
		}
		const RCP<const Basic> &get_basic() const
		{
			return m_basic;
		}
	private:
		RCP<const Basic> m_basic;
};

inline expression pow(const expression &base, const expression &exp)
{
	return pow(base.get_basic(),exp.get_basic());
}

inline expression expand(const expression &arg)
{
	return expand(arg.get_basic());
}

} // SymEngine

#endif