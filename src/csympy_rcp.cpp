#include <cstddef>
#include <stdexcept>

#include "csympy_rcp.h"
#include "Teuchos_RCP.hpp"

namespace CSymPy {

void print_stack_on_segfault()
{
    Teuchos::print_stack_on_segfault();
}

} // CSymPy
