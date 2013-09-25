if(PittPatt5_FOUND)
  set(JANUS_DEFAULT_IMPLEMENTATION ../janus/janus_allocate_media.c
                                   ../janus/janus_allocate_object.c
                                   ../janus/janus_allocate_object_list.c
                                   ../janus/janus_free_media.c
                                   ../janus/janus_free_object.c
                                   ../janus/janus_free_object_list.c)
  add_library(janus-pittpatt SHARED janus_pittpatt.c ${JANUS_DEFAULT_IMPLEMENTATION})
  set_target_properties(janus-pittpatt PROPERTIES
                        DEFINE_SYMBOL JANUS_LIBRARY
                        VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                        SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
  target_link_libraries(janus-pittpatt pittpatt_sdk)

  install(TARGETS janus-pittpatt RUNTIME DESTINATION bin
                                 LIBRARY DESTINATION lib
                                 ARCHIVE DESTINATION lib)

  # Test this implementation if PittPatt was found
  set(JANUS_TEST_IMPLEMENTATION "janus-pittpatt" PARENT_SCOPE)
  set(JANUS_TEST_SDK_PATH ${PittPatt5_DIR} PARENT_SCOPE)
endif()
