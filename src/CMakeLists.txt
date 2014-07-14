add_subdirectory(teuchos)

# Include Teuchos headers:
include_directories(BEFORE ${teuchos_SOURCE_DIR})
# Include Teuchos config file (generated per build):
include_directories(BEFORE ${teuchos_BINARY_DIR})

set(SRC
    csympy_rcp.cpp
    basic.cpp
    dict.cpp
    symbol.cpp
    number.cpp
    integer.cpp
    rational.cpp
    complex.cpp
    add.cpp
    mul.cpp
    pow.cpp
    functions.cpp
    monomials.cpp
    rings.cpp
    ntheory.cpp
    dense_matrix.cpp
    sparse_matrix.cpp
    matrix.cpp
    visitor.cpp
)

# Needed for "make install"
set(HEADERS
    add.h        csympy_assert.h  functions.h  monomials.h  number.h    rings.h
    basic.h      csympy_rcp.h     integer.h    mul.h        pow.h
    symbol.h
    basic-inl.h  dict.h           matrix.h     ntheory.h    rational.h complex.h
    visitor.h
)

# Configure CSymPy using our CMake options:
configure_file(csympy_config.h.in csympy_config.h)
# Include the config file:
include_directories(BEFORE ${csympy_BINARY_DIR}/src)

add_library(csympy ${SRC})

install(TARGETS csympy
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
        )
install(FILES ${HEADERS} "${csympy_BINARY_DIR}/src/csympy_config.h"
    DESTINATION include)

add_subdirectory(tests)
