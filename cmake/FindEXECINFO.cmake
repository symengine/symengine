find_path(EXECINFO_INCLUDE_DIR execinfo.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARB DEFAULT_MSG ARB_INCLUDE_DIR ARB_LIBRARY)
if (CMAKE_SYSTEM_NAME MATCHES "BSD")
    find_library(EXECINFO_LIBRARY execinfo)
    find_package_handle_standard_args(EXECINFO DEFAULT_MSG
        EXECINFO_LIBRARY EXECINFO_INCLUDE_DIR)
else ()
    find_package_handle_standard_args(EXECINFO DEFAULT_MSG
        EXECINFO_INCLUDE_DIR)
endif ()
