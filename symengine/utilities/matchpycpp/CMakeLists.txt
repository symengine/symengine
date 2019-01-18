project(matchpycpp)

include_directories(BEFORE ${symengine_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR})
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

set(SRC
)

set(HEADER
    bipartite.h
    common.h
    generator_trick.h
    many_to_one.h
    substitution.h
    util.h
)

add_subdirectory(tests)
add_subdirectory(generated_tests)

include_directories(BEFORE ${matchpycpp_BINARY_DIR})

#add_library(matchpycpp ${SRC})

#install(TARGETS matchpycpp
#EXPORT SymEngineTargets
#RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)
