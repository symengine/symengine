project(test_rcp)

include_directories(${csympy_SOURCE_DIR}/src)
include_directories(${csympy_SOURCE_DIR}/src/teuchos)
add_executable(${PROJECT_NAME} test_rcp.cpp)
target_link_libraries(${PROJECT_NAME} teuchos ${LIBS})
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

