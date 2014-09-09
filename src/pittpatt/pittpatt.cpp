#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <pittpatt_errors.h>
#include <pittpatt_license.h>
#include <pittpatt_sdk.h>

#include "janus.h"

#include <stdio.h>

static ppr_context_type ppr_context;

static janus_error to_janus_error(ppr_error_type error)
{
    if (error != PPR_SUCCESS)
        printf("PittPatt 5: %s\n", ppr_error_message(error));

    switch (error) {
      case PPR_SUCCESS:                 return JANUS_SUCCESS;
      case PPR_NULL_MODELS_PATH:
      case PPR_INVALID_MODELS_PATH:     return JANUS_INVALID_SDK_PATH;
      case PPR_NULL_IMAGE:
      case PPR_INVALID_RAW_IMAGE:
      case PPR_INCONSISTENT_IMAGE_DIMENSIONS:
                                        return JANUS_INVALID_IMAGE;
      default:                          return JANUS_UNKNOWN_ERROR;
    }
}

#define JANUS_TRY_PPR(PPR_API_CALL)            \
{                                              \
    ppr_error_type ppr_error = (PPR_API_CALL); \
    if (ppr_error != PPR_SUCCESS)              \
        return to_janus_error(ppr_error);      \
}

static ppr_error_type initialize_ppr_context(ppr_context_type *context)
{
    ppr_settings_type settings = ppr_get_default_settings();
    settings.detection.enable = 1;
    settings.detection.min_size = 4;
    settings.detection.max_size = PPR_MAX_MAX_SIZE;
    settings.detection.adaptive_max_size = 1.f;
    settings.detection.adaptive_min_size = 0.01f;
    settings.detection.threshold = 0;
    settings.detection.use_serial_face_detection = 1;
    settings.detection.num_threads = 1;
    settings.detection.search_pruning_aggressiveness = 0;
    settings.detection.detect_best_face_only = 1;
    settings.landmarks.enable = 1;
    settings.landmarks.landmark_range = PPR_LANDMARK_RANGE_COMPREHENSIVE;
    settings.landmarks.manually_detect_landmarks = 0;
    settings.recognition.enable_extraction = 1;
    settings.recognition.enable_comparison = 1;
    settings.recognition.recognizer = PPR_RECOGNIZER_MULTI_POSE;
    settings.recognition.num_comparison_threads = 1;
    settings.recognition.automatically_extract_templates = 0;
    settings.recognition.extract_thumbnails = 0;
    return ppr_initialize_context(settings, context);
}

janus_error janus_initialize(const char *sdk_path, const char *model_file)
{
    (void) model_file;
    const char *models = "/models/";
    const size_t models_path_len = strlen(sdk_path) + strlen(models);
    char *models_path = new char[models_path_len];
    snprintf(models_path, models_path_len, "%s%s", sdk_path, models);

    janus_error error = to_janus_error(ppr_initialize_sdk(models_path, my_license_id, my_license_key));
    free(models_path);

    if (error != JANUS_SUCCESS)
        return error;

    return to_janus_error(initialize_ppr_context(&ppr_context));

    return error;
}

janus_error janus_finalize()
{
    // This should be the same as above?
    // ppr_context_type ppr_context;
    // ppr_finalize_context(ppr_context);

    ppr_finalize_sdk();
    return JANUS_SUCCESS;
}

janus_error janus_allocate(janus_template *template_)
{
    /*
    *template_ = new janus_template_type();
    return JANUS_SUCCESS;*/
    return JANUS_SUCCESS;
}

typedef struct janus_template_type ppr_face_type;

janus_error janus_augment(const janus_image image, const janus_attribute_list attributes, janus_template template_)
{
    ppr_raw_image_type raw_image;

    int channels;
    if (image.color_space == JANUS_BGR24) {
        ppr_raw_image_create(&raw_image, image.width, image.height, PPR_RAW_IMAGE_BGR24);
        channels = 3;
    } else if (image.color_space == JANUS_GRAY8) {
        ppr_raw_image_create(&raw_image, image.width, image.height, PPR_RAW_IMAGE_GRAY8);
        channels = 1;
    }
    memcpy(raw_image.data, image.data, channels*image.height*image.width);

    ppr_image_type ppr_image;
    ppr_create_image(raw_image, &ppr_image);

    // Set attributes with pp5_face_type (?)
    ppr_face_list_type face_list;

    /*
     * TOASK:
     * Can you provide pittpatt with manual landmarks?
     * If so, how?
     * If not, how does janus augment work since it should be assigning attributes to the template?
     * -- janus_augment should not do anything with attributes that are passed to it, simply do detection
     */

    ppr_detect_faces(ppr_context, ppr_image, &face_list);
    // Enroll to template_
    for (int i=0; i<face_list.length; i++) {
        template_ = face_list.faces[i];
        int extractable;
        ppr_is_template_extractable(ppr_context, template_, &extractable);
        if (!extractable) continue;

        ppr_extract_face_template(ppr_context, ppr_image, &template_);
    }

    ppr_free_face_list(face_list);
    ppr_free_image(ppr_image);
    ppr_raw_image_free(raw_image);

    return JANUS_SUCCESS;
}

static ppr_error_type to_ppr_image(const janus_image image, ppr_image_type *ppr_image)
{
    ppr_raw_image_type raw_image;
    raw_image.bytes_per_line = (image.color_space == JANUS_BGR24 ? 3 : 1) * image.width;
    raw_image.color_space = (image.color_space == JANUS_BGR24 ? PPR_RAW_IMAGE_BGR24 : PPR_RAW_IMAGE_GRAY8);
    raw_image.data = image.data;
    raw_image.height = image.height;
    raw_image.width = image.width;
    return ppr_create_image(raw_image, ppr_image);
}

janus_error janus_flatten(janus_template template_, janus_flat_template flat_template, size_t *bytes)
{
    // TODO
}

janus_error janus_free(janus_template template_)
{
    // TODO
}

size_t janus_max_template_size()
{
    // TOCHECK
    return 33554432; // 32 MB
}

janus_error janus_verify(const janus_flat_template a, const size_t a_bytes, const janus_flat_template b, const size_t b_bytes, float *similarity)
{
    // TODO
}

janus_error janus_enroll(const janus_template template_, const janus_template_id template_id, janus_gallery gallery)
{

}

janus_error janus_gallery_size(janus_gallery gallery, size_t *size)
{

}

janus_error janus_search(const janus_template template_, janus_gallery gallery, int requested_returns, janus_template_id *template_ids, float *similarities, int *actual_returns)
{

}

janus_error janus_compare(janus_gallery target, janus_gallery query, float *similarity_matrix, janus_template_id *target_ids, janus_template_id *query_ids)
{

}

static janus_error to_janus_attribute_list(ppr_face_type face, janus_attribute_list *attribute_list, int media_id)
{
    ppr_face_attributes_type face_attributes;
    ppr_get_face_attributes(face, &face_attributes);

    const int num_face_attributes = 11;
    janus_attribute attributes[num_face_attributes];
    double values[num_face_attributes];

    janus_attribute_list result;
    //result.size = num_face_attributes;
    //results.

    //attributes[0] = JANUS_MEDIA_ID;
    //values[0] = media_id;
    attributes[1] = JANUS_FRAME;
    values[1] = face_attributes.tracking_info.frame_number;
    attributes[2] = JANUS_FACE_X;
    values[2] = face_attributes.position.x;
    attributes[3] = JANUS_FACE_Y;
    values[3] = face_attributes.position.y;
    attributes[4] = JANUS_FACE_WIDTH;
    values[4] = face_attributes.dimensions.width;
    attributes[5] = JANUS_FACE_HEIGHT;
    values[5] = face_attributes.dimensions.height;
    attributes[6] = JANUS_FACE_X;
    values[6] = face_attributes.position.x;
    attributes[7] = JANUS_FACE_Y;
    values[7] = face_attributes.position.y;

    ppr_landmark_list_type landmark_list;
    ppr_get_face_landmarks(face, &landmark_list);
    for (int j=0; j<face_attributes.num_landmarks; j++) {
        const int index = num_face_attributes + 2*j;
        switch (landmark_list.landmarks[j].category) {
        case PPR_LANDMARK_CATEGORY_LEFT_EYE:
            attributes[index] = JANUS_LEFT_EYE_X;
            attributes[index+1] = JANUS_LEFT_EYE_Y;
            break;
        case PPR_LANDMARK_CATEGORY_RIGHT_EYE:
            attributes[index] = JANUS_RIGHT_EYE_X;
            attributes[index+1] = JANUS_RIGHT_EYE_Y;
            break;
        case PPR_LANDMARK_CATEGORY_NOSE_BASE:
            attributes[index] = JANUS_NOSE_BASE_X;
            attributes[index+1] = JANUS_NOSE_BASE_Y;
            break;
        default:
            attributes[index] = JANUS_INVALID_ATTRIBUTE;
            attributes[index+1] = JANUS_INVALID_ATTRIBUTE;
            break;
        }
        values[index]   = landmark_list.landmarks[j].position.x;
        values[index+1] = landmark_list.landmarks[j].position.y;
    }
    ppr_free_landmark_list(landmark_list);
    *attribute_list = result;
    return JANUS_SUCCESS;
}

/*
 * To be used in a later phase...
 *
static int media_id_counter = 0; // TODO: This should be an atomic integer

janus_error janus_detect(const janus_context context, const janus_image image, janus_object_list *object_list)
{
    if (!object_list) return JANUS_UNKNOWN_ERROR;
    *object_list = NULL;

    const int media_id = media_id_counter++;

    ppr_image_type ppr_image;
    JANUS_TRY_PPR(to_ppr_image(image, &ppr_image))

    ppr_face_list_type face_list;
    ppr_detect_faces((ppr_context_type)context, ppr_image, &face_list);

    janus_object_list result;
    janus_allocate_object_list(face_list.length, &result);
    for (janus_size i=0; i<result->size; i++) {
        janus_object object;
        janus_allocate_object(1, &object);
        to_janus_attribute_list(face_list.faces[i], &object->attribute_lists[0], media_id);
        result->objects[i] = object;
    }

    ppr_free_face_list(face_list);
    ppr_free_image(ppr_image);
    *object_list = result;
    return JANUS_SUCCESS;
}

janus_error janus_initialize_track(janus_track *track)
{
    ppr_context_type ppr_context;
    ppr_error_type ppr_error = initialize_ppr_context(&ppr_context, 1);
    *track = (janus_track)ppr_context;
    return to_janus_error(ppr_error);
}

janus_error janus_track_frame(const janus_context context, const janus_image frame, janus_track track)
{
    (void) context;
    ppr_image_type ppr_image;
    JANUS_TRY_PPR(to_ppr_image(frame, &ppr_image))
    ppr_face_list_type face_list;
    ppr_error_type ppr_error = ppr_detect_faces((ppr_context_type)track, ppr_image, &face_list);
    ppr_free_face_list(face_list);
    ppr_free_image(ppr_image);
    return to_janus_error(ppr_error);
}

janus_error janus_finalize_track(janus_track track, janus_object_list *object_list)
{
    if (!object_list) return JANUS_UNKNOWN_ERROR;
    *object_list = NULL;

    const int media_id = media_id_counter++;

    ppr_track_list_type ppr_track_list;
    JANUS_TRY_PPR(ppr_finalize_tracks((ppr_context_type)track))
    JANUS_TRY_PPR(ppr_get_completed_tracks((ppr_context_type)track, &ppr_track_list));
    JANUS_TRY_PPR(ppr_finalize_context((ppr_context_type)track));

    janus_object_list result;
    janus_allocate_object_list(ppr_track_list.length, &result);
    for (janus_size i=0; i<result->size; i++) {
        ppr_track_type ppr_track = ppr_track_list.tracks[i];
        janus_object object;
        janus_allocate_object(ppr_track.tracked_faces.length, &object);
        for (janus_size j=0; j<object->size; j++)
            to_janus_attribute_list(ppr_track.tracked_faces.faces[j], &object->attribute_lists[j], media_id);
        result->objects[i] = object;
    }

    *object_list = result;
    return JANUS_SUCCESS;
} */
