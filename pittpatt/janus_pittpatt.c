#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <pittpatt_errors.h>
#include <pittpatt_license.h>
#include <pittpatt_sdk.h>

#include "janus.h"

ppr_context_type context;

janus_error janus_initialize(const char *sdk_path)
{
    const char *models = "/models/";
    const size_t models_path_len = strlen(sdk_path) + strlen(models);
    char *models_path = malloc(models_path_len);
    snprintf(models_path, models_path_len, "%s%s", sdk_path, models);

    ppr_error_type error = ppr_initialize_sdk(models_path, my_license_id, my_license_key);
    if (error != PPR_SUCCESS)
        return (error == PPR_INVALID_MODELS_PATH) ? janus_invalid_sdk_path : janus_unknown_error;

    ppr_settings_type settings = ppr_get_default_settings();
    settings.detection.enable = 1;
    settings.detection.min_size = 4;
    settings.detection.max_size = INT_MAX;
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
    settings.recognition.automatically_extract_templates = 1;
    settings.recognition.extract_thumbnails = 0;
    ppr_initialize_context(settings, &context);

    free(models_path);
    return janus_success;
}

void janus_finalize()
{
    ppr_finalize_context(context);
    ppr_finalize_sdk();
}
