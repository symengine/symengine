add_subdirectory(teuchos)
add_subdirectory(tests)

set(SRC
    add.cpp
    symbol.cpp
)

add_library(csympy ${SRC})
