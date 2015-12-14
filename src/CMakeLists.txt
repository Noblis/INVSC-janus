include_directories(BEFORE ../include)
file(GLOB JANUS_HEADERS ../include/*.h)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(JANUS_BUILD_PP5_WRAPPER "Build Janus implementation using PittPatt 5" OFF)
if(${JANUS_BUILD_PP5_WRAPPER})
  find_package(PP5 REQUIRED)
  add_subdirectory(pittpatt)
endif()

option(JANUS_BUILD_OPENCV_WRAPPER "Build Janus I/O implementation using OpenCV" OFF)
if(${JANUS_BUILD_OPENCV_WRAPPER} AND NOT ${JANUS_BUILD_PP5_WRAPPER})
  find_package(OpenCV)
  if(OpenCV_FOUND)
    add_subdirectory(opencv_io)
  endif()
elseif(${JANUS_BUILD_OPENCV_WRAPPER} AND ${JANUS_BUILD_PP5_WRAPPER})
  message(ERROR "You cannot build OpenCV I/O and PittPatt 5 I/O at the same time")
endif()

# Janus API documentation
add_subdirectory(doxygen)

# Janus command line utilities
add_subdirectory(utils)
