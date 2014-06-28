#include "complex.h"

namespace CSymPy {

Complex::Complex(mpq_class real, mpq_class imaginary)
    : real_{real}, imaginary_{imaginary}
{
    throw std::runtime_error("Not implemented.");
}

bool Complex::is_canonical(const mpq_class &real, const mpq_class &imaginary)
{
	mpq_class re = real;
	mpq_class im = imaginary;
	re.canonicalize();
	im.canonicalize();
	// If 'im' is 0, it should not be Complex:
	if (im.get_num() == 0) return false;
	// if 'real' or `imaginary` are not in canonical form:
	if (re.get_num() != real.get_num()) return false;
	if (re.get_den() != real.get_den()) return false;
	if (im.get_num() != imaginary.get_num()) return false;
	if (im.get_den() != imaginary.get_den()) return false;
	return true;
}

} // CSymPy
