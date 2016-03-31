/**
 *  \file ComplexDouble.h
 *  Class for ComplexDouble built on top of Number class
 *
 **/
#include <symengine/basic.h>
#include <symengine/complex_double.h>

namespace SymEngine
{

ComplexDouble::ComplexDouble(std::complex<double> i)
{
    this->i = i;
}
//! Get the real part of the complex number
RCP<const Number> ComplexDouble::real_part() const
{
    return real_double(i.real());
}
//! Get the imaginary part of the complex number
RCP<const Number> ComplexDouble::imaginary_part() const
{
    return real_double(i.imag());
}
std::size_t ComplexDouble::__hash__() const
{
    std::size_t seed = COMPLEX_DOUBLE;
    hash_combine<double>(seed, i.real());
    hash_combine<double>(seed, i.imag());
    return seed;
}

bool ComplexDouble::__eq__(const Basic &o) const
{
    if (is_a<ComplexDouble>(o)) {
        const ComplexDouble &s = static_cast<const ComplexDouble &>(o);
        return this->i == s.i;
    }
    return false;
}

int ComplexDouble::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ComplexDouble>(o))
    const ComplexDouble &s = static_cast<const ComplexDouble &>(o);
    if (i == s.i)
        return 0;
    if (i.real() == s.i.real()) {
        return i.imag() < s.i.imag() ? -1 : 1;
    }
    return i.real() < s.i.real() ? -1 : 1;
}

RCP<const ComplexDouble> complex_double(std::complex<double> x)
{
    return make_rcp<const ComplexDouble>(x);
};

} // SymEngine
