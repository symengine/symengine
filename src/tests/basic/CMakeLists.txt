project(test_basic)

add_executable(${PROJECT_NAME} test_basic.cpp)
target_link_libraries(${PROJECT_NAME} csympy ${LIBS})
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

add_executable(test_arit test_arit.cpp)
target_link_libraries(test_arit csympy ${LIBS})
add_test(test_arit ${PROJECT_BINARY_DIR}/test_arit)

add_executable(test_poly test_poly.cpp)
target_link_libraries(test_poly csympy ${LIBS})
add_test(test_poly ${PROJECT_BINARY_DIR}/test_poly)

add_executable(test_functions test_functions.cpp)
target_link_libraries(test_functions csympy ${LIBS})
add_test(test_functions ${PROJECT_BINARY_DIR}/test_functions)

add_executable(test_subs test_subs.cpp)
target_link_libraries(test_subs csympy ${LIBS})
add_test(test_subs ${PROJECT_BINARY_DIR}/test_subs)

add_executable(test_integer test_integer.cpp)
target_link_libraries(test_integer csympy ${LIBS})
add_test(test_integer ${PROJECT_BINARY_DIR}/test_integer)
