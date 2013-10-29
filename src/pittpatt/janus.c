#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "janus.h"
#include "janus_aux.h"

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
      case JANUS_NULL_VALUE:          return "Null value";
      case JANUS_NULL_ATTRIBUTE_LIST: return "Null attribute list";
      default:                        return "Unrecognized error code";
    }
}

// These implementations of janus_allocate_* require just one call to malloc
// and simplify the implementations of janus_copy_* and janus_free_*.

janus_error janus_allocate_attribute_list(const janus_size size,
                                          janus_attribute_list *attribute_list)
{
    if (!attribute_list) return JANUS_NULL_ATTRIBUTE_LIST;
    *attribute_list = NULL;
    janus_attribute_list result = malloc(sizeof(struct janus_attribute_list_type) +
                                         sizeof(janus_attribute) * size +
                                         sizeof(janus_value) * size);
    if (!result) return JANUS_OUT_OF_MEMORY;
    result->size = size;
    result->attributes = (janus_attribute*)(result + 1);
    result->values = (janus_value*)(result->attributes + size);
    memset(result->attributes, JANUS_INVALID_ATTRIBUTE, sizeof(janus_attribute) * size);
    *attribute_list = result;
    return JANUS_SUCCESS;
}

janus_error janus_copy_attribute_list(const janus_attribute_list input,
                                      janus_attribute_list *output)
{
    if (!input || !output) return JANUS_NULL_ATTRIBUTE_LIST;
    *output = NULL;
    const size_t size = sizeof(struct janus_attribute_list_type) +
                        sizeof(janus_attribute) * input->size +
                        sizeof(janus_value) * input->size;
    janus_attribute_list result = malloc(size);
    if (!result) return JANUS_OUT_OF_MEMORY;
    memcpy(input, result, size);
    *output = result;
    return JANUS_SUCCESS;
}

void janus_free_attribute_list(janus_attribute_list attribute_list)
{
    free(attribute_list);
}

janus_error janus_allocate_object(const janus_size size, janus_object *object)
{
    if (!object) return JANUS_UNKNOWN_ERROR;
    *object = NULL;
    janus_object result = malloc(sizeof(struct janus_object_type) +
                                 sizeof(janus_attribute_list) * size);
    if (!result) return JANUS_OUT_OF_MEMORY;
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
    if (!object_list) return JANUS_UNKNOWN_ERROR;
    *object_list = NULL;
    janus_object_list result = malloc(sizeof(struct janus_object_list_type) +
                                      sizeof(janus_object) * size);
    if (!result) return JANUS_OUT_OF_MEMORY;
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

janus_error janus_get_value(const janus_attribute_list attribute_list, const janus_attribute attribute, janus_value *value)
{
    if (!attribute_list) return JANUS_NULL_ATTRIBUTE_LIST;
    if (!value) return JANUS_NULL_VALUE;

    for (janus_size i=0; i<attribute_list->size; i++) {
        if (attribute_list->attributes[i] == attribute) {
            *value = attribute_list->values[i];
            return JANUS_SUCCESS;
        }
    }

    *value = 0;
    return JANUS_UNKNOWN_ERROR;
}

janus_error janus_get_values(const janus_object object, const janus_attribute attribute, janus_value *values)
{
    if (!object) return JANUS_UNKNOWN_ERROR;
    if (!values) return JANUS_NULL_VALUE;

    for (janus_size i=0; i<object->size; i++) {
        janus_error error = janus_get_value(object->attribute_lists[i], attribute, &values[i]);
        if (error != JANUS_SUCCESS)
            return error;
    }

    return JANUS_SUCCESS;
}
