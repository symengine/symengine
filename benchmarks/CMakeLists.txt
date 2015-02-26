project(benchmarks)

include_directories(BEFORE ${csympy_SOURCE_DIR}/src)
include_directories(BEFORE ${csympy_BINARY_DIR}/src)
include_directories(BEFORE ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${teuchos_BINARY_DIR})

add_executable(expand1 expand1.cpp)
target_link_libraries(expand1 csympy)

add_executable(eval_double1 eval_double1.cpp)
target_link_libraries(eval_double1 csympy)

add_executable(expand2 expand2.cpp)
target_link_libraries(expand2 csympy)

add_executable(expand2b expand2b.cpp)
target_link_libraries(expand2b csympy)

add_executable(expand3 expand3.cpp)
target_link_libraries(expand3 csympy)

add_executable(add1 add1.cpp)
target_link_libraries(add1 csympy)

add_executable(matrix_add1 matrix_add1.cpp)
target_link_libraries(matrix_add1 csympy)

add_executable(matrix_add2 matrix_add2.cpp)
target_link_libraries(matrix_add2 csympy)

add_executable(matrix_mul1 matrix_mul1.cpp)
target_link_libraries(matrix_mul1 csympy)

add_executable(matrix_mul2 matrix_mul2.cpp)
target_link_libraries(matrix_mul2 csympy)

add_executable(symbench symbench.cpp)
target_link_libraries(symbench csympy)
