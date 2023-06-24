#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OTExtension::otextension" for configuration "Release"
set_property(TARGET OTExtension::otextension APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(OTExtension::otextension PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libotextension.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS OTExtension::otextension )
list(APPEND _IMPORT_CHECK_FILES_FOR_OTExtension::otextension "${_IMPORT_PREFIX}/lib/libotextension.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
