#include "janus.h"

// These functions have no external dependencies

size_t janus_max_template_size()
{
    return JANUS_MAX_TEMPLATE_SIZE_LIMIT;
}

#define ENUM_CASE(X) case JANUS_##X: return #X;
#define ENUM_DEFAULT default: return "???";

const char *janus_error_to_string(janus_error error)
{
    switch (error) {
        ENUM_CASE(SUCCESS)
        ENUM_CASE(UNKNOWN_ERROR)
        ENUM_CASE(OUT_OF_MEMORY)
        ENUM_CASE(INVALID_SDK_PATH)
        ENUM_CASE(OPEN_ERROR)
        ENUM_CASE(READ_ERROR)
        ENUM_CASE(WRITE_ERROR)
        ENUM_CASE(PARSE_ERROR)
        ENUM_CASE(INVALID_IMAGE)
        ENUM_CASE(INVALID_VIDEO)
        ENUM_CASE(MISSING_TEMPLATE_ID)
        ENUM_CASE(MISSING_FILE_NAME)
        ENUM_CASE(NULL_ATTRIBUTE_LIST)
        ENUM_DEFAULT
    }
}

const char *janus_attribute_to_string(janus_attribute attribute)
{
    switch (attribute) {
        ENUM_CASE(INVALID_ATTRIBUTE)
        ENUM_CASE(FRAME)
        ENUM_CASE(RIGHT_EYE_X)
        ENUM_CASE(RIGHT_EYE_Y)
        ENUM_CASE(LEFT_EYE_X)
        ENUM_CASE(LEFT_EYE_Y)
        ENUM_CASE(NOSE_BASE_X)
        ENUM_CASE(NOSE_BASE_Y)
        ENUM_DEFAULT
    }
}
