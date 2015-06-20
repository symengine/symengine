add_subdirectory(lib)
add_subdirectory(tests)

set(PY_PATH ${PYTHON_INSTALL_PATH}/symengine)
FILE(GLOB files "${symengine_SOURCE_DIR}/symengine/*.py")
install(FILES ${files} DESTINATION ${PY_PATH})