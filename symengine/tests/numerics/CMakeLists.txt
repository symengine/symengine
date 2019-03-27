project(test_numerics)

add_executable(test_optimizer test_optimizer)
target_link_libraries(test_optimizer symengine catch)
add_test(test_optimizer ${PROJECT_BINARY_DIR}/test_optimizer)
