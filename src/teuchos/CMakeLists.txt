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

add_library(teuchos ${SRC})
