file(GLOB JANUS_IO_SOURCES *.c *.h)
add_library(janus_io SHARED ${JANUS_IO_SOURCES})
set_target_properties(janus_io PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(janus_io janus)
install(TARGETS janus_io RUNTIME DESTINATION bin
                         LIBRARY DESTINATION lib
                         ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
option(JANUS_TEST_JANUS_IO "Test the trivial implementation of the Janus I/O API" ON)
if(${JANUS_TEST_JANUS_IO})
  set(JANUS_IO_TEST_IMPLEMENTATIONS ${JANUS_IO_TEST_IMPLEMENTATIONS} "janus_io" PARENT_SCOPE)
endif()
