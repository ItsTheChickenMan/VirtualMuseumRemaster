#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFML::System" for configuration "Release"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsfml-system-s.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::System )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::System "${_IMPORT_PREFIX}/lib/libsfml-system-s.a" )

# Import target "SFML::Main" for configuration "Release"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsfml-main.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Main )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Main "${_IMPORT_PREFIX}/lib/libsfml-main.a" )

# Import target "SFML::Audio" for configuration "Release"
set_property(TARGET SFML::Audio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::Audio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsfml-audio-s.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Audio )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Audio "${_IMPORT_PREFIX}/lib/libsfml-audio-s.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
