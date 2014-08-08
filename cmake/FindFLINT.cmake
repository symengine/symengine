find_path(FLINT_INCLUDE_DIR flint/flint.h $ENV{PYTHONHPC}/include)
find_library(FLINT flint $ENV{PYTHONHPC}/lib)
set(FLINT_LIBRARY ${FLINT})
set(FLINT_INCLUDE_DIRS ${FLINT_INCLUDE_DIR}/flint)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLINT DEFAULT_MSG FLINT_LIBRARY FLINT_INCLUDE_DIR)

mark_as_advanced(FLINT_INCLUDE_DIR FLINT)
