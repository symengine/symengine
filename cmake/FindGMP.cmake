include(FindPackageHandleStandardArgs)

find_path(GMP_INCLUDE_DIR gmp.h)
find_library(GMP_LIBRARY gmp)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_INCLUDE_DIR GMP_LIBRARY)

find_path(GMPXX_INCLUDE_DIR gmpxx.h)
find_library(GMPXX_LIBRARY gmpxx)
find_package_handle_standard_args(GMPXX DEFAULT_MSG GMPXX_INCLUDE_DIR GMPXX_LIBRARY)
