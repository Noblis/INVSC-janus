include_directories(../include)

# Empty implementation of the Janus API
add_subdirectory(janus)
add_subdirectory(janus_io)

# Implementation of the Janus API using PittPatt
add_subdirectory(pittpatt)

# Janus API documentation
add_subdirectory(doxygen)

# Janus unit tests
add_subdirectory(tests)
