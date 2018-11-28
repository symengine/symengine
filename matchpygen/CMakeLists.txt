project(matchpygen)

include_directories(BEFORE ${symengine_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR})
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

add_executable(example example.cpp)
target_link_libraries(example symengine)

add_executable(example_yield example_yield.cpp)
target_link_libraries(example_yield symengine)
