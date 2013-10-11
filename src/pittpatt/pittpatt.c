#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <pittpatt_errors.h>
#include <pittpatt_license.h>
#include <pittpatt_sdk.h>

#include "janus.h"

static janus_error to_janus_error(ppr_error_type error)
{
    if (error != PPR_SUCCESS)
        printf("PittPatt 5: %s\n", ppr_error_message(error));

    switch (error) {
      case PPR_SUCCESS:                 return JANUS_SUCCESS;
      case PPR_NULL_CONTEXT:
      case PPR_CORRUPT_CONTEXT:
      case PPR_CONTEXT_NOT_INITIALIZED: return JANUS_NULL_CONTEXT;
      case PPR_NULL_IMAGE:              return JANUS_NULL_IMAGE;
      case PPR_NULL_FACE:               return JANUS_NULL_OBJECT;
      case PPR_NULL_MODELS_PATH:
      case PPR_INVALID_MODELS_PATH:     return JANUS_INVALID_SDK_PATH;
      default:                          return JANUS_UNKNOWN_ERROR;
    }
}

janus_error janus_initialize(const char *sdk_path)
{
    const char *models = "/models/";
    const size_t models_path_len = strlen(sdk_path) + strlen(models);
    char *models_path = malloc(models_path_len);
    snprintf(models_path, models_path_len, "%s%s", sdk_path, models);

    janus_error error = to_janus_error(ppr_initialize_sdk(models_path, my_license_id, my_license_key));
    free(models_path);
    return error;
}

void janus_finalize()
{
    ppr_finalize_sdk();
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
    settings.detection.detect_best_face_only = 0;
    settings.landmarks.enable = 1;
    settings.landmarks.landmark_range = PPR_LANDMARK_RANGE_COMPREHENSIVE;
    settings.landmarks.manually_detect_landmarks = 0;
    settings.recognition.enable_extraction = 1;
    settings.recognition.enable_comparison = 1;
    settings.recognition.recognizer = PPR_RECOGNIZER_MULTI_POSE;
    settings.recognition.num_comparison_threads = 1;
    settings.recognition.automatically_extract_templates = 0;
    settings.recognition.extract_thumbnails = 0;
    settings.tracking.enable = 1;
    settings.tracking.cutoff = 0;
    settings.tracking.discard_completed_tracks = 0;
    settings.tracking.enable_shot_boundary_detection = 1;
    settings.tracking.shot_boundary_threshold = 0;
    return ppr_initialize_context(settings, context);
}

janus_error janus_initialize_context(janus_context *context)
{
    if (!context) return JANUS_NULL_CONTEXT;
    ppr_context_type ppr_context;
    ppr_error_type ppr_error = initialize_ppr_context(&ppr_context);
    *context = (janus_context)ppr_context;
    return to_janus_error(ppr_error);
}

void janus_finalize_context(janus_context context)
{
    if (!context) return;
    ppr_finalize_context((ppr_context_type)context);
}

janus_error janus_detect(const janus_context context, const janus_image image, janus_object_list *object_list)
{
    if (!object_list) return JANUS_NULL_OBJECT_LIST;
    *object_list = NULL;
    if (!context) return JANUS_NULL_CONTEXT;
    if (!image) return JANUS_NULL_IMAGE;

    ppr_raw_image_type raw_image;
    raw_image.bytes_per_line = image->channels * image->width;
    raw_image.color_space = (image->channels == 1 ? PPR_RAW_IMAGE_GRAY8 : PPR_RAW_IMAGE_BGR24);
    raw_image.data = image->data;
    raw_image.height = image->height;
    raw_image.width = image->width;

    ppr_image_type ppr_image;
    ppr_create_image(raw_image, &ppr_image);

    ppr_face_list_type face_list;
    ppr_detect_faces((ppr_context_type)context, ppr_image, &face_list);

    janus_object_list result;
    janus_allocate_object_list(face_list.length, &result);
    for (janus_size i=0; i<result->size; i++) {
        ppr_face_type face = face_list.faces[i];
        ppr_face_attributes_type face_attributes;
        ppr_get_face_attributes(face, &face_attributes);

        const int num_face_attributes = 8;
        janus_attribute_list attribute_list;
        janus_allocate_attribute_list(num_face_attributes + 2*face_attributes.num_landmarks, &attribute_list);
        attribute_list->attributes[0] = JANUS_FACE_CONFIDENCE;
        attribute_list->values[0] = face_attributes.confidence;
        attribute_list->attributes[1] = JANUS_FACE_WIDTH;
        attribute_list->values[1] = face_attributes.dimensions.width;
        attribute_list->attributes[2] = JANUS_FACE_HEIGHT;
        attribute_list->values[2] = face_attributes.dimensions.height;
        attribute_list->attributes[3] = JANUS_FACE_X;
        attribute_list->values[3] = face_attributes.position.x;
        attribute_list->attributes[4] = JANUS_FACE_Y;
        attribute_list->values[4] = face_attributes.position.y;
        attribute_list->attributes[5] = JANUS_FACE_ROLL;
        attribute_list->values[5] = face_attributes.rotation.roll;
        attribute_list->attributes[6] = JANUS_FACE_PITCH;
        attribute_list->values[6] = face_attributes.rotation.pitch;
        attribute_list->attributes[7] = JANUS_FACE_YAW;
        attribute_list->values[7] = face_attributes.rotation.yaw;

        ppr_landmark_list_type landmark_list;
        ppr_get_face_landmarks(face, &landmark_list);
        for (int j=0; j<face_attributes.num_landmarks; j++) {
            const int index = num_face_attributes + 2*j;
            switch (landmark_list.landmarks[j].category) {
              case PPR_LANDMARK_CATEGORY_LEFT_EYE:
                attribute_list->attributes[index] = JANUS_LEFT_EYE_X;
                attribute_list->attributes[index+1] = JANUS_LEFT_EYE_Y;
                break;
              case PPR_LANDMARK_CATEGORY_RIGHT_EYE:
                attribute_list->attributes[index] = JANUS_RIGHT_EYE_X;
                attribute_list->attributes[index+1] = JANUS_RIGHT_EYE_Y;
                break;
              case PPR_LANDMARK_CATEGORY_NOSE_BASE:
                attribute_list->attributes[index] = JANUS_NOSE_BASE_X;
                attribute_list->attributes[index+1] = JANUS_NOSE_BASE_Y;
                break;
              case PPR_LANDMARK_CATEGORY_NOSE_BRIDGE:
                attribute_list->attributes[index] = JANUS_NOSE_BRIDGE_X;
                attribute_list->attributes[index+1] = JANUS_NOSE_BRIDGE_Y;
                break;
              case PPR_LANDMARK_CATEGORY_EYE_NOSE:
                attribute_list->attributes[index] = JANUS_EYE_NOSE_X;
                attribute_list->attributes[index+1] = JANUS_EYE_NOSE_Y;
                break;
              case PPR_LANDMARK_CATEGORY_LEFT_UPPER_CHEEK:
                attribute_list->attributes[index] = JANUS_LEFT_UPPER_CHEEK_X;
                attribute_list->attributes[index+1] = JANUS_LEFT_UPPER_CHEEK_Y;
                break;
              case PPR_LANDMARK_CATEGORY_LEFT_LOWER_CHEEK:
                attribute_list->attributes[index] = JANUS_LEFT_LOWER_CHEEK_X;
                attribute_list->attributes[index+1] = JANUS_LEFT_LOWER_CHEEK_Y;
                break;
              case PPR_LANDMARK_CATEGORY_RIGHT_UPPER_CHEEK:
                attribute_list->attributes[index] = JANUS_RIGHT_UPPER_CHEEK_X;
                attribute_list->attributes[index+1] = JANUS_RIGHT_UPPER_CHEEK_Y;
                break;
              case PPR_LANDMARK_CATEGORY_RIGHT_LOWER_CHEEK:
                attribute_list->attributes[index] = JANUS_RIGHT_LOWER_CHEEK_X;
                attribute_list->attributes[index+1] = JANUS_RIGHT_LOWER_CHEEK_Y;
                break;
              case PPR_NUM_LANDMARK_CATEGORIES:
                attribute_list->attributes[index] = JANUS_INVALID_ATTRIBUTE;
                attribute_list->attributes[index+1] = JANUS_INVALID_ATTRIBUTE;
                break;
            }
            attribute_list->values[index]   = landmark_list.landmarks[j].position.x;
            attribute_list->values[index+1] = landmark_list.landmarks[j].position.y;
        }
        ppr_free_landmark_list(landmark_list);

        janus_object object;
        janus_allocate_object(1, &object);
        object->attribute_lists[0] = attribute_list;
        result->objects[i] = object;
    }

    ppr_free_face_list(face_list);
    ppr_free_image(ppr_image);
    *object_list = result;
    return JANUS_SUCCESS;
}

janus_error janus_initialize_track(janus_track *track)
{
    ppr_context_type context = NULL;
    *track = (janus_track)context;
    return JANUS_SUCCESS;
}

janus_error janus_track_frame(const janus_context context, const janus_image frame, janus_track *track)
{
    (void) context;
    (void) frame;
    (void) track;
    return JANUS_SUCCESS;
}

janus_error janus_finalize_track(janus_track track, janus_object_list *object_list)
{
    (void) track;
    janus_allocate_object_list(0, object_list);
    return JANUS_SUCCESS;
}
