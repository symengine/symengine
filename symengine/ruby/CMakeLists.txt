project(ruby_wrapper)

add_subdirectory(lib/symengine)
#Copies LICENSE from root directory to gem directory
file(COPY ${symengine_SOURCE_DIR}/LICENSE DESTINATION ${ruby_wrapper_SOURCE_DIR})