include_directories(BEFORE ../include)
file(GLOB JANUS_HEADERS ../include/*.h)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(${JANUS_IMPLEMENTATION} STREQUAL pittpatt)
  find_package(PP4 REQUIRED)
  add_subdirectory(pittpatt)
endif()

if(${JANUS_IO_IMPLEMENTATION} STREQUAL opencv_io)
  find_package(OpenCV 3.1)
  if(OpenCV_FOUND)
    add_subdirectory(opencv_io)
  endif()
endif()

# Janus API documentation
add_subdirectory(doxygen)

# Janus command line utilities
add_subdirectory(utils)
