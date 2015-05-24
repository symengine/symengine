#include <cstddef>
#include <stdexcept>

#include "symengine_rcp.h"
#include "Teuchos_RCP.hpp"

namespace SymEngine {

#if defined(WITH_SYMENGINE_RCP)


void print_stack_on_segfault()
{
    Teuchos::print_stack_on_segfault();
}

#endif

} // SymEngine
