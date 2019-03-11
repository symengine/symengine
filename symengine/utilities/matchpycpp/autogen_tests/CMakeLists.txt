project(matchpycpp_tests)
include_directories(BEFORE ${catch_SOURCE_DIR})

add_executable(test_case001 test_case001.cpp)
target_link_libraries(test_case001 symengine catch)
add_test(test_case001 test_case001)

add_executable(test_case002 test_case002.cpp)
target_link_libraries(test_case002 symengine catch)
add_test(test_case002 test_case002)

add_executable(test_case003 test_case003.cpp)
target_link_libraries(test_case003 symengine catch)
add_test(test_case003 test_case003)

add_executable(test_case004 test_case004.cpp)
target_link_libraries(test_case004 symengine catch)
add_test(test_case004 test_case004)

add_executable(test_case005 test_case005.cpp)
target_link_libraries(test_case005 symengine catch)
add_test(test_case005 test_case005)

add_executable(test_case006 test_case006.cpp)
target_link_libraries(test_case006 symengine catch)
add_test(test_case006 test_case006)

add_executable(test_case007 test_case007.cpp)
target_link_libraries(test_case007 symengine catch)
add_test(test_case007 test_case007)
