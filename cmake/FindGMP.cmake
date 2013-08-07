find_path(GMP_INCLUDE_DIR gmpxx.h $ENV{PYTHONHPC}/include)
find_library(GMPXX gmpxx $ENV{PYTHONHPC}/lib)
find_library(GMP gmp $ENV{PYTHONHPC}/lib)
set(GMP_LIBRARIES ${GMPXX} ${GMP})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMPXX GMP)
