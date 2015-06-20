set(PY_PATH ${PYTHON_INSTALL_PATH}/symengine/tests)
FILE(GLOB files "${symengine_SOURCE_DIR}/symengine/tests/*.py")
install(FILES ${files} DESTINATION ${PY_PATH})