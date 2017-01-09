include_directories(BEFORE ../include)
file(GLOB JANUS_HEADERS ../include/*.h)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(${JANUS_IMPLEMENTATION} STREQUAL pittpatt)
  find_package(PP5 REQUIRED)
  add_subdirectory(pittpatt)
endif()

if(${JANUS_IO_IMPLEMENTATION} STREQUAL opencv_io)
  find_package(OpenCV 3.1)
  if(OpenCV_FOUND)
    add_subdirectory(opencv_io)
  endif()
endif()

add_library(janus_harness SHARED janus_io.cpp)
target_link_libraries(janus_harness ${JANUS_IMPLEMENTATION} ${JANUS_IO_IMPLEMENTATION})

set_target_properties(janus_harness PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
install(TARGETS janus_harness RUNTIME DESTINATION bin
                              LIBRARY DESTINATION lib)

# Janus API documentation
add_subdirectory(doxygen)

# Janus command line utilities
add_subdirectory(utils)
