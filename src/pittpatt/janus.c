#include <stdlib.h>
#include <string.h>

#include "janus.h"

// The implementation of these functions rely only on the C Standard Library.

const char *janus_error_to_string(janus_error error)
{
    switch (error) {
      case JANUS_SUCCESS:             return "Success";
      case JANUS_UNKNOWN_ERROR:       return "Unknown error";
      case JANUS_OUT_OF_MEMORY:       return "Out of memory";
      case JANUS_INVALID_SDK_PATH:    return "Invalid SDK path";
      case JANUS_INVALID_IMAGE:       return "Invalid image";
      case JANUS_INVALID_VIDEO:       return "Invalid video";
      case JANUS_NULL_CONTEXT:        return "Null context";
      case JANUS_NULL_IMAGE:          return "Null image";
      case JANUS_NULL_ATTRIBUTE_LIST: return "Null attribute list";
      case JANUS_NULL_OBJECT:         return "Null object";
      case JANUS_NULL_OBJECT_LIST:    return "Null object list";
      default:                        return "Unrecognized error code";
    }
}

// These implementations of janus_allocate_* require just one call to malloc
// and simplify the implementations of janus_free_*.

janus_error janus_allocate_image(const janus_size channels,
                                 const janus_size width,
                                 const janus_size height,
                                 janus_image *image)
{
    if (!image) return JANUS_NULL_IMAGE;
    janus_image result = malloc(sizeof(struct janus_image_type) +
                                sizeof(janus_data) * channels * width * height);
    if (!result) {
        *image = NULL;
        return JANUS_OUT_OF_MEMORY;
    }
    result->channels = channels;
    result->width = width;
    result->height = height;
    result->data = (janus_data*)(result + 1);
    *image = result;
    return JANUS_SUCCESS;
}

void janus_free_image(janus_image image)
{
    free(image);
}

janus_error janus_allocate_attribute_list(const janus_size size,
                                          janus_attribute_list *attribute_list)
{
    if (!attribute_list) return JANUS_NULL_ATTRIBUTE_LIST;
    janus_attribute_list result = malloc(sizeof(struct janus_attribute_list_type) +
                                         sizeof(janus_attribute) * size +
                                         sizeof(janus_value) * size);
    if (!result) {
        *attribute_list = NULL;
        return JANUS_OUT_OF_MEMORY;
    }
    result->size = size;
    result->attributes = (janus_attribute*)(result + 1);
    result->values = (janus_value*)(result->attributes + size);
    memset(result->attributes, JANUS_INVALID_ATTRIBUTE, sizeof(janus_attribute) * size);
    *attribute_list = result;
    return JANUS_SUCCESS;
}

void janus_free_attribute_list(janus_attribute_list attribute_list)
{
    free(attribute_list);
}

janus_error janus_allocate_object(const janus_size size, janus_object *object)
{
    if (!object) return JANUS_NULL_OBJECT;
    janus_object result = malloc(sizeof(struct janus_object_type) +
                                 sizeof(janus_attribute_list) * size);
    if (!result) {
        *object = NULL;
        return JANUS_OUT_OF_MEMORY;
    }
    result->size = size;
    result->attribute_lists = (janus_attribute_list*)(result + 1);
    memset(result->attribute_lists, 0, sizeof(janus_attribute_list) * size);
    *object = result;
    return JANUS_SUCCESS;
}

void janus_free_object(janus_object object)
{
    if (!object) return;
    for (janus_size i=0; i<object->size; i++)
        janus_free_attribute_list(object->attribute_lists[i]);
    free(object);
}

janus_error janus_allocate_object_list(const janus_size size,
                                       janus_object_list *object_list)
{
    if (!object_list) return JANUS_NULL_OBJECT_LIST;
    janus_object_list result = malloc(sizeof(struct janus_object_list_type) +
                                      sizeof(janus_object) * size);
    if (!result) {
        *object_list = NULL;
        return JANUS_OUT_OF_MEMORY;
    }
    result->size = size;
    result->objects = (janus_object*)(result + 1);
    memset(result->objects, 0, sizeof(janus_object) * size);
    *object_list = result;
    return JANUS_SUCCESS;
}

void janus_free_object_list(janus_object_list object_list)
{
    if (!object_list) return;
    for (janus_size i=0; i<object_list->size; i++)
        janus_free_object(object_list->objects[i]);
    free(object_list);
}
