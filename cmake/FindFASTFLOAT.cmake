include(LibFindMacros)

libfind_include(fast_float/fast_float.h fastfloat)

set(FASTFLOAT_INCLUDE_DIRS ${FASTFLOAT_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FASTFLOAT DEFAULT_MSG FASTFLOAT_INCLUDE_DIRS)

mark_as_advanced(FASTFLOAT_INCLUDE_DIR)
