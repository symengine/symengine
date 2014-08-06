find_path(ARB_INCLUDE_DIR fmprb.h $ENV{PYTHONHPC}/include)
find_library(ARB arb $ENV{PYTHONHPC}/lib)
set(ARB_LIBRARY ${ARB})
set(ARB_INCLUDE_DIRS ${ARB_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARB DEFAULT_MSG ARB_LIBRARY ARB_INCLUDE_DIR)

mark_as_advanced(ARB_INCLUDE_DIR ARB)