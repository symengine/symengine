add_subdirectory(teuchos)

include_directories(${csympy_SOURCE_DIR}/src/teuchos)

set(SRC
    basic.cpp
    add.cpp
    symbol.cpp
)

add_library(csympy ${SRC})

add_subdirectory(tests)
