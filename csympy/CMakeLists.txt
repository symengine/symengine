add_subdirectory(lib)

set(PY_PATH ${PYTHON_INSTALL_PATH}/csympy)
install(FILES __init__.py
    DESTINATION ${PY_PATH}
    )
