project(test_basic)

add_executable(${PROJECT_NAME} test_basic.cpp)
target_link_libraries(${PROJECT_NAME} symengine catch)
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

add_executable(tests_main
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
target_link_libraries(tests_main symengine catch)
add_test(tests_main ${PROJECT_BINARY_DIR}/tests_main)

if (WITH_PIRANHA)
    add_executable(test_series_expansion_UP test_series_expansion_UP.cpp)
    target_link_libraries(test_series_expansion_UP symengine catch)
    add_test(test_series_expansion_UP ${PROJECT_BINARY_DIR}/test_series_expansion_UP)

    add_executable(test_series_expansion_URatP test_series_expansion_URatP.cpp)
    target_link_libraries(test_series_expansion_URatP symengine catch)
    add_test(test_series_expansion_URatP ${PROJECT_BINARY_DIR}/test_series_expansion_URatP)
endif()

if (WITH_FLINT)
    add_executable(test_series_expansion_URatF test_series_expansion_URatF.cpp)
    target_link_libraries(test_series_expansion_URatF symengine catch)
    add_test(test_series_expansion_URatF ${PROJECT_BINARY_DIR}/test_series_expansion_URatF)
endif()

if ("${INTEGER_CLASS}" MATCHES "boostmp")
	add_executable(test_integer_class test_integer_class.cpp)
	target_link_libraries(test_integer_class symengine catch)
	add_test(test_integer_class ${PROJECT_BINARY_DIR}/test_integer_class)
endif()
