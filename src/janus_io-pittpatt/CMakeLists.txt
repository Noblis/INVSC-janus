file(GLOB JANUS_IO-PITTATT_SOURCES *.c)
add_library(janus_io-pittpatt SHARED ${JANUS_IO-PITTATT_SOURCES})
set_target_properties(janus_io-pittpatt PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(janus_io-pittpatt janus-pittpatt pittpatt_raw_image pittpatt_raw_image_io)
install(TARGETS janus_io-pittpatt RUNTIME DESTINATION bin
                                  LIBRARY DESTINATION lib
                                  ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
option(JANUS_TEST_JANUS_IO-PITTPATT "Test the PittPatt implementation of the Janus I/O API" ON)
if(${JANUS_TEST_JANUS_IO-PITTPATT})
  set(JANUS_IO_TEST_IMPLEMENTATIONS ${JANUS_IO_TEST_IMPLEMENTATIONS} "janus_io-pittpatt" PARENT_SCOPE)
endif()
