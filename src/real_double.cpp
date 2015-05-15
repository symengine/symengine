/**
 *  \file RealDouble.h
 *  Class for RealDouble built on top of Number class
 *
 **/
#include <cmath>
#include "basic.h"
#include "real_double.h"

namespace SymEngine {

RealDouble::RealDouble(double i) {
    this->i = i;
}

std::size_t RealDouble::__hash__() const
{
    std::hash<double> hash_fn;
    // only the least significant bits that fit into "signed long int" are
    // hashed:
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
    if (i == s.i) return 0;
    return i < s.i ? -1 : 1;
}

RCP<const RealDouble> real_double(double x) { return rcp(new RealDouble(x)); };

//! Evaluate functions with double precision
class EvaluateDouble : public Evaluate {
    virtual RCP<const Basic> sin(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::sin(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> cos(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::cos(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> tan(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::tan(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> cot(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(1/std::tan(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> sec(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(1/std::sin(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> csc(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(1/std::cos(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> asin(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::asin(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> acos(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::acos(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> atan(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::atan(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> acot(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::atan(1/static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> asec(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::acos(1/static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> acsc(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::asin(1/static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> sinh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::sinh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> cosh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::cosh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> tanh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::tanh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> coth(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(1/std::tanh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> asinh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::asinh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> acosh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::acosh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> atanh(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::atanh(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> acoth(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::atanh(1/static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> log(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::log(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> gamma(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::tgamma(static_cast<const RealDouble &>(x).i));
    }
    virtual RCP<const Basic> abs(const Basic &x) const {
        SYMENGINE_ASSERT(is_a<RealDouble>(x))
        return real_double(std::abs(static_cast<const RealDouble &>(x).i));
    }
};

Evaluate& RealDouble::get_eval() const {
    static EvaluateDouble evaluate_double;
    return evaluate_double;
}
} // SymEngine
