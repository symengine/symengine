set(SRC
    symengine_wrapper.cpp
    )

configure_file(config.pxi.in config.pxi)

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(BEFORE ${symengine_BINARY_DIR} ${teuchos_BINARY_DIR})
include_directories(BEFORE ${symengine_SOURCE_DIR} ${teuchos_SOURCE_DIR})
include_directories(BEFORE ${symengine_BINARY_DIR}/symengine/python/symengine/lib)

cython_add_module_pyx(symengine_wrapper symengine.pxd)
add_python_library(symengine_wrapper ${SRC})
target_link_libraries(symengine_wrapper symengine)
if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
  # Must suppress strict aliasing for this file
  set_source_files_properties(
    symengine_wrapper.cpp
    PROPERTIES COMPILE_FLAGS "-fno-strict-aliasing -Wno-unused-function -Wno-constant-logical-operand"
    )
endif()

add_custom_target(cython
    COMMAND cython symengine_wrapper.pyx
    )

set(PY_PATH ${PYTHON_INSTALL_PATH}/symengine/lib)
install(TARGETS symengine_wrapper
            RUNTIME DESTINATION ${PY_PATH}
            ARCHIVE DESTINATION ${PY_PATH}
            LIBRARY DESTINATION ${PY_PATH}
        )
install(FILES __init__.py
    ${CMAKE_CURRENT_BINARY_DIR}/config.pxi
    symengine.pxd
    symengine_wrapper.pxd
    DESTINATION ${PY_PATH}
    )
