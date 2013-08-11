project(benchmarks)

include_directories(${csympy_SOURCE_DIR}/src)
include_directories(${teuchos_SOURCE_DIR})
include_directories(${teuchos_BINARY_DIR})

add_executable(expand1 expand1.cpp)
target_link_libraries(expand1 csympy teuchos ${LIBS})

add_executable(expand2 expand2.cpp)
target_link_libraries(expand2 csympy teuchos ${LIBS})
