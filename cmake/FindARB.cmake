find_path(ARB_INCLUDE_DIR fmprb.h)
find_library(ARB_LIBRARY arb)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARB DEFAULT_MSG ARB_INCLUDE_DIR ARB_LIBRARY)
