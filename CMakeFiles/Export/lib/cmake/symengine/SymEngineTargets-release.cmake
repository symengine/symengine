#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "teuchos" for configuration "Release"
set_property(TARGET teuchos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teuchos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteuchos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teuchos )
list(APPEND _IMPORT_CHECK_FILES_FOR_teuchos "${_IMPORT_PREFIX}/lib/libteuchos.a" )

# Import target "symengine" for configuration "Release"
set_property(TARGET symengine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(symengine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchos;gmpxx;gmp"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsymengine.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS symengine )
list(APPEND _IMPORT_CHECK_FILES_FOR_symengine "${_IMPORT_PREFIX}/lib/libsymengine.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
