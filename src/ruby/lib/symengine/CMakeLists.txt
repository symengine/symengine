set(RUBY_WRAPPER_SRC
    ../../ext/symengine/ruby_basic.c
    ../../ext/symengine/symengine.c
    )

include_directories(BEFORE  ${symengine_SOURCE_DIR}/src/ruby/ext/symengine
                            ${symengine_SOURCE_DIR}/src)

add_library(symengine_ruby SHARED ${RUBY_WRAPPER_SRC})
target_link_libraries(symengine_ruby symengine)
set_target_properties(symengine_ruby PROPERTIES PREFIX "" OUTPUT_NAME "symengine")
