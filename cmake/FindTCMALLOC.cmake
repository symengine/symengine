find_library(TCMALLOC_LIBRARY tcmalloc)
if (NOT TCMALLOC_LIBRARY)
    find_library(TCMALLOC_MINIMAL_LIBRARY tcmalloc_minimal)
    set(TCMALLOC_LIBRARY ${TCMALLOC_MINIMAL_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TCMALLOC DEFAULT_MSG TCMALLOC_LIBRARY)
