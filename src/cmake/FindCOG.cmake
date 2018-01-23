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
  include_directories(${COG_DIR}/include)
  link_directories(${cog_DIR}/lib/x86_64/gcc-4.8-sse2)
  set(COG_LIBS biospi
               jfrsdkjni
               output
               frsdk
               simio)
endif()
