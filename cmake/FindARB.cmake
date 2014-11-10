include(LibFindMacros)

libfind_header(fmprb.h arb)
libfind_library(arb arb)

set(ARB_LIBRARIES ${ARB_LIBRARY})
set(ARB_INCLUDE_DIRS ${ARB_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARB DEFAULT_MSG ARB_LIBRARIES ARB_INCLUDE_DIRS)

mark_as_advanced(ARB_INCLUDE_DIR ARB_LIBRARY)
