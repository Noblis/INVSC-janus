include_directories(../include)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Implementation of the Janus API using PittPatt
find_package(PittPatt5)
if(PittPatt5_FOUND)
  # add_subdirectory(pittpatt)
  add_subdirectory(pittpatt_io)
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
