set(RUBY_WRAPPER_SRC
    ruby_basic.c
    ruby_symbol.c
    ruby_integer.c
    ruby_rational.c
    symengine_macros.c
    symengine.c
    )

include_directories(BEFORE ${RUBY_INCLUDE_DIRS})

add_library(symengine_ruby SHARED ${RUBY_WRAPPER_SRC})
target_link_libraries(symengine_ruby ${SYMENGINE_LIBRARIES} ${RUBY_LIBRARIES})
set_target_properties(symengine_ruby PROPERTIES
                        PREFIX ""
                        OUTPUT_NAME "symengine"
                        LIBRARY_OUTPUT_DIRECTORY "${ruby_wrapper_BINARY_DIR}/lib/symengine"
                     )
