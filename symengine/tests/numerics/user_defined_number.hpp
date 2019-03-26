
#ifndef SYMENGINE_TESTS_NUMERICS_USER_DEFINED_NUMBER_HPP_
#define SYMENGINE_TESTS_NUMERICS_USER_DEFINED_NUMBER_HPP_

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/expression.h>
#include <symengine/number.h>
#include <symengine/symengine_assert.h>

#include <iostream>
#include <type_traits>
#include <array>

struct Vector {
    static const unsigned int dim = 3;

    Vector(double x, double y, double z) : values({{x, y, z}})
    {
    }
    Vector() = default;
    Vector(const Vector &) = default;
    Vector(Vector &&) = default;
    Vector &operator=(Vector rhs)
    {
        std::swap(values, rhs.values);
        return *this;
    }

    const std::array<double, dim> &get_values() const
    {
        return values;
    }

    Vector &operator+=(const Vector &rhs)
    {
        for (unsigned int d = 0; d < dim; ++d)
            values[d] += rhs.values[d];
        return *this;
    }

    template <typename T,
              typename
              = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    Vector &operator*=(const T &sf)
    {
        for (unsigned int d = 0; d < dim; ++d)
            values[d] *= sf;
        return *this;
    }

    double operator*(const Vector &rhs) const
    {
        double inner_product = 0.0;
        for (unsigned int d = 0; d < dim; ++d)
            inner_product += values[d] * rhs.values[d];
        return inner_product;
    }

    bool operator==(const Vector &rhs) const
    {
        for (unsigned int d = 0; d < dim; ++d)
            if (values[d] != rhs.values[d])
                return false;

        return true;
    }

    double l2_norm() const
    {
        double norm_squ = 0.0;
        for (unsigned int d = 0; d < dim; ++d)
            norm_squ += values[d] * values[d];
        return std::sqrt(norm_squ);
    }

private:
    std::array<double, dim> values;
};

Vector operator+(Vector lhs, const Vector &rhs)
{
    lhs += rhs;
    return lhs;
}

template <typename T,
          typename
          = typename std::enable_if<std::is_arithmetic<T>::value>::type>
Vector operator*(Vector lhs, const T &sf)
{
    lhs *= sf;
    return lhs;
}

template <typename T,
          typename
          = typename std::enable_if<std::is_arithmetic<T>::value>::type>
Vector operator*(const T &sf, Vector rhs)
{
    rhs *= sf;
    return rhs;
}

std::ostream &operator<<(std::ostream &stream, const Vector &vec)
{
    for (unsigned int d = 0; d < Vector::dim; ++d) {
        stream << vec.get_values()[d];
        if (d < Vector::dim - 1)
            stream << ",";
    }
    return stream;
}

template <typename VectorType>
class LAVectorWrapper;

template <typename VectorType>
bool is_a_LAVectorWrapper(const SymEngine::Basic &value)
{
    return dynamic_cast<const LAVectorWrapper<VectorType> *const>(&value);
}

template <typename VectorType>
inline SymEngine::RCP<const SymEngine::Basic>
make_rcp_for_LAVectorWrapper(const VectorType &vec, const std::string &name)
{
    return SymEngine::make_rcp<const LAVectorWrapper<VectorType>>(vec, name);
}

#define NOT_IMPLEMENTED_NUMBER_BOOL(func)                                      \
    virtual bool func() const                                                  \
    {                                                                          \
        return false;                                                          \
    }

#define NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(func)                         \
    virtual SymEngine::RCP<const SymEngine::Basic> func(                       \
        const SymEngine::Basic &other) const                                   \
    {                                                                          \
        throw SymEngine::NotImplementedError("Not Implemented");               \
        return SymEngine::real_double(0.0);                                    \
    }

#define NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(func)                        \
    virtual SymEngine::RCP<const SymEngine::Number> func(                      \
        const SymEngine::Number &other) const                                  \
    {                                                                          \
        throw SymEngine::NotImplementedError("Not Implemented");               \
        return SymEngine::real_double(0.0);                                    \
    }

template <typename VectorType>
class LAVectorWrapper : public SymEngine::NumberWrapper,
                        public SymEngine::Evaluate
{

private:
    VectorType value;

public:
    explicit LAVectorWrapper(const VectorType &vec) : value(vec)
    {
    }

    virtual ~LAVectorWrapper(){};

    explicit operator VectorType() const
    {
        return value;
    }

    // ==== METHODS FROM Basic ====

    virtual SymEngine::hash_t __hash__() const
    {
        return std::hash<std::string>{}(__str__());
    }

    virtual bool __eq__(const SymEngine::Basic &other) const
    {
        if (is_a_LAVectorWrapper<VectorType>(other)) {
            const LAVectorWrapper &op_other
                = SymEngine::down_cast<const LAVectorWrapper &>(other);
            return &(this->value) == &(op_other.value);
        }
        return false;
    }

    virtual int compare(const SymEngine::Basic &other) const
    {
        SYMENGINE_ASSERT_MSG(is_a_LAVectorWrapper<VectorType>(other),
                             "Not a LAVectorWrapper");

        const LAVectorWrapper &op_other
            = SymEngine::down_cast<const LAVectorWrapper &>(other);
        if (&(this->value) == &(op_other.value))
            return 0;
        return (&(this->value) < &(op_other.value)) ? -1 : 1;
    }

    // ==== METHODS FROM Number ====

    NOT_IMPLEMENTED_NUMBER_BOOL(is_exact);
    NOT_IMPLEMENTED_NUMBER_BOOL(is_positive);
    NOT_IMPLEMENTED_NUMBER_BOOL(is_negative);
    NOT_IMPLEMENTED_NUMBER_BOOL(is_one);
    NOT_IMPLEMENTED_NUMBER_BOOL(is_minus_one);
    NOT_IMPLEMENTED_NUMBER_BOOL(is_complex);

    virtual bool is_zero() const
    {
        return this->value.l2_norm() == 0.0;
    }

    virtual SymEngine::Evaluate &get_eval() const
    {
        return const_cast<LAVectorWrapper &>(*this);
    }

    // ==== METHODS FROM NumberWrapper ====

    virtual std::string __str__() const
    {
        std::stringstream ss;
        ss << "Vec[";
        ss << this->value;
        ss << "]";
        return ss.str();
    }

    virtual SymEngine::RCP<const SymEngine::Number> eval(long /*bits*/) const
    {
        throw SymEngine::NotImplementedError("Not Implemented");
        return SymEngine::real_double(0.0);
    }

    virtual SymEngine::RCP<const SymEngine::Number>
    add(const SymEngine::Number &other) const
    {
        if (is_a_LAVectorWrapper<VectorType>(other)) {
            const LAVectorWrapper &other_op
                = SymEngine::down_cast<const LAVectorWrapper &>(other);
            VectorType result(this->value);
            result += other_op.value;
            return SymEngine::make_rcp<const LAVectorWrapper>(result);
        } else if (SymEngine::is_a<SymEngine::Integer>(other)
                   && other.is_zero()) {
            return SymEngine::make_rcp<const LAVectorWrapper>(this->value);
        }

        throw SymEngine::NotImplementedError("Not Implemented");
        return other.add(*this);
    }

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(sub);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(rsub);

    virtual SymEngine::RCP<const SymEngine::Number>
    mul(const SymEngine::Number &other) const
    {
        if (is_a_LAVectorWrapper<VectorType>(other)) {
            const LAVectorWrapper &other_op
                = SymEngine::down_cast<const LAVectorWrapper &>(other);
            // Vector inner product returns a number
            return SymEngine::real_double(this->value * other_op.value);
        } else if (other.is_one()) {
            return SymEngine::make_rcp<const LAVectorWrapper>(this->value);
        } else if (SymEngine::is_a<SymEngine::RealDouble>(other)) {
            const SymEngine::RealDouble &other_nmbr
                = SymEngine::down_cast<const SymEngine::RealDouble &>(other);
            const double value = eval_double(other_nmbr);
            VectorType result(this->value);
            result *= value;
            return SymEngine::make_rcp<const LAVectorWrapper>(result);
        } else if (SymEngine::is_a<SymEngine::Integer>(other)) {
            const SymEngine::Integer &other_nmbr
                = SymEngine::down_cast<const SymEngine::Integer &>(other);
            const signed long int value = other_nmbr.as_int();
            VectorType result(this->value);
            result *= value;
            return SymEngine::make_rcp<const LAVectorWrapper>(result);
        } else if (SymEngine::is_a<SymEngine::Rational>(other)) {
            const SymEngine::Rational &other_nmbr
                = SymEngine::down_cast<const SymEngine::Rational &>(other);
            const double value
                = SymEngine::mp_get_d(other_nmbr.as_rational_class());
            VectorType result(this->value);
            result *= value;
            return SymEngine::make_rcp<const LAVectorWrapper>(result);
        }

        throw SymEngine::NotImplementedError("Not Implemented");
        return other.mul(*this);
    }

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(div);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(rdiv);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(pow);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_NUMBER(rpow);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(exp);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(log);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(sin);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(cos);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(tan);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(csc);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(sec);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(cot);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(asin);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(acos);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(atan);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(acsc);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(asec);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(acot);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(sinh);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(cosh);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(tanh);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(csch);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(sech);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(coth);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(asinh);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(acosh);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(atanh);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(acsch);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(asech);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(acoth);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(abs);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(floor);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(ceiling);

    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(erf);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(erfc);
    NOT_IMPLEMENTED_NUMBERWRAPPER_FUNC_BASIC(gamma);

}; // class LAVectorWrapper  Vector &

template <typename VectorType>
std::ostream &operator<<(std::ostream &stream,
                         const LAVectorWrapper<VectorType> &vec)
{
    stream << vec.__str__();
    return stream;
}

template <typename VectorType>
class LAVectorExpression : public SymEngine::Expression
{
public:
    LAVectorExpression() = default;
    LAVectorExpression(const VectorType &vec)
        : SymEngine::Expression::Expression(
              SymEngine::make_rcp<const LAVectorWrapper<VectorType>>(vec))
    {
    }
    template <class T, typename = typename std::enable_if<!std::is_same<T,VectorType>::value>::type>
    LAVectorExpression(
        const T n)
        : SymEngine::Expression::Expression(n)
    {
    }

    LAVectorExpression(const LAVectorExpression &) = default;
    LAVectorExpression(LAVectorExpression &&) = default;

    explicit operator VectorType() const
    {
        if (is_a_LAVectorWrapper<VectorType>(*get_basic())) {
            const LAVectorWrapper<VectorType> &la_vw
                = SymEngine::down_cast<const LAVectorWrapper<VectorType> &>(
                    *get_basic());
            return static_cast<VectorType>(la_vw);
        }

        throw SymEngine::NotImplementedError("Not Implemented");
        return VectorType();
    }
};

#include <symengine/numerics/number_visitor.h>
#include <symengine/numerics/optimizer.h>

namespace SymEngine
{
namespace Numerics
{
namespace internal
{
// Floating point numbers
template <typename ReturnType_, typename ExpressionType_>
struct DictionaryOptimizer<ReturnType_, ExpressionType_,
                           typename std::
                               enable_if<std::is_same<ReturnType_,
                                                      Vector>::value>::type> {
    typedef Vector ReturnType;
    typedef LAVectorExpression<Vector> ExpressionType;
    typedef DictionarySubstitutionVisitor<ReturnType, ExpressionType>
        VisitorType;

    static void print(std::ostream &stream, const VisitorType &optimizer,
                      const bool print_independent_symbols = false,
                      const bool print_dependent_expressions = false,
                      const bool print_cse_reductions = true)
    {
        optimizer.print(stream, print_independent_symbols,
                        print_dependent_expressions, print_cse_reductions);
    }
};

template <typename ReturnType_>
struct LambdaOptimizer<ReturnType_,
                       typename std::enable_if<std::is_same<ReturnType_,
                                                            Vector>::value>::
                           type> {
    typedef Vector ReturnType;
    typedef LAVectorExpression<Vector> ExpressionType;
    // For the sake of this test, we don't really want to define a
    // LambdaOptimizer, so we'll just create a dummy one that actually
    // invokes a DictionarySubstitutionVisitor.
    typedef DictionarySubstitutionVisitor<ReturnType, ExpressionType>
        VisitorType;

    static void print(std::ostream & /*stream*/,
                      const VisitorType & /*optimizer*/,
                      const bool /*print_independent_symbols*/ = false,
                      const bool /*print_dependent_expressions*/ = false,
                      const bool /*print_cse_reductions*/ = true)
    {
        // No built-in print function
    }
};

template <typename ReturnType_>
struct LLVMOptimizer<ReturnType_,
                     typename std::enable_if<std::is_same<ReturnType_, Vector>::
                                                 value>::type> {
    typedef Vector ReturnType;
    typedef LAVectorExpression<Vector> ExpressionType;
    // For the sake of this test, we don't really want to define a
    // LLVMOptimizer, so we'll just create a dummy one that actually
    // invokes a DictionarySubstitutionVisitor.
    typedef DictionarySubstitutionVisitor<ReturnType, ExpressionType>
        VisitorType;

    static void print(std::ostream & /*stream*/,
                      const VisitorType & /*optimizer*/,
                      const bool /*print_independent_symbols*/ = false,
                      const bool /*print_dependent_expressions*/ = false,
                      const bool /*print_cse_reductions*/ = true)
    {
        // No built-in print function
    }
};
}
}
}

#endif /* SYMENGINE_TESTS_NUMERICS_USER_DEFINED_NUMBER_HPP_ */
