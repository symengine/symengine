macro (libfind_library LIBNAME PKG)

    find_library(${LIBNAME}_LIBRARY
        NAMES
            ${LIBNAME}
        PATHS
            ${${PKG}_LIB}
            ${${PKG}_LIB_DIR}
            ${${PKG}_LIBRARY}
            ${${PKG}_LIBRARIES}
        NO_DEFAULT_PATH
    )

    find_library(${LIBNAME}_LIBRARY
        NAMES
            ${LIBNAME}
        PATHS
            ${${PKG}_DIR}
            ${COMMON_DIR}/${PKG}
            ${CMAKE_INSTALL_PREFIX}
        PATH_SUFFIXES
            lib
            lib64
        NO_DEFAULT_PATH
    )

    find_library(${LIBNAME}_LIBRARY
        NAMES
            ${LIBNAME}
    )

endmacro()

macro (libfind_path HEADER PKG)

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
            ${COMMON_DIR}/${PKG}
            ${CMAKE_INSTALL_PREFIX}
        PATH_SUFFIXES
            include
        NO_DEFAULT_PATH
    )

    find_path(${PKG}_INCLUDE_DIR
        NAMES
            ${HEADER}
        PATH_SUFFIXES
            ${PKG}
    )
endmacro()
