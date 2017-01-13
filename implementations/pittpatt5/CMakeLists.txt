if (NOT JANUS_IO_IMPLEMENTATION STREQUAL "opencv_io")
  message(FATAL_ERROR "The PittPatt5 wrapper requires the JANUS_IO_IMPLEMENTATION to be opencv_io. Consider setting BUILD_JANUS_OPENCV_IO_IMPLEMENTATION in your cmake file")
endif()

include_directories(BEFORE ../../include)
include_directories(../opencv_io)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(PP5 REQUIRED)

add_library(pittpatt5 SHARED pittpatt5.cpp)
set_target_properties(pittpatt5 PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
target_link_libraries(pittpatt5 ${PP5_LIBS} opencv_io)
install(TARGETS pittpatt5 RUNTIME DESTINATION bin
                         LIBRARY DESTINATION lib
                         ARCHIVE DESTINATION lib)

# Add this to the list of implementations to test
set(JANUS_TEST_IMPLEMENTATION ${JANUS_TEST_IMPLEMENTATIONS} "pittpatt5" PARENT_SCOPE)
set(JANUS_TEST_SDK_PATH ${JANUS_TEST_SDK_PATHS} ${PP5_DIR} PARENT_SCOPE)
