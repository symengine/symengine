add_subdirectory(teuchos)

include_directories(${csympy_SOURCE_DIR}/src/teuchos)

set(SRC
    add.cpp
    symbol.cpp
)

add_library(csympy ${SRC})

add_subdirectory(tests)
