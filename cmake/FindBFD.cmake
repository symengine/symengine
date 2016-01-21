find_path(BFD_INCLUDE_DIR bfd.h)
find_library(BFD_LIBRARY bfd)
find_library(IBERTY_LIBRARY iberty)
find_library(Z_LIBRARY z)
find_library(DL_LIBRARY dl)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD DEFAULT_MSG BFD_INCLUDE_DIR BFD_LIBRARY)
