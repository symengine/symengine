find_path(ARB_INCLUDE_DIR fmprb.h "${ARB_DIR}/include")
find_library(ARB_LIBRARY arb "${ARB_DIR}/lib")
set(ARB_LIBRARIES ${ARB_LIBRARY})
set(ARB_INCLUDE_DIRS ${ARB_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARB DEFAULT_MSG ARB_LIBRARIES ARB_INCLUDE_DIR)

mark_as_advanced(ARB_INCLUDE_DIR ARB_LIBRARY)
