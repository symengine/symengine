/**
 *  \file RealDouble.h
 *  Class for RealDouble built on top of Number class
 *
 **/
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

} // SymEngine
