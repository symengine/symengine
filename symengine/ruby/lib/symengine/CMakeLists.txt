set(RUBY_WRAPPER_SRC
    ${ruby_wrapper_SOURCE_DIR}/ext/symengine/ruby_basic.c
    ${ruby_wrapper_SOURCE_DIR}/ext/symengine/symengine.c
    )

include_directories(BEFORE ${ruby_wrapper_SOURCE_DIR}/ext/symengine)

add_library(symengine_ruby SHARED ${RUBY_WRAPPER_SRC})
target_link_libraries(symengine_ruby symengine)
set_target_properties(symengine_ruby PROPERTIES PREFIX "" OUTPUT_NAME "symengine")
