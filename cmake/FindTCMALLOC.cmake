include(LibFindMacros)

libfind_library(tcmalloc tcmalloc)
if (NOT TCMALLOC_LIBRARY_FOUND)
    libfind_library(tcmalloc_minimal tcmalloc)
    set(TCMALLOC_LIBRARY ${TCMALLOC_MINIMAL_LIBRARY})
endif()
set(TCMALLOC_LIBRARIES ${TCMALLOC_LIBRARY})
find_package_handle_standard_args(TCMALLOC DEFAULT_MSG
    TCMALLOC_LIBRARIES)

mark_as_advanced(TCMALLOC_LIBRARY TCMALLOC_MINIMAL_LIBRARY)
