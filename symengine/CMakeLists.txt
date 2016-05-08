if (WITH_SYMENGINE_TEUCHOS)
    add_subdirectory(utilities/teuchos)
    # Include Teuchos headers:
    include_directories(BEFORE ${teuchos_SOURCE_DIR})
    # Include Teuchos config file (generated per build):
    include_directories(BEFORE ${teuchos_BINARY_DIR})
endif()

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
    series.cpp
    series_generic.cpp
    rings.cpp
    ntheory.cpp
    dense_matrix.cpp
    sparse_matrix.cpp
    matrix.cpp
    visitor.cpp
    eval_double.cpp
    diophantine.cpp
    cwrapper.cpp
    printer.cpp
    real_double.cpp
    complex_double.cpp
    expand.cpp
    expression.cpp
    numer_denom.cpp
    derivative.cpp
    parser.cpp
    mp_wrapper.cpp
    sets.cpp
)

if (WITH_MPFR)
    set(SRC ${SRC} eval_mpfr.cpp real_mpfr.cpp)
endif()

if (WITH_MPC)
    set(SRC ${SRC} eval_mpc.cpp complex_mpc.cpp)
endif()

if (WITH_FLINT)
    set(SRC ${SRC} series_flint.cpp)
endif()

if (WITH_ARB)
    set(SRC ${SRC} eval_arb.cpp)
endif()

if (WITH_PIRANHA)
    set(SRC series_piranha.cpp ${SRC})
endif()

# Needed for "make install"
set(HEADERS
    add.h        symengine_assert.h  functions.h  monomials.h  polynomial.h  number.h    rings.h
    basic.h      symengine_rcp.h     integer.h    mul.h        pow.h       constants.h
    symbol.h     expression.h        parser.h     mp_class.h   mp_wrapper.h
    basic-inl.h  dict.h           matrix.h     ntheory.h    rational.h complex.h
    visitor.h    eval_double.h    diophantine.h cwrapper.h  printer.h  real_double.h
    eval_mpfr.h  eval_arb.h       eval_mpc.h     complex_double.h         series_visitor.h
    real_mpfr.h  complex_mpc.h    type_codes.inc lambda_double.h series.h series_piranha.h
    basic-methods.inc   series_flint.h  series_generic.h sets.h  derivative.h   subs.h
)

# Configure SymEngine using our CMake options:
configure_file(symengine_config.h.in symengine_config.h)
# Include the config file:
include_directories(BEFORE ${symengine_BINARY_DIR})
# Include the source directory
include_directories(BEFORE ${symengine_SOURCE_DIR})

add_library(symengine ${SRC})
if (WITH_SYMENGINE_TEUCHOS)
    target_link_libraries(symengine teuchos)
endif()
target_link_libraries(symengine ${LIBS})

install(TARGETS symengine
            EXPORT SymEngineTargets
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
        )
install(FILES ${HEADERS} "${symengine_BINARY_DIR}/symengine/symengine_config.h"
    DESTINATION include/symengine)

if (BUILD_TESTS)
    add_subdirectory(utilities/catch)
    add_subdirectory(tests)
endif()
