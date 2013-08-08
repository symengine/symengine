find_path(BFD_INCLUDE_DIR bfd.h)
find_library(BFD_LIBRARY bfd)

set(BFD_LIBRARIES ${BFD_LIBRARY})
set(BFD_INCLUDE_DIRS ${BFD_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD DEFAULT_MSG
    BFD_LIBRARY BFD_INCLUDE_DIR)

mark_as_advanced(BFD_INCLUDE_DIR BFD_LIBRARY)
