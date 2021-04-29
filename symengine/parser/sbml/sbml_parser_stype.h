#ifndef SYMENGINE_PARSER_SBML_STYPE_H
#define SYMENGINE_PARSER_SBML_STYPE_H

#include <string>
#include "symengine/basic.h"

namespace SymEngine
{

struct SBMLSTYPE {
    SymEngine::RCP<const SymEngine::Basic> basic;
    SymEngine::vec_basic basic_vec;
    std::string string;
    // Constructor
    SBMLSTYPE() = default;
    // Destructor
    ~SBMLSTYPE() = default;
    // Copy constructor and assignment
    SBMLSTYPE(const SBMLSTYPE &) = default;
    SBMLSTYPE &operator=(const SBMLSTYPE &) = default;
    // Move constructor and assignment
    SBMLSTYPE(SBMLSTYPE &&) = default;
    SBMLSTYPE &operator=(SBMLSTYPE &&) = default;
};

} // namespace SymEngine

#endif
