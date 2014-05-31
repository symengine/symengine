find_path(LINKH_INCLUDE_DIR link.h)

set(LINKH_INCLUDE_DIRS ${LINKH_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LINKH DEFAULT_MSG
    LINKH_INCLUDE_DIR)

mark_as_advanced(LINKH_INCLUDE_DIR)
