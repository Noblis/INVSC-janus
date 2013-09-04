add_library(janus SHARED janus.c ../include/janus.h)
set_target_properties(janus PROPERTIES DEFINE_SYMBOL JANUS_LIBRARY)
install(TARGETS janus RUNTIME DESTINATION bin 
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib)
