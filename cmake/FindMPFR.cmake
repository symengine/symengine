find_path(MPFR_INCLUDE_DIR mpfr.h $ENV{PYTHONHPC}/include)
find_library(MPFR mpfr $ENV{PYTHONHPC}/lib)
set(MPFR_LIBRARY ${MPFR})
set(MPFR_INCLUDE_DIRS ${MPFR_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_LIBRARY MPFR_INCLUDE_DIR)

mark_as_advanced(MPFR_INCLUDE_DIR MPFR)
