if (WITH_SYMENGINE_TEUCHOS)
    add_subdirectory(utilities/teuchos)
    # Include Teuchos headers:
    include_directories(BEFORE ${teuchos_SOURCE_DIR})
    # Include Teuchos config file (generated per build):
    include_directories(BEFORE ${teuchos_BINARY_DIR})
endif()

if (WITH_SYMENGINE_PARSER)
    add_custom_command(OUTPUT parser/parser.cpp
                      COMMAND bisonc++ parser.yy
                      DEPENDS parser/parser.yy parser/parser.ih
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser
                     )

    add_custom_command(OUTPUT parser/scanner.cpp
                      COMMAND flexc++ --namespace=SymEngine scanner.ll
                      DEPENDS parser/scanner.ll parser/scanner.h
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/parser/
                     )
endif()

set(SRC
    add.cpp
    basic.cpp
    complex.cpp
    complex_double.cpp
    constants.cpp
    cse.cpp
    cwrapper.cpp
    dense_matrix.cpp
    derivative.cpp
    dict.cpp
    diophantine.cpp
    eval.cpp
    eval_double.cpp
    expand.cpp
    expression.cpp
    fields.cpp
    finitediff.cpp
    functions.cpp
    infinity.cpp
    integer.cpp
    logic.cpp
    matrix.cpp
    monomials.cpp
    mul.cpp
    nan.cpp
    ntheory.cpp
    number.cpp
    numer_denom.cpp
    parser/parser.cpp
    parser/parser_old.cpp
    parser/scanner.cpp
    polys/basic_conversions.cpp
    polys/msymenginepoly.cpp
    polys/uexprpoly.cpp
    polys/uintpoly.cpp
    polys/uratpoly.cpp
    pow.cpp
    printer.cpp
    rational.cpp
    real_double.cpp
    rewrite.cpp
    rings.cpp
    series.cpp
    series_generic.cpp
    sets.cpp
    solve.cpp
    sparse_matrix.cpp
    symbol.cpp
    symengine_rcp.cpp
    visitor.cpp
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
    set_source_files_properties(parser/parser.cpp parser/scanner.cpp PROPERTIES COMPILE_FLAGS "-Wno-conversion")
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
