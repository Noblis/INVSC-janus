include_directories(../include)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Trivial implementation of the Janus API
add_subdirectory(janus)
add_subdirectory(janus_io)

# Implementation of the Janus API using PittPatt
find_package(PittPatt5)
if(PittPatt5_FOUND)
  add_subdirectory(janus-pittpatt)
  add_subdirectory(janus_io-pittpatt)
endif()

# Implementation of the Janus API using OpenCV
find_package(OpenCV)
if(OpenCV_FOUND)
  add_subdirectory(janus_io-opencv)
endif()

# Janus API documentation
add_subdirectory(doxygen)

# Janus unit tests
add_subdirectory(tests)
