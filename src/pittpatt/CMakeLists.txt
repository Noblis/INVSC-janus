add_library(pittpatt SHARED ../janus.c pittpatt.c pittpatt_io.c)
set_target_properties(pittpatt PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(pittpatt pittpatt_sdk pittpatt_raw_image pittpatt_raw_image_io pittpatt_video_io)
install(TARGETS pittpatt RUNTIME DESTINATION bin
                         LIBRARY DESTINATION lib
                         ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
set(JANUS_TEST_IMPLEMENTATION ${JANUS_TEST_IMPLEMENTATIONS} "pittpatt" PARENT_SCOPE)
set(JANUS_TEST_SDK_PATH ${JANUS_TEST_SDK_PATHS} ${PittPatt5_DIR} PARENT_SCOPE)
