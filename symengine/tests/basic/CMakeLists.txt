project(test_basic)

add_executable(${PROJECT_NAME} test_basic.cpp)
target_link_libraries(${PROJECT_NAME} symengine catch)
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

add_executable(test_arit test_arit.cpp)
target_link_libraries(test_arit symengine catch)
add_test(test_arit ${PROJECT_BINARY_DIR}/test_arit)

add_executable(test_poly test_poly.cpp)
target_link_libraries(test_poly symengine catch)
add_test(test_poly ${PROJECT_BINARY_DIR}/test_poly)

add_executable(test_polynomial test_polynomial.cpp)
target_link_libraries(test_polynomial symengine catch)
add_test(test_polynomial ${PROJECT_BINARY_DIR}/test_polynomial)

add_executable(test_series test_series.cpp)
target_link_libraries(test_series symengine catch)
add_test(test_series ${PROJECT_BINARY_DIR}/test_series)

add_executable(test_series_generic test_series_generic.cpp)
target_link_libraries(test_series_generic symengine catch)
add_test(test_series_generic ${PROJECT_BINARY_DIR}/test_series_generic)

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

add_executable(test_functions test_functions.cpp)
target_link_libraries(test_functions symengine catch)
add_test(test_functions ${PROJECT_BINARY_DIR}/test_functions)

add_executable(test_subs test_subs.cpp)
target_link_libraries(test_subs symengine catch)
add_test(test_subs ${PROJECT_BINARY_DIR}/test_subs)

add_executable(test_integer test_integer.cpp)
target_link_libraries(test_integer symengine catch)
add_test(test_integer ${PROJECT_BINARY_DIR}/test_integer)

add_executable(test_rational test_rational.cpp)
target_link_libraries(test_rational symengine catch)
add_test(test_rational ${PROJECT_BINARY_DIR}/test_rational)

add_executable(test_number test_number.cpp)
target_link_libraries(test_number symengine catch)
add_test(test_number ${PROJECT_BINARY_DIR}/test_number)

add_executable(test_as_numer_denom test_as_numer_denom.cpp)
target_link_libraries(test_as_numer_denom symengine catch)
add_test(test_as_numer_denom ${PROJECT_BINARY_DIR}/test_as_numer_denom)

add_executable(test_parser test_parser.cpp)
target_link_libraries(test_parser symengine catch)
add_test(test_parser ${PROJECT_BINARY_DIR}/test_parser)

add_executable(test_sets test_sets.cpp)
target_link_libraries(test_sets symengine catch)
add_test(test_sets ${PROJECT_BINARY_DIR}/test_sets)
