#ifndef SYMENGINE_PRINTER_H
#define SYMENGINE_PRINTER_H

#include <symengine/basic.h>
#include <symengine/matrix.h>

namespace SymEngine
{
std::string str(const Basic &x);
std::string str(const DenseMatrix &x);
std::string unicode(const Basic &x);
std::string julia_str(const Basic &x);
std::string sbml(const Basic &x);
std::string ascii_art();

std::string mathml(const Basic &x);

std::string latex(const Basic &x);
std::string latex(const DenseMatrix &x, const int max_rows=24,
				  const int max_cols=16);

std::string ccode(const Basic &x);
std::string c89code(const Basic &x);
std::string c99code(const Basic &x);
std::string jscode(const Basic &x);
} // namespace SymEngine

#endif // SYMENGINE_PRINTER_H
