project(catch)

set(SRC
    catch.cpp
)

add_library(catch STATIC ${SRC})
target_link_libraries(catch symengine)
set_property(TARGET catch PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${catch_SOURCE_DIR})
