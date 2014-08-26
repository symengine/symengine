find_path(GMP_INCLUDE_DIR gmpxx.h "${GMP_DIR}/include")
find_library(GMPXX gmpxx "${GMP_DIR}/lib")
find_library(GMP gmp "${GMP_DIR}/lib")
set(GMP_LIBRARIES ${GMPXX} ${GMP})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMPXX GMP)
