# We use JANUS_MISC_FILES as an idiom to ensure these files are added to the
#  IDE's project tree.
set(JANUS_MISC_FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt
                     ${CMAKE_SOURCE_DIR}/README.md
                     ${CMAKE_SOURCE_DIR}/src/doxygen/Doxyfile.in
                     ${CMAKE_SOURCE_DIR}/include/janus.h)

add_library(janus SHARED janus.c ${JANUS_MISC_FILES})
set_target_properties(janus PROPERTIES
                      DEFINE_SYMBOL JANUS_LIBRARY
                      VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH}
                      SOVERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR})

install(TARGETS janus RUNTIME DESTINATION bin 
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib)
