include_directories(../include)

# Empty implementation of the Janus API
add_subdirectory(janus)

# Implementation of the Janus API using PittPatt
add_subdirectory(pittpatt)

# Janus API documentation
add_subdirectory(doxygen)

# Janus unit tests
add_subdirectory(tests)
