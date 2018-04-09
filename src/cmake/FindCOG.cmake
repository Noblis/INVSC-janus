# ==================================================== #
# The Cognitec CMake configuration file                #
#                                                      #
# Usage from an external project:                      #
#   In your CMakeLists.txt, add these lines:           #
#                                                      #
#   find_package(COG REQUIRED)                         #
#   target_link_libraries(MY_TARGET ${COG_LIBS})       #
# ==================================================== #

include(FindPackageHandleStandardArgs)
find_path(COG_DIR include/FRsdk/config.h)
find_package_handle_standard_args(COG DEFAULT_MSG COG_DIR)
set(COG_FOUND ${COG_FOUND})
if(COG_FOUND)
  set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
  set(VERSION "")
  include_directories(${COG_DIR}/include)
  #This is because the folder structure only contains major.minor and this variable contains the patch version too.
  string(SUBSTRING ${CMAKE_CXX_COMPILER_VERSION} 0 3 VERSION)
  set(COG_LIB_DIR ${COG_DIR}/lib/x86_64/gcc-${VERSION}-sse2/)
  message(STATUS ${COG_LIB_DIR})
  link_directories(${COG_LIB_DIR})
  set(COG_LIBS biospi
               jfrsdkjni
               output
               frsdk
               simio)
  message(STATUS ${COG_LIBS})
endif()
