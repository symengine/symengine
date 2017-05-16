project(test_polynomial)

add_executable(test_upoly_int_symengine test_upoly_int_symengine.cpp)
target_link_libraries(test_upoly_int_symengine symengine catch)
add_test(test_upoly_int_symengine ${PROJECT_BINARY_DIR}/test_upoly_int_symengine)

add_executable(test_upoly_rat_symengine test_upoly_rat_symengine.cpp)
target_link_libraries(test_upoly_rat_symengine symengine catch)
add_test(test_upoly_rat_symengine ${PROJECT_BINARY_DIR}/test_upoly_rat_symengine)

add_executable(test_mpoly_int_symengine test_mpoly_int_symengine.cpp)
target_link_libraries(test_mpoly_int_symengine symengine catch)
add_test(test_mpoly_int_symengine ${PROJECT_BINARY_DIR}/test_mpoly_int_symengine)

add_executable(test_upoly_expr_symengine test_upoly_expr_symengine.cpp)
target_link_libraries(test_upoly_expr_symengine symengine catch)
add_test(test_upoly_expr_symengine ${PROJECT_BINARY_DIR}/test_upoly_expr_symengine)

add_executable(test_mpoly_expr_symengine test_mpoly_expr_symengine.cpp)
target_link_libraries(test_mpoly_expr_symengine symengine catch)
add_test(test_mpoly_expr_symengine ${PROJECT_BINARY_DIR}/test_mpoly_expr_symengine)

add_executable(test_basic_conversions test_basic_conversions.cpp)
target_link_libraries(test_basic_conversions symengine catch)
add_test(test_basic_conversions ${PROJECT_BINARY_DIR}/test_basic_conversions)

if (WITH_FLINT)
    add_executable(test_upoly_int_flint test_upoly_int_flint.cpp)
    target_link_libraries(test_upoly_int_flint symengine catch)
    add_test(test_upoly_int_flint ${PROJECT_BINARY_DIR}/test_upoly_int_flint)

    add_executable(test_upoly_rat_flint test_upoly_rat_flint.cpp)
    target_link_libraries(test_upoly_rat_flint symengine catch)
    add_test(test_upoly_rat_flint ${PROJECT_BINARY_DIR}/test_upoly_rat_flint)
endif()

if (WITH_PIRANHA)
    add_executable(test_upoly_int_piranha test_upoly_int_piranha.cpp)
    target_link_libraries(test_upoly_int_piranha symengine catch)
    add_test(test_upoly_int_piranha ${PROJECT_BINARY_DIR}/test_upoly_int_piranha)

    add_executable(test_upoly_rat_piranha test_upoly_rat_piranha.cpp)
    target_link_libraries(test_upoly_rat_piranha symengine catch)
    add_test(test_upoly_rat_piranha ${PROJECT_BINARY_DIR}/test_upoly_rat_piranha)
endif()