project(benchmarks)

include_directories(${csympy_SOURCE_DIR}/src)
include_directories(${teuchos_SOURCE_DIR})

add_executable(expand1 expand1.cpp)
target_link_libraries(expand1 csympy teuchos ${LIBS})
