include(LibFindMacros)

libfind_path(gmpxx.h GMP)
libfind_library(gmpxx GMP)
libfind_library(gmp GMP)

set(GMP_LIBRARIES ${gmpxx_LIBRARY} ${gmp_LIBRARY})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR gmpxx_LIBRARY gmp_LIBRARY)
