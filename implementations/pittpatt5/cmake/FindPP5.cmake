# ==================================================== #
# The PittPatt 5.x.x CMake configuration file          #
#                                                      #
# Usage from an external project:                      #
#   In your CMakeLists.txt, add these lines:           #
#                                                      #
#   find_package(PP5 REQUIRED)                         #
#   target_link_libraries(MY_TARGET ${PP5_LIBS})       #
# ==================================================== #

macro(find_pittpatt_library LIB_VAR LIB_NAME)
  find_library(${LIB_VAR} NAMES ${LIB_NAME}
                          PATHS ${PP5_DIR}/lib)
  
  if (NOT ${LIB_VAR})
    message(WARNING "Could not find PittPatt library: ${LIB_NAME} in ${PP5_DIR}/lib")
    set(PP5_FOUND FALSE)
  endif()

  mark_as_advanced(${LIB_VAR})
endmacro()

# Look for the PittPatt5 directory
include(FindPackageHandleStandardArgs)
find_path(PP5_DIR include/pittpatt_sdk.h)
find_package_handle_standard_args(PP5 DEFAULT_MSG PP5_DIR)
set(PP5_FOUND ${PP5_FOUND})

# If we found a PittPatt5 directory try and find all the relevant libraries
set(PP5_LIBS)
if(PP5_FOUND)
  include_directories(${PP5_DIR}/include)

  find_pittpatt_library(PP5_RAW_IMAGE    pittpatt_raw_image)
  find_pittpatt_library(PP5_RAW_IMAGE_IO pittpatt_raw_image_io)
  find_pittpatt_library(PP5_SDK          pittpatt_sdk)
  find_pittpatt_library(PP5_VIDEO_IO     pittpatt_video_io)

  set(PP5_LIBS ${PP5_RAW_IMAGE}
               ${PP5_RAW_IMAGE_IO}
               ${PP5_SDK}
               ${PP5_VIDEO_IO})        
endif()
