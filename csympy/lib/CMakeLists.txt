set(SRC
    csympy_wrapper.cpp
    )

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${csympy_BINARY_DIR}/src ${teuchos_BINARY_DIR})

cython_add_module_pyx(csympy_wrapper)
add_python_library(csympy_wrapper ${SRC})
target_link_libraries(csympy_wrapper csympy teuchos ${LIBS})

add_custom_target(cython
    COMMAND cython csympy_wrapper.pyx
    )

set(PY_PATH ${PYTHON_INSTALL_PATH}/csympy/lib)
install(TARGETS csympy_wrapper
            RUNTIME DESTINATION ${PY_PATH}
            ARCHIVE DESTINATION ${PY_PATH}
            LIBRARY DESTINATION ${PY_PATH}
        )
install(FILES __init__.py
    DESTINATION ${PY_PATH}
    )
