#include "janus.h"

// These functions have no external dependencies

#define ENUM_CASE(X) case X: return #X;
#define ENUM_DEFAULT default: return "???";

const char *janus_error_to_string(janus_error error)
{
    switch (error) {
        ENUM_CASE(JANUS_SUCCESS)
        ENUM_CASE(JANUS_UNKNOWN_ERROR)
        ENUM_CASE(JANUS_OUT_OF_MEMORY)
        ENUM_CASE(JANUS_INVALID_SDK_PATH)
        ENUM_CASE(JANUS_OPEN_ERROR)
        ENUM_CASE(JANUS_READ_ERROR)
        ENUM_CASE(JANUS_WRITE_ERROR)
        ENUM_CASE(JANUS_PARSE_ERROR)
        ENUM_CASE(JANUS_INVALID_IMAGE)
        ENUM_CASE(JANUS_INVALID_VIDEO)
        ENUM_CASE(JANUS_MISSING_TEMPLATE_ID)
        ENUM_CASE(JANUS_MISSING_FILE_NAME)
        ENUM_CASE(JANUS_NULL_ATTRIBUTE_LIST)
        ENUM_CASE(JANUS_TEMPLATE_ID_MISMATCH)
        ENUM_DEFAULT
    }
}

const char *janus_attribute_to_string(janus_attribute attribute)
{
    switch (attribute) {
        ENUM_CASE(JANUS_INVALID_ATTRIBUTE)
        ENUM_CASE(JANUS_FRAME)
        ENUM_CASE(JANUS_RIGHT_EYE_X)
        ENUM_CASE(JANUS_RIGHT_EYE_Y)
        ENUM_CASE(JANUS_LEFT_EYE_X)
        ENUM_CASE(JANUS_LEFT_EYE_Y)
        ENUM_CASE(JANUS_NOSE_BASE_X)
        ENUM_CASE(JANUS_NOSE_BASE_Y)
        ENUM_DEFAULT
    }
}
