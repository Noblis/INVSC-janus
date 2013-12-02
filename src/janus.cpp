#include "janus.h"

// These functions have no external dependencies

const char *janus_error_to_string(janus_error error)
{
    switch (error) {
      case JANUS_SUCCESS:              return "Success";
      case JANUS_UNKNOWN_ERROR:        return "Unknown error";
      case JANUS_OUT_OF_MEMORY:        return "Out of memory";
      case JANUS_INVALID_SDK_PATH:     return "Invalid SDK path";
      case JANUS_OPEN_ERROR:           return "File open error";
      case JANUS_READ_ERROR:           return "File read error";
      case JANUS_WRITE_ERROR:          return "File write error";
      case JANUS_PARSE_ERROR:          return "File parse error";
      case JANUS_INVALID_IMAGE:        return "Invalid image";
      case JANUS_INVALID_VIDEO:        return "Invalid video";
      case JANUS_MISSING_TEMPLATE_ID:  return "Missing field Template_ID";
      case JANUS_MISSING_FILE_NAME:    return "Missing field File_Name";
      case JANUS_NULL_VALUE:           return "Null value";
      case JANUS_NULL_ATTRIBUTE_LIST:  return "Null attribute list";
      case JANUS_TEMPLATE_ID_MISMATCH: return "Expected matching template IDs";
      default:                         return "Unrecognized error code";
    }
}
