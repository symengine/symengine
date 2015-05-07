project(test_eval)

if (HAVE_SYMENGINE_ARB)
    add_executable(test_eval_arb test_eval_arb.cpp)
    target_link_libraries(test_eval_arb symengine)
    add_test(test_eval_arb ${PROJECT_BINARY_DIR}/test_eval_arb)
endif()

add_executable(test_eval_double test_eval_double.cpp)
target_link_libraries(test_eval_double symengine)
add_test(test_eval_double ${PROJECT_BINARY_DIR}/test_eval_double)
