# ==================================================== #
# The PittPatt 4.x.x CMake configuration file          #
#                                                      #
# Usage from an external project:                      #
#   In your CMakeLists.txt, add these lines:           #
#                                                      #
#   find_package(PP5 REQUIRED)                         #
#   target_link_libraries(MY_TARGET ${PP5_LIBS})       #
# ==================================================== #

include(FindPackageHandleStandardArgs)
find_path(PP4_DIR include/pittpatt_nc_sdk.h)
find_package_handle_standard_args(PP4 DEFAULT_MSG PP4_DIR)
set(PP4_FOUND ${PP4_FOUND})
if(PP4_FOUND)
  include_directories(${PP4_DIR}/include)
  link_directories(${PP4_DIR}/lib)
  set(PP4_LIBS pittpatt_raw_image
               pittpatt_raw_image_io
               pittpatt_nc_sdk
               pittpatt_recognition_core
               pittpatt_video_io)
endif()
