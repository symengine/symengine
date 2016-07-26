project(test_polynomial)

if (WITH_FLINT)
    add_executable(test_uintpoly_flint test_uintpoly_flint.cpp)
    target_link_libraries(test_uintpoly_flint symengine catch)
    add_test(test_uintpoly_flint ${PROJECT_BINARY_DIR}/test_uintpoly_flint)

    add_executable(test_uratpoly_flint test_uratpoly_flint.cpp)
    target_link_libraries(test_uratpoly_flint symengine catch)
    add_test(test_uratpoly_flint ${PROJECT_BINARY_DIR}/test_uratpoly_flint)
endif()

if (WITH_PIRANHA)
    add_executable(test_uintpoly_piranha test_uintpoly_piranha.cpp)
    target_link_libraries(test_uintpoly_piranha symengine catch)
    add_test(test_uintpoly_piranha ${PROJECT_BINARY_DIR}/test_uintpoly_piranha)

    add_executable(test_uratpoly_piranha test_uratpoly_piranha.cpp)
    target_link_libraries(test_uratpoly_piranha symengine catch)
    add_test(test_uratpoly_piranha ${PROJECT_BINARY_DIR}/test_uratpoly_piranha)
endif()