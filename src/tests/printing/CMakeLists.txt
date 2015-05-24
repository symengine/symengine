project(test_printing)

add_executable(test_printing test_printing.cpp)
target_link_libraries(test_printing symengine)
add_test(test_printing ${PROJECT_BINARY_DIR}/test_printing)
