#ifndef CSYMPY_RCP_H
#define CSYMPY_RCP_H


#if defined(WITH_CSYMPY_RCP)

#else

// Include all Teuchos headers here:
#include "Teuchos_RCP.hpp"
#include "Teuchos_TypeNameTraits.hpp"

#endif

namespace CSymPy {


#if defined(WITH_CSYMPY_RCP)

#else

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;
using Teuchos::typeName;

#endif

} // CSymPy


#endif
