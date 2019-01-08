project(matchpycpp)

include_directories(BEFORE ${symengine_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR})
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

#add_executable(output_test output_test.cpp)
#target_link_libraries(output_test symengine)

add_subdirectory(tests)
