project(test_basic)

add_executable(${PROJECT_NAME} test_basic.cpp)
target_link_libraries(${PROJECT_NAME} symengine catch)
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

set(SRC
    test_arit.cpp
    test_poly.cpp
    test_series.cpp
    test_series_generic.cpp
    test_functions.cpp
    test_subs.cpp
    test_integer.cpp
    test_rational.cpp
    test_relationals.cpp
    test_number.cpp
    test_as_numer_denom.cpp
    test_parser.cpp
    test_sets.cpp
    test_fields.cpp
    test_infinity.cpp
    test_nan.cpp
    test_solve.cpp
    test_as_real_imag.cpp
    test_cse.cpp
    test_count_ops.cpp
)
if (WITH_PIRANHA)
    set(SRC ${SRC}
        test_series_expansion_UP.cpp
        test_series_expansion_URatP.cpp
    )
endif()

if (WITH_FLINT)
    set(SRC ${SRC}
        test_series_expansion_URatF.cpp
    )
endif()

if ("${INTEGER_CLASS}" MATCHES "boostmp")
    set(SRC ${SRC}
        test_integer_class.cpp
    )
endif()


add_executable(tests_main ${SRC})
target_link_libraries(tests_main symengine catch)
add_test(tests_main ${PROJECT_BINARY_DIR}/tests_main)
