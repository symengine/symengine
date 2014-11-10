include(LibFindMacros)

libfind_path(gmpxx.h gmp)
libfind_library(gmpxx gmp)
libfind_library(gmp gmp)

set(GMP_LIBRARIES ${GMPXX_LIBRARY} ${GMP_LIBRARY})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMPXX_LIBRARY GMP_LIBRARY)
