find_path(MPFR_INCLUDE_DIR mpfr.h "${MPFR_DIR}/include")
find_library(MPFR_LIBRARY mpfr "${MPFR_DIR}/lib")
set(MPFR_LIBRARIES ${MPFR_LIBRARY})
set(MPFR_INCLUDE_DIRS ${MPFR_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_LIBRARIES MPFR_INCLUDE_DIR)

mark_as_advanced(MPFR_INCLUDE_DIR MPFR_LIBRARY)
