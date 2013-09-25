# ==================================================== #
# The PittPatt 5.x.x CMake configuration file          #
#                                                      #
# Usage from an external project:                      #
#   In your CMakeLists.txt, add these lines:           #
#                                                      #
#   find_package(PittPatt5 REQUIRED)                   #
#   target_link_libraries(MY_TARGET ${PittPatt5_LIBS}) #
# ==================================================== #

include(FindPackageHandleStandardArgs)
find_path(PittPatt5_DIR include/pittpatt_sdk.h)
find_package_handle_standard_args(PittPatt5 DEFAULT_MSG PittPatt5_DIR)
set(PittPatt5_FOUND ${PITTPATT5_FOUND})
if(PittPatt5_FOUND)
  include_directories(${PittPatt5_DIR}/include)
  link_directories(${PittPatt5_DIR}/lib)
  set(PittPatt5_LIBS pittpatt_raw_image
                     pittpatt_raw_image_io
                     pittpatt_sdk
                     pittpatt_video_io)
endif()
