project(matchpycpp_tests)

include_directories(BEFORE ${symengine_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR})
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

include_directories(BEFORE ${catch_SOURCE_DIR})

add_executable(test_case001 test_case001.cpp)
target_link_libraries(test_case001 symengine catch)

add_executable(test_case002 test_case002.cpp)
target_link_libraries(test_case002 symengine catch)

add_executable(test_case003 test_case003.cpp)
target_link_libraries(test_case003 symengine catch)

add_executable(test_case004 test_case004.cpp)
target_link_libraries(test_case004 symengine catch)

add_executable(test_case005 test_case005.cpp)
target_link_libraries(test_case005 symengine catch)
