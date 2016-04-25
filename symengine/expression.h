/**
 *  \file expression.h
 *  Includes the Expression class
 *
 **/

#ifndef SYMENGINE_EXPRESSION_H
#define SYMENGINE_EXPRESSION_H

#include <symengine/add.h>
#include <symengine/basic.h>
#include <symengine/symengine_rcp.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>

namespace SymEngine
{

class Expression
{
private:
    RCP<const Basic> m_basic;

public:
    //! Plain constructor of Expression
    Expression() : m_basic(integer(0))
    {
    }
    //! Construct Expression from integral types
    template <class T>
    Expression(
        T n,
        typename std::enable_if<std::is_integral<T>::value>::type * = nullptr)
        : m_basic(integer(n))
    {
    }
    //! Construct Expression from floating point types
    template <class T>
    Expression(T n,
               typename std::enable_if<std::is_floating_point<T>::value>::type
                   * = nullptr)
        : m_basic(real_double(n))
    {
    }
    //! Construct Expression from std::complex<> types
    template <class T>
    Expression(std::complex<T> n,
               typename std::enable_if<std::is_floating_point<T>::value>::type
                   * = nullptr)
        : m_basic(complex_double(n))
    {
    }

#if defined(HAVE_SYMENGINE_IS_CONSTRUCTIBLE)
    template <
        typename T,
        typename
        = typename std::enable_if<std::is_constructible<RCP<const Basic>,
                                                        T &&>::value>::type>
#else
    template <typename T>
#endif
    Expression(T &&o)
        : m_basic(std::forward<T>(o))
    {
    }
    //! Construct Expression from Expression
    Expression(const Expression &) = default;
    //! Construct Expression from reference to Expression
    Expression(Expression &&other) SYMENGINE_NOEXCEPT
        : m_basic(std::move(other.m_basic))
    {
    }
    //! Overload assignment operator
    Expression &operator=(const Expression &) = default;
    //! Overload assignment operator for reference
    Expression &operator=(Expression &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other) {
            this->m_basic = std::move(other.m_basic);
        }
        return *this;
    }
    //! Destructor of Expression
    ~Expression() SYMENGINE_NOEXCEPT
    {
    }
    //! Overload stream operator
    friend std::ostream &operator<<(std::ostream &os, const Expression &expr)
    {
        os << expr.m_basic->__str__();
        return os;
    }
    //! Overload addition
    friend Expression operator+(const Expression &a, const Expression &b)
    {
        return Expression(add(a.m_basic, b.m_basic));
    }
    //! Overload addition and assignment(+=)
    Expression &operator+=(const Expression &other)
    {
        m_basic = add(m_basic, other.m_basic);
        return *this;
    }
    //! Overload subtraction
    friend Expression operator-(const Expression &a, const Expression &b)
    {
        return Expression(sub(a.m_basic, b.m_basic));
    }
    //! Overload unary negative
    Expression operator-() const
    {
        Expression retval(*this);
        retval *= -1;
        return retval;
    }
    //! Overload subtraction and assignment(-=)
    Expression &operator-=(const Expression &other)
    {
        m_basic = sub(m_basic, other.m_basic);
        return *this;
    }
    //! Overload multiplication
    friend Expression operator*(const Expression &a, const Expression &b)
    {
        return Expression(mul(a.m_basic, b.m_basic));
    }
    //! Overload multiplication and assignment (*=)
    Expression &operator*=(const Expression &other)
    {
        m_basic = mul(m_basic, other.m_basic);
        return *this;
    }
    //! Overload Division
    friend Expression operator/(const Expression &a, const Expression &b)
    {
        return Expression(div(a.m_basic, b.m_basic));
    }
    //! Overload Division and assignment (/=)
    Expression &operator/=(const Expression &other)
    {
        m_basic = div(m_basic, other.m_basic);
        return *this;
    }
    //! Overload check equality (==)
    bool operator==(const Expression &other) const
    {
        return eq(*m_basic, *other.m_basic);
    }

    //! Overload check not equal (!=)
    bool operator!=(const Expression &other) const
    {
        return not(*this == other);
    }

    //! Method to get Basic from Expression
    const RCP<const Basic> &get_basic() const
    {
        return m_basic;
    }
};

inline Expression pow_ex(const Expression &base, const Expression &exp)
{
    return pow(base.get_basic(), exp.get_basic());
}

inline Expression expand(const Expression &arg)
{
    return expand(arg.get_basic());
}

namespace detail
{
// This function must have external linkage
std::string poly_print(const Expression &x);
}

} // SymEngine

#ifdef HAVE_SYMENGINE_PIRANHA

#include <piranha/math.hpp>
#include <piranha/pow.hpp>
#include <piranha/print_coefficient.hpp>
namespace piranha
{
namespace math
{

template <typename T>
struct partial_impl<T, typename std::
                           enable_if<std::is_same<T, SymEngine::Expression>::
                                         value>::type> {
    /// Call operator.
    /**
     * @return an instance of Expression constructed from zero.
     */
    SymEngine::Expression operator()(const SymEngine::Expression &,
                                     const std::string &) const
    {
        return SymEngine::Expression(0);
    }
};

template <typename T, typename U>
struct pow_impl<T, U,
                typename std::
                    enable_if<std::is_same<T, SymEngine::Expression>::value
                              && std::is_integral<U>::value>::type> {
    SymEngine::Expression operator()(const SymEngine::Expression &x,
                                     const U &y) const
    {
        return SymEngine::pow(SymEngine::Expression(x).get_basic(),
                              SymEngine::integer(y));
    }
};
}

template <typename U>
struct print_coefficient_impl<U, typename std::
                                     enable_if<std::is_same<U, SymEngine::
                                                                   Expression>::
                                                   value>::type> {
    auto operator()(std::ostream &os, const U &cf) const -> decltype(os << cf)
    {
        return os << SymEngine::detail::poly_print(cf);
    }
};
}
#endif // HAVE_SYMENGINE_PIRANHA

#endif // SYMENGINE_EXPRESSION_H
