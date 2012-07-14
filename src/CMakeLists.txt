add_subdirectory(teuchos)

include_directories(${csympy_SOURCE_DIR}/src/teuchos)

set(SRC
    basic.cpp
    dict.cpp
    symbol.cpp
    integer.cpp
    add.cpp
    mul.cpp
)

add_library(csympy ${SRC})

add_subdirectory(tests)
