project(test_rcp)

add_executable(${PROJECT_NAME} test_rcp.cpp)
target_link_libraries(${PROJECT_NAME} symengine catch)
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})

project(test_cooperative_intrusive_rcp)

add_executable(${PROJECT_NAME} test_cooperative_intrusive_rcp.cpp)
target_link_libraries(${PROJECT_NAME} symengine catch)
add_test(${PROJECT_NAME} ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
# The Python-owned test cases use fake hooks that cannot trigger deallocation,
# so ASAN/LSAN would report intentional leaks.  Disable leak detection.
set_tests_properties(${PROJECT_NAME} PROPERTIES
    ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0:symbolize=1")

# Regression guard: ownership gates must use the backend-neutral predicate,
# never use_count() == 1.
add_test(NAME check_use_count_eq1
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/check_use_count_eq1.sh)
