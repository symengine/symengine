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
    polys/uintpoly.cpp
    polys/uratpoly.cpp
    polys/uexprpoly.cpp
    polys/basic_conversions.cpp
    polys/msymenginepoly.cpp
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
    sets.cpp
    eval.cpp
    fields.cpp
    infinity.cpp
    logic.cpp
    finitediff.cpp
    nan.cpp
    solve.cpp
    rewrite.cpp
    cse.cpp
)

if ("${SYMENGINE_INTEGER_CLASS}" STREQUAL "BOOSTMP")
    set(SRC ${SRC} mp_boost.cpp)
elseif ("${SYMENGINE_INTEGER_CLASS}" STREQUAL "FLINT")
    set(SRC ${SRC} mp_wrapper.cpp)
elseif ("${SYMENGINE_INTEGER_CLASS}" STREQUAL "GMP")
    set(SRC ${SRC} mp_wrapper.cpp)
endif()

if (WITH_MPFR)
    set(SRC ${SRC} eval_mpfr.cpp real_mpfr.cpp)
endif()

if (WITH_MPC)
    set(SRC ${SRC} eval_mpc.cpp complex_mpc.cpp)
endif()

if (WITH_FLINT)
    set(SRC series_flint.cpp ${SRC})
    set(SRC polys/uintpoly_flint.cpp ${SRC})
endif()

if (WITH_ARB)
    set(SRC ${SRC} eval_arb.cpp)
endif()

if (WITH_PIRANHA)
    set(SRC series_piranha.cpp ${SRC})
    set(SRC polys/uintpoly_piranha.cpp ${SRC})
endif()

if (WITH_LLVM)
    set(SRC ${SRC} llvm_double.cpp)
endif()

# Needed for "make install"
set(HEADERS
    add.h
    basic.h
    basic-inl.h
    basic-methods.inc
    codegen.h
    complex_double.h
    complex.h
    complex_mpc.h
    constants.h
    cwrapper.h
    derivative.h
    dict.h
    diophantine.h
    eval_arb.h
    eval_double.h
    eval.h
    eval_mpc.h
    eval_mpfr.h
    expression.h
    fields.h
    finitediff.h
    flint_wrapper.h
    functions.h
    infinity.h
    integer.h
    lambda_double.h
    llvm_double.h
    logic.h
    matrix.h
    monomials.h
    mp_class.h
    mp_wrapper.h
    mul.h
    nan.h
    ntheory.h
    number.h
    parser.h
    polys/basic_conversions.h
    polys/uexprpoly.h
    polys/uintpoly_flint.h
    polys/uintpoly.h
    polys/uintpoly_piranha.h
    polys/upolybase.h
    polys/uratpoly.h
    polys/usymenginepoly.h
    polys/msymenginepoly.h
    pow.h
    printer.h
    rational.h
    real_double.h
    real_mpfr.h
    rings.h
    series_flint.h
    series_generic.h
    series.h
    series_piranha.h
    series_visitor.h
    sets.h
    subs.h
    symbol.h
    symengine_assert.h
    symengine_casts.h
    symengine_exception.h
    symengine_rcp.h
    type_codes.inc
    visitor.h
    solve.h
)

# Configure SymEngine using our CMake options:
configure_file(symengine_config.h.in symengine_config.h)
# Include the config file:
include_directories(BEFORE ${symengine_BINARY_DIR})
# Include the source directory
include_directories(BEFORE ${symengine_SOURCE_DIR})

add_library(symengine ${SRC})


if (WITH_COTIRE)
    # throws if CMAKE_VERSION < 2.8.12
    include(cotire)
    cotire(symengine)
endif()

include(GenerateExportHeader)
generate_export_header(symengine)
set_target_properties(symengine PROPERTIES COMPILE_DEFINITIONS "symengine_EXPORTS")
if (BUILD_SHARED_LIBS)
    set_target_properties(
        symengine
        PROPERTIES
        VERSION ${SYMENGINE_VERSION}
        SOVERSION ${SYMENGINE_MAJOR_VERSION}.${SYMENGINE_MINOR_VERSION}
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_target_properties(symengine PROPERTIES COMPILE_FLAGS "-Wconversion -Wno-sign-conversion")
endif()

if (WITH_SYMENGINE_TEUCHOS)
    target_link_libraries(symengine teuchos)
endif()
target_link_libraries(symengine ${LIBS})

include(GNUInstallDirs)

install(TARGETS symengine
            EXPORT SymEngineTargets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
install(FILES "${symengine_BINARY_DIR}/symengine/symengine_config.h"
    "${symengine_BINARY_DIR}/symengine/symengine_export.h"
    DESTINATION include/symengine)

# make install
macro(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)
    FOREACH(HEADER ${${HEADER_LIST}})
        STRING(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
        install(FILES ${HEADER} DESTINATION include/symengine/${DIR})
    ENDFOREACH(HEADER HEADER_LIST)
endmacro(INSTALL_HEADERS_WITH_DIRECTORY)

INSTALL_HEADERS_WITH_DIRECTORY(HEADERS)

if (BUILD_TESTS)
    add_subdirectory(utilities/catch)
    add_subdirectory(tests)
endif()
