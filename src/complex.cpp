#include "complex.h"

namespace CSymPy {

Complex::Complex(mpq_class real, mpq_class imaginary)
    : real_{real}, imaginary_{imaginary}
{
    throw std::runtime_error("Not implemented.");
}

} // CSymPy
