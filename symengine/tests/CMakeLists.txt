include_directories(BEFORE ${symengine_SOURCE_DIR})
# Include Catch headers:
include_directories(BEFORE ${catch_SOURCE_DIR})

add_subdirectory(rcp)
add_subdirectory(basic)
add_subdirectory(ntheory)
add_subdirectory(matrix)
add_subdirectory(printing)
add_subdirectory(eval)
add_subdirectory(cwrapper)
add_subdirectory(expression)
