#include <stdexcept>

#include "monomials.h"

namespace CSymPy {

void monomial_mul(const vec_int &A, const vec_int &B, vec_int &C)
{
    auto a = A.begin();
    auto b = B.begin();
    auto c = C.begin();
    for (; a != A.end(); ++a, ++b, ++c) {
        *c = *a + *b;
    }
}

} // CSymPy
