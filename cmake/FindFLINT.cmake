find_path(FLINT_INCLUDE_DIR flint/flint.h "${FLINT_DIR}/include")
find_library(FLINT_LIBRARY flint "${FLINT_DIR}/lib")
set(FLINT_LIBRARIES ${FLINT_LIBRARY})
set(FLINT_INCLUDE_DIRS ${FLINT_INCLUDE_DIR}/flint)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLINT DEFAULT_MSG FLINT_LIBRARIES FLINT_INCLUDE_DIR)

mark_as_advanced(FLINT_INCLUDE_DIR FLINT_LIBRARY)
