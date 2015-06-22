project(test_eval)

if (HAVE_SYMENGINE_MPFR)
    add_executable(test_eval_mpfr test_eval_mpfr.cpp)
    target_link_libraries(test_eval_mpfr symengine)
    add_test(test_eval_mpfr ${PROJECT_BINARY_DIR}/test_eval_mpfr)
endif()

if (HAVE_SYMENGINE_ARB)
    add_executable(test_eval_arb test_eval_arb.cpp)
    target_link_libraries(test_eval_arb symengine)
    add_test(test_eval_arb ${PROJECT_BINARY_DIR}/test_eval_arb)
endif()

if (HAVE_SYMENGINE_MPC)
    add_executable(test_eval_mpc test_eval_mpc.cpp)
    target_link_libraries(test_eval_mpc symengine)
    add_test(test_eval_mpc ${PROJECT_BINARY_DIR}/test_eval_mpc)
endif()

add_executable(test_eval_double test_eval_double.cpp)
target_link_libraries(test_eval_double symengine)
add_test(test_eval_double ${PROJECT_BINARY_DIR}/test_eval_double)
