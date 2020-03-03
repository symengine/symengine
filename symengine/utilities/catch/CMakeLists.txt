project(catch)

set(SRC
    catch.cpp
)

add_library(catch STATIC ${SRC})
target_link_libraries(catch symengine)
target_include_directories(catch SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})  # ignore compiler warnings from catch.hpp
