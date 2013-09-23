# Reference implementation of the Janus API using PittPatt 5.x.x
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(PittPatt5)

if(PittPatt5_FOUND)
  add_library(janus_pittpatt SHARED janus_pittpatt.c)
  set_target_properties(janus_pittpatt PROPERTIES
                        DEFINE_SYMBOL JANUS_LIBRARY
                        VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                        SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
  target_link_libraries(janus_pittpatt ${PittPatt5_LIBS})

  install(TARGETS janus_pittpatt RUNTIME DESTINATION bin
                                 LIBRARY DESTINATION lib
                                 ARCHIVE DESTINATION lib)

  # Test this implementation if PittPatt was found
  set(JANUS_TEST_IMPLEMENTATION "janus_pittpatt" PARENT_SCOPE)
  set(JANUS_TEST_SDK_PATH ${PittPatt5_DIR} PARENT_SCOPE)
endif()
