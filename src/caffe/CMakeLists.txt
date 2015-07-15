add_library(janus_caffe SHARED caffe.cpp caffe_io.cpp ../opencv_io/opencv_io.cpp)
set_target_properties(janus_caffe PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})
                      
include_directories(${Caffe_INCLUDE_DIRS})
add_definitions(${Caffe_DEFINITIONS})
target_link_libraries(janus_caffe ${Caffe_Libraries} opencv_core opencv_highgui)
install(TARGETS janus_caffe RUNTIME DESTINATION bin
                            LIBRARY DESTINATION lib
                            ARCHIVE DESTINATION lib)

if(NOT DEFINED ${JANUS_IMPLEMENTATION})
  set(JANUS_IMPLEMENTATION "janus_caffe" PARENT_SCOPE)
endif()