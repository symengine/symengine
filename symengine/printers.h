#ifndef SYMENGINE_PRINTER_H
#define SYMENGINE_PRINTER_H

#include <cstdint>

#include <symengine/basic.h>
#include <symengine/matrix.h>

namespace SymEngine
{
enum class CodePrinterPrecision : std::uint32_t { Double = 0, Float = 1 };

SYMENGINE_EXPORT std::string str(const Basic &x);
SYMENGINE_EXPORT std::string str(const DenseMatrix &x);
SYMENGINE_EXPORT std::string unicode(const Basic &x);
SYMENGINE_EXPORT std::string julia_str(const Basic &x);
SYMENGINE_EXPORT std::string sbml(const Basic &x);
SYMENGINE_EXPORT std::string ascii_art();

SYMENGINE_EXPORT std::string mathml(const Basic &x);

SYMENGINE_EXPORT std::string latex(const Basic &x);
SYMENGINE_EXPORT std::string latex(const DenseMatrix &x,
                                   const unsigned max_rows = 20,
                                   const unsigned max_cols = 12);

SYMENGINE_EXPORT
std::string ccode(const Basic &x, CodePrinterPrecision precision
                                  = CodePrinterPrecision::Double);

SYMENGINE_EXPORT
std::string cudacode(const Basic &x, CodePrinterPrecision precision
                                     = CodePrinterPrecision::Double);
SYMENGINE_EXPORT std::string c89code(const Basic &x);
SYMENGINE_EXPORT std::string c99code(const Basic &x);
SYMENGINE_EXPORT std::string jscode(const Basic &x);
} // namespace SymEngine

#endif // SYMENGINE_PRINTER_H
