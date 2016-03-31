/**
 *  \file RealDouble.h
 *  Class for RealDouble built on top of Number class
 *
 **/
#include <symengine/basic.h>
#include <symengine/real_double.h>
#include <symengine/complex_double.h>

namespace SymEngine
{

RealDouble::RealDouble(double i)
{
    this->i = i;
}

std::size_t RealDouble::__hash__() const
{
    std::hash<double> hash_fn;
    return hash_fn(i);
}

bool RealDouble::__eq__(const Basic &o) const
{
    if (is_a<RealDouble>(o)) {
        const RealDouble &s = static_cast<const RealDouble &>(o);
        return this->i == s.i;
    }
    return false;
}

int RealDouble::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<RealDouble>(o))
    const RealDouble &s = static_cast<const RealDouble &>(o);
    if (i == s.i)
        return 0;
    return i < s.i ? -1 : 1;
}

RCP<const RealDouble> real_double(double x)
{
    return make_rcp<const RealDouble>(x);
};

RCP<const Number> number(std::complex<double> x)
{
    return complex_double(x);
};
RCP<const Number> number(double x)
{
    return real_double(x);
};

//! Evaluate functions with double precision
template <class T>
class EvaluateDouble : public Evaluate
{
    virtual RCP<const Basic> sin(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::sin(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> cos(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::cos(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> tan(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::tan(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> cot(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(1.0 / std::tan(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> sec(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(1.0 / std::sin(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> csc(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(1.0 / std::cos(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> atan(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::atan(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> acot(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::atan(1.0 / static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> sinh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::sinh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> csch(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(1.0 / std::sinh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> cosh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::cosh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> sech(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(1.0 / std::cosh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> tanh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::tanh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> coth(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(1.0 / std::tanh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> asinh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::asinh(static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> acsch(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::asinh(1.0 / static_cast<const T &>(x).i));
    }
    virtual RCP<const Basic> abs(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<T>(x))
        return number(std::abs(static_cast<const T &>(x).i));
    }
};

class EvaluateRealDouble : public EvaluateDouble<RealDouble>
{
    virtual RCP<const Basic> gamma(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return number(std::tgamma(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> asin(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d <= 1.0 and d >= -1.0) {
            return number(std::asin(d));
        } else {
            return number(std::asin(std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> acos(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d <= 1.0 and d >= -1.0) {
            return number(std::acos(d));
        } else {
            return number(std::acos(std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> acsc(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d >= 1.0 or d <= -1.0) {
            return number(std::asin(1.0 / d));
        } else {
            return number(std::asin(1.0 / std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> asec(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d >= 1.0 or d <= -1.0) {
            return number(std::acos(1.0 / d));
        } else {
            return number(std::acos(1.0 / std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> acosh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d >= 1.0) {
            return number(std::acosh(d));
        } else {
            return number(std::acosh(std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> atanh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d <= 1.0 and d >= -1.0) {
            return number(std::atanh(d));
        } else {
            return number(std::atanh(std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> acoth(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d >= 1.0 or d <= -1.0) {
            return number(std::atanh(1.0 / d));
        } else {
            return number(std::atanh(1.0 / std::complex<double>(d)));
        }
    }
    virtual RCP<const Basic> log(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        double d = static_cast<const RealDouble &>(x).i;
        if (d >= 0.0) {
            return number(std::log(d));
        } else {
            return number(std::log(std::complex<double>(d)));
        }
    }
};

class EvaluateComplexDouble : public EvaluateDouble<ComplexDouble>
{
    virtual RCP<const Basic> gamma(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        throw std::runtime_error("Not Implemented.");
    }
    virtual RCP<const Basic> asin(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(std::asin(static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> acos(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(std::acos(static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> acsc(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(1.0 / std::asin(static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> asec(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(1.0 / std::acos(static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> acosh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(std::acosh(static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> atanh(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(std::atanh(static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> acoth(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(
            std::atanh(1.0 / static_cast<const ComplexDouble &>(x).i));
    }
    virtual RCP<const Basic> log(const Basic &x) const override
    {
        SYMENGINE_ASSERT(is_a<ComplexDouble>(x))
        return number(std::log(static_cast<const ComplexDouble &>(x).i));
    }
};

Evaluate &RealDouble::get_eval() const
{
    static EvaluateRealDouble evaluate_real_double;
    return evaluate_real_double;
}

Evaluate &ComplexDouble::get_eval() const
{
    static EvaluateComplexDouble evaluate_complex_double;
    return evaluate_complex_double;
}

} // SymEngine
