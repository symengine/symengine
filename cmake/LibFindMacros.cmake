# Macros for finding external libraries
#
# Example usage:
#
#     include(LibFindMacros)
#     libfind_include(gmpxx.h gmp)
#     libfind_library(gmpxx gmp)
#     libfind_library(gmp gmp)
#     set(GMP_LIBRARIES ${GMPXX_LIBRARY} ${GMP_LIBRARY})
#     set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})
#     include(FindPackageHandleStandardArgs)
#     find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES
#         GMP_INCLUDE_DIRS)
#     mark_as_advanced(GMP_INCLUDE_DIR GMPXX_LIBRARY GMP_LIBRARY)
#

macro (libfind_library libname pkg)
    string(TOUPPER ${pkg} PKG)
    string(TOUPPER ${libname} LIBNAME)

    find_library(${LIBNAME}_LIBRARY
        NAMES
            ${libname}
        PATHS
            ${${PKG}_LIB}
            ${${PKG}_LIB_DIR}
            ${${PKG}_LIBRARY}
            ${${PKG}_LIBRARIES}
        NO_DEFAULT_PATH
    )

    find_library(${LIBNAME}_LIBRARY
        NAMES
            ${libname}
        PATHS
            ${${PKG}_DIR}
            ${COMMON_DIR}/${pkg}
        PATH_SUFFIXES
            lib
            lib64
        NO_DEFAULT_PATH
    )

    find_library(${LIBNAME}_LIBRARY
        NAMES
            ${libname}
    )

endmacro()

macro (libfind_path HEADER pkg)
    string(TOUPPER ${pkg} PKG)

    find_path(${PKG}_INCLUDE_DIR
        NAMES
            ${HEADER}
        PATHS
            ${${PKG}_INCLUDE}
            ${${PKG}_INCLUDE_DIRS}
        NO_DEFAULT_PATH
    )

    find_path(${PKG}_INCLUDE_DIR
        NAMES
            ${HEADER}
        PATHS
            ${${PKG}_DIR}
            ${COMMON_DIR}/${pkg}
        PATH_SUFFIXES
            include
        NO_DEFAULT_PATH
    )

    find_path(${PKG}_INCLUDE_DIR
        NAMES
            ${HEADER}
        PATH_SUFFIXES
            ${pkg}
    )
endmacro()
