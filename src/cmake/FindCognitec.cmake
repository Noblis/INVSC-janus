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
  link_directories(${cog_DIR}/lib)
  set(COG_LIBS pittpatt_raw_image
               pittpatt_raw_image_io
               pittpatt_nc_sdk
               pittpatt_recognition_core
               pittpatt_video_io)
endif()
