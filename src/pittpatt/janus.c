#include <stdlib.h>

#include "janus.h"

// The implementation of these functions only rely on the C Standard Library.

const char *janus_error_to_string(janus_error error)
{
    switch (error) {
      case JANUS_SUCCESS:          return "Success";
      case JANUS_UNKNOWN_ERROR:    return "Unknown error";
      case JANUS_OUT_OF_MEMORY:    return "Out of memory";
      case JANUS_INVALID_SDK_PATH: return "Invalid SDK path";
      case JANUS_NULL_CONTEXT:     return "Null context";
      case JANUS_NULL_IMAGE:       return "Null image";
      case JANUS_NULL_OBJECT_LIST: return "Null object list";
      default:                     return "Unrecognized error";
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
    *image = malloc(sizeof(struct janus_image_type) +
                    sizeof(janus_data) * channels * width * height);
    if (!*image) return JANUS_OUT_OF_MEMORY;
    (*image)->channels = channels;
    (*image)->width = width;
    (*image)->height = height;
    (*image)->data = (janus_data*)(*image + 1);
    return JANUS_SUCCESS;
}

janus_object janus_allocate_object(janus_size size)
{
    janus_object object = malloc(sizeof(struct janus_object_type) +
                                 sizeof(janus_attribute) * size +
                                 sizeof(janus_value) * size);
    object->size = size;
    object->attributes = (janus_attribute*)(object + 1);
    object->values = (janus_value*)(object->attributes + size);
    return object;
}

janus_object_list janus_allocate_object_list(janus_size size)
{
    janus_object_list object_list = malloc(sizeof(struct janus_object_list_type) +
                                           sizeof(janus_object) * size);
    object_list->size = size;
    object_list->objects = (janus_object*)(object_list + 1);
    return object_list;
}

void janus_free_image(janus_image image)
{
    free(image);
}

void janus_free_object(janus_object object)
{
    free(object);
}

void janus_free_object_list(janus_object_list object_list)
{
    for (janus_size i=0; i<object_list->size; i++)
        janus_free_object(object_list->objects[i]);
    free(object_list);
}
