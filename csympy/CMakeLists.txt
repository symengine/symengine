add_subdirectory(lib)

set(PY_PATH ${PYTHON_INSTALL_PATH}/csympy)
install(FILES __init__.py utilities.py
    DESTINATION ${PY_PATH}
    )
