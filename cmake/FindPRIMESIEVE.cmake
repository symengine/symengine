include(LibFindMacros)

libfind_header(primesieve.hpp primesieve)
libfind_library(primesieve primesieve)

set(PRIMESIEVE_LIBRARIES ${PRIMESIEVE_LIBRARY})
set(PRIMESIEVE_INCLUDE_DIRS ${PRIMESIEVE_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PRIMESIEVE DEFAULT_MSG
    PRIMESIEVE_LIBRARIES PRIMESIEVE_INCLUDE_DIRS)

mark_as_advanced(PRIMESIEVE_INCLUDE_DIR PRIMESIEVE_LIBRARY)
