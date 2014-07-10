project(test_ntheory)

add_executable(test_ntheory test_ntheory.cpp)
target_link_libraries(test_ntheory csympy teuchos ${LIBS})
add_test(test_ntheory ${PROJECT_BINARY_DIR}/test_ntheory)
