#include "janus.h"

// These functions have no external dependencies

const char *janus_error_to_string(janus_error error)
{
    switch (error) {
      case JANUS_SUCCESS:             return "Success";
      case JANUS_UNKNOWN_ERROR:       return "Unknown error";
      case JANUS_OUT_OF_MEMORY:       return "Out of memory";
      case JANUS_INVALID_SDK_PATH:    return "Invalid SDK path";
      case JANUS_INVALID_IMAGE:       return "Invalid image";
      case JANUS_INVALID_VIDEO:       return "Invalid video";
      case JANUS_NULL_VALUE:          return "Null value";
      case JANUS_NULL_ATTRIBUTE_LIST: return "Null attribute list";
      default:                        return "Unrecognized error code";
    }
}
