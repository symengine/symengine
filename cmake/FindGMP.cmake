include(LibFindMacros)

libfind_include(gmp.h gmp)
libfind_library(mpirxx-x86-v120-mt-sgd gmp)
libfind_library(mpir-x86-v120-mt-sgd gmp)

set(GMP_LIBRARIES ${GMPXX_LIBRARY} ${GMP_LIBRARY})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES
    GMP_INCLUDE_DIRS)

mark_as_advanced(GMP_INCLUDE_DIR GMPXX_LIBRARY GMP_LIBRARY)
