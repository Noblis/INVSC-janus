project(janus)
cmake_minimum_required(VERSION 3.0)

set(JANUS_NAME "Janus")
set(JANUS_DESCRIPTION "IARPA Janus Program API")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Extract version info from header file
macro(janus_get_version VAR)
  file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/iarpa_janus.h LINE
       REGEX "#define[ ]+${VAR}[ ]+[0-9]+")
  string(REGEX MATCH "[0-9]+" ${VAR} ${LINE})
endmacro(janus_get_version)

janus_get_version(JANUS_VERSION_MAJOR)
janus_get_version(JANUS_VERSION_MINOR)
janus_get_version(JANUS_VERSION_PATCH)

configure_file(include/iarpa_janus.h ${CMAKE_CURRENT_BINARY_DIR}/refreshes_version_when_changed)
set(JANUS_VERSION ${JANUS_VERSION_MAJOR}.${JANUS_VERSION_MINOR}.${JANUS_VERSION_PATCH})

if(UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fvisibility=hidden")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fvisibility=hidden")
endif()

include(CTest)

# The harness includes an optional PittPatt5 implementation
add_subdirectory(implementations)

# If we have an implementation, build the test harness
if(JANUS_IMPLEMENTATION)
  add_subdirectory(harness)
endif()

# Optionally build the documentation
option(BUILD_JANUS_DOCUMENTATION "Use doxygen to build the Janus documentation" OFF)
if (${BUILD_JANUS_DOCUMENTATION})
  add_subdirectory(docs)
endif()
