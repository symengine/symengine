project(test_diff)

add_executable(test_diff test_diff.cpp)
target_link_libraries(test_diff symengine catch)
add_test(test_diff ${PROJECT_BINARY_DIR}/test_diff)
