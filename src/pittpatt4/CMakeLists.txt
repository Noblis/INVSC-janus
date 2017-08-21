add_library(pittpatt4 SHARED pittpatt4.cpp ../janus_io.cpp)
set_target_properties(pittpatt4 PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(pittpatt4 ${PP4_LIBS} ${JANUS_IO_IMPLEMENTATION})
install(TARGETS pittpatt4 RUNTIME DESTINATION bin
                         LIBRARY DESTINATION lib
                         ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
set(JANUS_TEST_IMPLEMENTATION ${JANUS_TEST_IMPLEMENTATIONS} "pittpatt4" PARENT_SCOPE)
set(JANUS_TEST_SDK_PATH ${JANUS_TEST_SDK_PATHS} ${PP4_DIR} PARENT_SCOPE)
