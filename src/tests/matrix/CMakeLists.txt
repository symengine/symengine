project(test_matrix)

include_directories(${csympy_SOURCE_DIR}/src)

add_executable(test_matrix test_matrix.cpp)
target_link_libraries(test_matrix csympy teuchos ${LIBS})
add_test(test_matrix ${PROJECT_BINARY_DIR}/test_matrix)
