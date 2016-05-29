project(test_polynomial)

add_executable(test_uintpoly test_uintpoly.cpp)
target_link_libraries(test_uintpoly symengine catch)
add_test(test_uintpoly ${PROJECT_BINARY_DIR}/test_uintpoly)

add_executable(test_mintpoly test_mintpoly.cpp)
target_link_libraries(test_mintpoly symengine catch)
add_test(test_mintpoly ${PROJECT_BINARY_DIR}/test_mintpoly)

add_executable(test_uexprpoly test_uexprpoly.cpp)
target_link_libraries(test_uexprpoly symengine catch)
add_test(test_uexprpoly ${PROJECT_BINARY_DIR}/test_uexprpoly)

add_executable(test_mexprpoly test_mexprpoly.cpp)
target_link_libraries(test_mexprpoly symengine catch)
add_test(test_mexprpoly ${PROJECT_BINARY_DIR}/test_mexprpoly)