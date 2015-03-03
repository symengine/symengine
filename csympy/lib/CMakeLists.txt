set(SRC
    csympy_wrapper.cpp
    )

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(BEFORE ${csympy_BINARY_DIR}/src ${teuchos_BINARY_DIR})
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

cython_add_module_pyx(csympy_wrapper csympy.pxd)
add_python_library(csympy_wrapper ${SRC})
target_link_libraries(csympy_wrapper csympy)
if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
  # Must suppress strict aliasing for this file
  set_source_files_properties(
    csympy_wrapper.cpp
    PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -Wno-unused-function -Wno-constant-logical-operand"
    )
endif()

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
