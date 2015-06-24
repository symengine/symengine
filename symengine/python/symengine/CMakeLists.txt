add_subdirectory(lib)
add_subdirectory(tests)

set(PY_PATH ${PYTHON_INSTALL_PATH}/symengine)
install(FILES __init__.py utilities.py compatibility.py
    DESTINATION ${PY_PATH}
    )
