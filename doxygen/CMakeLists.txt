# Build HTML documentation if Doxygen is found
find_package(Doxygen)
if(${DOXYGEN_FOUND})
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in Doxyfile)
  add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE} Doxyfile)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION .)
endif()
