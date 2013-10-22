#include <float.h>
#include <math.h>
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
      case JANUS_NULL_VALUE:          return "Null value";
      case JANUS_NULL_ATTRIBUTE_LIST: return "Null attribute list";
      case JANUS_NULL_OBJECT:         return "Null object";
      case JANUS_NULL_OBJECT_LIST:    return "Null object list";
      default:                        return "Unrecognized error code";
    }
}

// These implementations of janus_allocate_* require just one call to malloc
// and simplify the implementations of janus_copy_* and janus_free_*.

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
    if (!object) return JANUS_NULL_OBJECT;
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
    if (!object_list) return JANUS_NULL_OBJECT_LIST;
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
    if (!object) return JANUS_NULL_OBJECT;
    if (!values) return JANUS_NULL_VALUE;

    for (janus_size i=0; i<object->size; i++) {
        janus_error error = janus_get_value(object->attribute_lists[i], attribute, &values[i]);
        if (error != JANUS_SUCCESS)
            return error;
    }

    return JANUS_SUCCESS;
}

janus_error janus_downsample(const janus_object input, janus_object *output)
{
    if (!input || !output) return JANUS_NULL_OBJECT;

    // Arbitrary choice to downsample by a sqrt factor
    const janus_size num_selected = sqrt(input->size);
    janus_size *selected_indicies = malloc(num_selected);

    // A greedy search strategy to select attributes with the high confidence and most dispersed yaw
    janus_value *confidences = malloc(input->size * sizeof(janus_value));
    janus_value *yaws        = malloc(input->size * sizeof(janus_value));
    janus_get_values(input, JANUS_FACE_CONFIDENCE, confidences);
    janus_get_values(input, JANUS_FACE_YAW, yaws);

    // Normalize confidence values to [0,1] range
    janus_value min_confidence = FLT_MAX;
    janus_value max_confidence = -FLT_MAX;
    for (janus_size i=0; i<input->size; i++) {
        if (confidences[i] < min_confidence) min_confidence = confidences[i];
        if (confidences[i] > max_confidence) max_confidence = confidences[i];
    }
    janus_value confidence_range = max_confidence - min_confidence;
    if (confidence_range == 0) confidence_range = 1;
    for (janus_size i=0; i<input->size; i++)
        confidences[i] = (confidences[i] - min_confidence) / confidence_range;

    // Do the search
    for (janus_size i=0; i<num_selected; i++) {
        float best_score = -FLT_MAX;
        int best_index = -1;

        for (janus_size index=0; index<input->size; index++) {
            float widest_yaw_delta = 360;
            for (janus_size k=0; k<i; k++) {
                const float yaw_delta = abs(yaws[index] - yaws[selected_indicies[k]]);
                if (yaw_delta < widest_yaw_delta)
                    widest_yaw_delta = yaw_delta;
            }
            const float score = (confidences[index] + 1) * (widest_yaw_delta / 180 + 1);
            if (score > best_score) {
                best_index = index;
                best_score = score;
            }
        }

        selected_indicies[i] = best_index;
    }

    janus_object result;
    janus_allocate_object(num_selected, &result);
    for (janus_size i=0; i<num_selected; i++)
        janus_copy_attribute_list(input->attribute_lists[selected_indicies[i]], &result->attribute_lists[i]);

    free(yaws);
    free(confidences);
    free(selected_indicies);
    *output = result;
    return JANUS_SUCCESS;
}
