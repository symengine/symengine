find_path(ECM_INCLUDE_DIR ecm.h $ENV{PYTHONHPC}/include)
find_library(ECM ecm $ENV{PYTHONHPC}/lib)
set(ECM_LIBRARY ${ECM})
set(ECM_INCLUDE_DIRS ${ECM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ECM DEFAULT_MSG ECM_LIBRARY ECM_INCLUDE_DIR)

mark_as_advanced(ECM_INCLUDE_DIR ECM)
