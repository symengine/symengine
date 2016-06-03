find_path(ECM_INCLUDE_DIR ecm.h)
find_library(ECM_LIBRARY ecm)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ECM DEFAULT_MSG ECM_INCLUDE_DIR ECM_LIBRARY)
