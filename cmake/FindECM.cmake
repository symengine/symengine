include(LibFindMacros)

libfind_path(ecm.h ecm)
libfind_library(ecm ecm)

set(ECM_LIBRARIES ${ECM_LIBRARY})
set(ECM_INCLUDE_DIRS ${ECM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ECM DEFAULT_MSG ECM_LIBRARIES
    ECM_INCLUDE_DIRS)

mark_as_advanced(ECM_INCLUDE_DIR ECM_LIBRARY)
