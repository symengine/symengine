add_subdirectory(lib/symengine)
#Copies LICENSE from root directory to gem directory
file(COPY ${symengine_SOURCE_DIR}/LICENSE DESTINATION ${symengine_SOURCE_DIR}/src/ruby)
