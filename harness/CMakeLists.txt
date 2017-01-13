include_directories(BEFORE ../include)
include_directories(.)

add_library(janus_harness SHARED janus_harness.cpp)
target_link_libraries(janus_harness ${JANUS_IMPLEMENTATION} ${JANUS_IO_IMPLEMENTATION})

set_target_properties(janus_harness PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
install(TARGETS janus_harness RUNTIME DESTINATION bin
                              LIBRARY DESTINATION lib)

# Janus command line utilities
add_subdirectory(utils)
