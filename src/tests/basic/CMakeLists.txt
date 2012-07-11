project(test-basic)

include_directories(${csympy_SOURCE_DIR}/src/teuchos)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} teuchos)
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

