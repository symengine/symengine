include(LibFindMacros)

libfind_path(flint.h flint)
libfind_library(flint flint)

set(FLINT_LIBRARIES ${FLINT_LIBRARY})
set(FLINT_INCLUDE_DIRS ${FLINT_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLINT DEFAULT_MSG FLINT_LIBRARIES FLINT_INCLUDE_DIR)

mark_as_advanced(FLINT_INCLUDE_DIR flint_LIBRARY)
