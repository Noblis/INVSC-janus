file(GLOB JANUS_IO-OPENCV_SOURCES *.cpp)
add_library(janus_io-opencv SHARED ${JANUS_IO-OPENCV_SOURCES})
set_target_properties(janus_io-opencv PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(janus_io-opencv janus opencv_core opencv_highgui)
install(TARGETS janus_io-opencv RUNTIME DESTINATION bin
                                LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
set(JANUS_IO_TEST_IMPLEMENTATIONS ${JANUS_IO_TEST_IMPLEMENTATIONS} "janus_io-opencv" PARENT_SCOPE)
