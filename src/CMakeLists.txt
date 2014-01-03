add_subdirectory(teuchos)

# Include Teuchos headers:
include_directories(${teuchos_SOURCE_DIR})
# Include Teuchos config file (generated per build):
include_directories(${teuchos_BINARY_DIR})

set(SRC
    csympy_rcp.cpp
    basic.cpp
    dict.cpp
    symbol.cpp
    number.cpp
    integer.cpp
    rational.cpp
    add.cpp
    mul.cpp
    pow.cpp
    functions.cpp
    monomials.cpp
    rings.cpp
    ntheory.cpp
)

# Configure CSymPy using our CMake options:
configure_file(csympy_config.h.in csympy_config.h)
# Include the config file:
include_directories(${csympy_BINARY_DIR}/src)

add_library(csympy ${SRC})

add_subdirectory(tests)
