include_directories(../include)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Implementation of the Janus API using PittPatt
option(JANUS_BUILD_PP5_WRAPPER "Build Janus implementation using PittPatt 5" ON)
if(${JANUS_BUILD_PP5_WRAPPER})
  find_package(PittPatt5)
  if(PittPatt5_FOUND)
    #add_subdirectory(pittpatt)
    add_subdirectory(pittpatt_io)
  endif()
endif()

# Implementation of the Janus API using OpenCV
find_package(OpenCV)
if(OpenCV_FOUND AND PittPatt5_FOUND)
  add_subdirectory(opencv_io)
endif()

# Janus API documentation
add_subdirectory(doxygen)

# Janus unit tests
add_subdirectory(tests)
