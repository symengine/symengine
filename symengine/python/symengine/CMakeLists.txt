add_subdirectory(lib)
add_subdirectory(tests)

set(PY_PATH ${PYTHON_INSTALL_PATH}/symengine)
install(FILES
    __init__.py
    utilities.py
    compatibility.py
    ${CMAKE_CURRENT_BINARY_DIR}/__init__.pyc
    ${CMAKE_CURRENT_BINARY_DIR}/utilities.pyc
    ${CMAKE_CURRENT_BINARY_DIR}/compatibility.pyc
    DESTINATION ${PY_PATH}
    )
