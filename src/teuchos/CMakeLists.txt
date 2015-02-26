project(teuchos)

set(SRC
    Teuchos_dyn_cast.cpp
    Teuchos_Ptr.cpp
    Teuchos_RCPNode.cpp
    Teuchos_TestForException.cpp
    Teuchos_TypeNameTraits.cpp
    Teuchos_stacktrace.cpp
)

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  # Must suppress strick aliasing warnings for this file!
  SET_SOURCE_FILES_PROPERTIES(
    Teuchos_stacktrace.cpp
    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing
    )
endif()

# Configure Teuchos using our CMake options:
configure_file(Teuchos_config.h.in Teuchos_config.h)
# Include the config file:
include_directories(BEFORE ${teuchos_BINARY_DIR})

add_library(teuchos STATIC ${SRC})

install(TARGETS teuchos
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
        )
