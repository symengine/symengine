project(test_basic)

include_directories(${csympy_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} test_basic.cpp)
target_link_libraries(${PROJECT_NAME} csympy teuchos ${LIBS})
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

add_executable(test_arit test_arit.cpp)
target_link_libraries(test_arit csympy teuchos ${LIBS})
add_test(test_arit ${PROJECT_BINARY_DIR}/test_arit)

add_executable(test_poly test_poly.cpp)
target_link_libraries(test_poly csympy teuchos ${LIBS})
add_test(test_poly ${PROJECT_BINARY_DIR}/test_poly)
