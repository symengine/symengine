project(ruby_wrapper)

add_subdirectory(ext/symengine)
#Copies LICENSE from root directory to gem directory
file(COPY ${symengine_SOURCE_DIR}/LICENSE DESTINATION ${ruby_wrapper_BINARY_DIR})