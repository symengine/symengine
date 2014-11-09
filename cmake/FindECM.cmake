find_path(ECM_INCLUDE_DIR ecm.h "${ECM_DIR}/include")
find_library(ECM_LIBRARY ecm "${ECM_DIR}/lib")
set(ECM_LIBRARIES ${ECM_LIBRARY})
set(ECM_INCLUDE_DIRS ${ECM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ECM DEFAULT_MSG ECM_LIBRARIES ECM_INCLUDE_DIR)

mark_as_advanced(ECM_INCLUDE_DIR ECM_LIBRARY)
