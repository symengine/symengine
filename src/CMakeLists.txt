add_subdirectory(teuchos)

# Include Teuchos headers:
include_directories(${teuchos_SOURCE_DIR})
# Include Teuchos config file (generated per build):
include_directories(${teuchos_BINARY_DIR})

set(SRC
    basic.cpp
    dict.cpp
    symbol.cpp
    integer.cpp
    add.cpp
    mul.cpp
    pow.cpp
    monomials.cpp
    rings.cpp
)

add_library(csympy ${SRC})

add_subdirectory(tests)
