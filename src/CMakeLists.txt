add_subdirectory(teuchos)

# Include Teuchos headers:
include_directories(BEFORE ${teuchos_SOURCE_DIR})
# Include Teuchos config file (generated per build):
include_directories(BEFORE ${teuchos_BINARY_DIR})

set(SRC
    symengine_rcp.cpp
    basic.cpp
    dict.cpp
    symbol.cpp
    number.cpp
    integer.cpp
    rational.cpp
    complex.cpp
    constants.cpp
    add.cpp
    mul.cpp
    pow.cpp
    functions.cpp
    monomials.cpp
    polynomial.cpp
    rings.cpp
    ntheory.cpp
    dense_matrix.cpp
    sparse_matrix.cpp
    matrix.cpp
    visitor.cpp
    eval_double.cpp
    eval_arb.cpp
    diophantine.cpp
    cwrapper.cpp
    printer.cpp
    real_double.cpp
)

# Needed for "make install"
set(HEADERS
    add.h        symengine_assert.h  functions.h  monomials.h  polynomial.h  number.h    rings.h
    basic.h      symengine_rcp.h     integer.h    mul.h        pow.h       constants.h
    symbol.h
    basic-inl.h  dict.h           matrix.h     ntheory.h    rational.h complex.h
    visitor.h    eval_double.h    diophantine.h cwrapper.h  printer.h  real_double.h
)

# Configure SymEngine using our CMake options:
configure_file(symengine_config.h.in symengine_config.h)
# Include the config file:
include_directories(BEFORE ${symengine_BINARY_DIR}/src)

add_library(symengine ${SRC})
target_link_libraries(symengine teuchos ${LIBS})

install(TARGETS symengine
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
        )
install(FILES ${HEADERS} "${symengine_BINARY_DIR}/src/symengine_config.h"
    DESTINATION include)

add_subdirectory(tests)
