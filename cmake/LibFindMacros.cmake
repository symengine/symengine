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
