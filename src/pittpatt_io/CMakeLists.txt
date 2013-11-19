add_library(pittpatt_io SHARED pittpatt_io.c)
set_target_properties(pittpatt_io PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(pittpatt_io pittpatt_raw_image pittpatt_raw_image_io pittpatt_video_io)
install(TARGETS pittpatt_io RUNTIME DESTINATION bin
                            LIBRARY DESTINATION lib
                            ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
option(JANUS_TEST_PITTPATT_IO "Test the PittPatt implementation of the Janus I/O API" ON)
if(${JANUS_TEST_PITTPATT_IO})
  set(JANUS_IO_TEST_IMPLEMENTATIONS ${JANUS_IO_TEST_IMPLEMENTATIONS} "pittpatt_io" PARENT_SCOPE)
endif()
