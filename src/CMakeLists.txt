# We use JANUS_MISC_FILES as an idiom to ensure these files are populated in the
#  IDE project tree.
set(JANUS_MISC_FILES ../LICENSE.txt
                     ../README.md
                     ../doxygen/Doxyfile.in
                     ../include/janus.h)

include_directories(../include)
add_library(janus SHARED janus.c ${JANUS_MISC_FILES})
set_target_properties(janus PROPERTIES DEFINE_SYMBOL JANUS_LIBRARY)
install(TARGETS janus RUNTIME DESTINATION bin 
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib)
