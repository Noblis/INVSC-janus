#include <cstring>
#include <limits>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <assert.h>

#include <pittpatt_errors.h>
#include <pittpatt_license.h>
#include <pittpatt_sdk.h>

#include <iarpa_janus.h>
#include <iarpa_janus_io.h>

using namespace std;

ppr_context_type ppr_context;

struct janus_template_type
{
    vector<ppr_face_list_type> ppr_face_lists;
};

struct janus_gallery_type
{
    ppr_gallery_type ppr_gallery;
};

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
                                        return JANUS_INVALID_MEDIA;
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
    return ppr_initialize_context(settings, context);
}

janus_error janus_initialize(const string &sdk_path, const string &, const string &, const int)
{
    janus_error error = to_janus_error(ppr_initialize_sdk((sdk_path + "/models/").c_str(), my_license_id, my_license_key));
    if (error != JANUS_SUCCESS)
        return error;
    return to_janus_error(initialize_ppr_context(&ppr_context));
}

janus_error janus_finalize()
{
    janus_error error = to_janus_error(ppr_finalize_context(ppr_context));
    ppr_finalize_sdk();

    return error;
}

static ppr_error_type to_ppr_media(const janus_media media, vector<ppr_image_type> &ppr_media)
{
    for (size_t i = 0; i < media.data.size(); i++) {
        ppr_raw_image_type raw_image;
        raw_image.bytes_per_line = (media.color_space == JANUS_BGR24 ? 3 : 1) * media.width;
        raw_image.color_space = (media.color_space == JANUS_BGR24 ? PPR_RAW_IMAGE_BGR24 : PPR_RAW_IMAGE_GRAY8);
        raw_image.data = media.data[i];
        raw_image.height = media.height;
        raw_image.width = media.width;

        ppr_image_type ppr_image;
        ppr_error_type error = ppr_create_image(raw_image, &ppr_image);
        if (error != PPR_SUCCESS)
            return error;
        ppr_media.push_back(ppr_image);
    }

    return PPR_SUCCESS;
}

struct sort_first_greater {
    bool operator()(const std::pair<float,janus_template_id> &left, const std::pair<float,janus_template_id> &right) {
        return left.first > right.first;
    }
    bool operator()(const std::pair<float,ppr_face_attributes_type> &left, const std::pair<float,ppr_face_attributes_type> &right) {
        return left.first > right.first;
    }
};

janus_error janus_detect(const janus_media &media, const size_t min_face_size, std::vector<janus_track> &tracks)
{
    vector<ppr_image_type> ppr_media;
    JANUS_TRY_PPR(to_ppr_media(media, ppr_media))

    for (size_t i = 0; i < ppr_media.size(); i++) {
        ppr_face_list_type face_list;
        ppr_detect_faces(ppr_context, ppr_media[i], &face_list);

        vector<pair<float, ppr_face_attributes_type> > face_confidences;
        for (int j = 0; j < face_list.length; j++) {
            ppr_face_type face = face_list.faces[j];
            ppr_face_attributes_type face_attributes;
            JANUS_TRY_PPR(ppr_get_face_attributes(face, &face_attributes))
            face_confidences.push_back(make_pair(face_attributes.confidence, face_attributes));
        }

        // Sort by confidence, descending
        sort(face_confidences.begin(), face_confidences.end(), sort_first_greater());

        for (size_t j = 0; j < face_confidences.size(); j++) {
            janus_track track;
            janus_attributes attributes;
            attributes.face_x = face_confidences[j].second.position.x - face_confidences[i].second.dimensions.width/2;
            attributes.face_y = face_confidences[j].second.position.y - face_confidences[i].second.dimensions.height/2;
            attributes.face_width = face_confidences[j].second.dimensions.width;
            if (attributes.face_width < (int)min_face_size)
                continue;

            attributes.face_height = face_confidences[j].second.dimensions.height;
            attributes.frame_number = i;
            track.track.push_back(attributes);
            track.detection_confidence = face_confidences[i].first;
            tracks.push_back(track);
        }

        ppr_free_face_list(face_list);
    }

    for (size_t i = 0; i < ppr_media.size(); i++)
        ppr_free_image(ppr_media[i]);

    return JANUS_SUCCESS;
}

janus_media janus_crop_media(const janus_association &association)
{
    const janus_media &src = association.media;
    const janus_track &track = association.metadata;

    janus_media dst; dst.data.reserve(src.data.size());
    for (size_t i = 0; i < src.data.size(); i++) {
        const janus_attributes &face_attributes = track.track[i];
        const size_t x = face_attributes.face_x < 0 ? 0 : face_attributes.face_x;
        const size_t y = face_attributes.face_y < 0 ? 0 : face_attributes.face_y;
        dst.width = (x + face_attributes.face_width > src.width) ? src.width - x : face_attributes.face_width;
        dst.height = (y + face_attributes.face_height > src.height) ? src.height - y : face_attributes.face_height;
        dst.color_space = src.color_space;
        int channels = src.color_space == JANUS_BGR24 ? 3 : 1;

        janus_data *data = new janus_data[dst.width * dst.height * channels];
        const unsigned long dst_elements_per_row = dst.width * channels;
        const unsigned long src_elements_per_row = src.width * channels;

        for (size_t j = 0; j < dst.height; j++)
            memcpy(data + j*dst_elements_per_row, src.data[i] + src_elements_per_row * (y + j) + channels*x, dst_elements_per_row);
        dst.data.push_back(data);
    }
    return dst;
}

janus_error janus_create_template(const std::vector<janus_association> &associations, const janus_template_role, janus_template &template_)
{
    template_ = new janus_template_type();

    for (size_t i = 0; i < associations.size(); i++) {
        janus_media cropped = janus_crop_media(associations[i]);

        vector<ppr_image_type> ppr_media;
        to_ppr_media(cropped, ppr_media);

        for (size_t j = 0; j < ppr_media.size(); j++) {
            ppr_face_list_type face_list;
            JANUS_TRY_PPR(ppr_detect_faces(ppr_context, ppr_media[j], &face_list))

            for (int k = 0; k < face_list.length; k++) {
                ppr_face_type &face = face_list.faces[k];

                int extractable;
                JANUS_TRY_PPR(ppr_is_template_extractable(ppr_context, face, &extractable))
                if (extractable) { // Only extract a single face template
                    ppr_extract_face_template(ppr_context, ppr_media[j], &face);
                    break;
                }
            }

            template_->ppr_face_lists.push_back(face_list);
        }

        for (size_t i = 0; i < ppr_media.size(); i++)
            ppr_free_image(ppr_media[i]);
        janus_free_media(cropped);
    }

    return JANUS_SUCCESS;
}

janus_error janus_create_template(const janus_media &, const janus_template_role, vector<janus_template> &, vector<janus_track> &)
{
    return JANUS_NOT_IMPLEMENTED;
}

janus_error janus_serialize_template(const janus_template &template_, janus_data *&data, size_t &bytes)
{
    bytes = 0;

    vector<ppr_flat_data_type> data_list;
    for (size_t i = 0; i < template_->ppr_face_lists.size(); i++) {
        const ppr_face_list_type &face_list = template_->ppr_face_lists[i];

        ppr_flat_data_type flat_data;
        JANUS_TRY_PPR(ppr_flatten_face_list(ppr_context, face_list, &flat_data))
        data_list.push_back(flat_data);

        bytes += sizeof(size_t);
        bytes += flat_data.length;
    }

    data = new janus_data[bytes];
    for (size_t i = 0; i < data_list.size(); i++) {
        const ppr_flat_data_type &flat_data = data_list[i];

        const size_t templateBytes = flat_data.length;
        memcpy(data, &templateBytes, sizeof(size_t));
        data += sizeof(size_t);

        memcpy(data, flat_data.data, templateBytes);
        data += templateBytes;
    }
    data -= bytes;

    return JANUS_SUCCESS;
}

janus_error janus_deserialize_template(const janus_data *data, const size_t template_bytes, janus_template &template_)
{
    template_ = new janus_template_type();

    const janus_data *pointer = data;
    while (pointer < data + template_bytes) {
        const size_t templateBytes = *reinterpret_cast<const size_t*>(pointer);
        pointer += sizeof(size_t);

        ppr_flat_data_type flat_data;
        JANUS_TRY_PPR(ppr_create_flat_data(templateBytes, &flat_data))
        memcpy(flat_data.data, pointer, templateBytes);

        ppr_face_list_type face_list;
        JANUS_TRY_PPR(ppr_unflatten_face_list(ppr_context, flat_data, &face_list))

        template_->ppr_face_lists.push_back(face_list);

        ppr_free_flat_data(flat_data);

        pointer += templateBytes;
    }

    return JANUS_SUCCESS;
}

janus_error janus_delete_serialized_template(janus_data *&template_, const size_t)
{
    delete[] template_;
    return JANUS_SUCCESS;
}

janus_error janus_delete_template(janus_template &template_)
{
    for (size_t i = 0; i < template_->ppr_face_lists.size(); i++)
        ppr_free_face_list(template_->ppr_face_lists[i]);
    template_->ppr_face_lists.clear();
    delete template_;
    return JANUS_SUCCESS;
}

janus_error janus_verify(const janus_template &reference, const janus_template &verification, double &similarity)
{
    // Set the default similarity score to be a rejection score (for galleries that don't contain faces)
    similarity = -1.5;

    // Pittpatt compares galleries so we convert our templates to galleries with size 1
    ppr_gallery_type gallery_a;
    JANUS_TRY_PPR(ppr_create_gallery(ppr_context, &gallery_a))

    int face_id = 0;
    for (size_t i = 0; i < reference->ppr_face_lists.size(); i++) {
        const ppr_face_list_type &face_list = reference->ppr_face_lists[i];
        for (int j = 0; j < face_list.length; j++) {
            ppr_face_type face = face_list.faces[j];

            int has_template;
            JANUS_TRY_PPR(ppr_face_has_template(ppr_context, face, &has_template))
            if (!has_template)
                continue;

            JANUS_TRY_PPR(ppr_add_face(ppr_context, &gallery_a, face, 0, face_id++))
        }
    }

    ppr_gallery_type gallery_b;
    JANUS_TRY_PPR(ppr_create_gallery(ppr_context, &gallery_b))

    face_id = 0;
    for (size_t i = 0; i < verification->ppr_face_lists.size(); i++) {
        const ppr_face_list_type &face_list = verification->ppr_face_lists[i];
        for (int j = 0; j < face_list.length; j++) {
            ppr_face_type face = face_list.faces[j];

            int has_template;
            JANUS_TRY_PPR(ppr_face_has_template(ppr_context, face, &has_template))
            if (!has_template)
                continue;

            JANUS_TRY_PPR(ppr_add_face(ppr_context, &gallery_b, face, 0, face_id++))
        }
    }

    ppr_similarity_matrix_type simmat;
    JANUS_TRY_PPR(ppr_compare_galleries(ppr_context, gallery_a, gallery_b, &simmat))
    float fsimilarity;
    JANUS_TRY_PPR(ppr_get_subject_similarity_score(ppr_context, simmat, 0, 0, &fsimilarity))
    similarity = fsimilarity + 1.5;

    ppr_free_gallery(gallery_a);
    ppr_free_gallery(gallery_b);
    ppr_free_similarity_matrix(simmat);

    if (similarity != similarity) // True for NaN
        return JANUS_UNKNOWN_ERROR;

    return JANUS_SUCCESS;
}

janus_error janus_create_gallery(const vector<janus_template> &templates, const vector<janus_template_id> &ids, janus_gallery &gallery)
{
    gallery = new janus_gallery_type();
    JANUS_TRY_PPR(ppr_create_gallery(ppr_context, &gallery->ppr_gallery));

    int face_id = 0;
    for (size_t i = 0; i < templates.size(); i++) {
        const vector<ppr_face_list_type> &face_lists = templates[i]->ppr_face_lists;
        for (size_t j = 0; j < face_lists.size(); j++) {
            const ppr_face_list_type &face_list = face_lists[j];
            for (int k = 0; k < face_list.length; k++) {
                const ppr_face_type &face = face_list.faces[k];

                int has_template;
                JANUS_TRY_PPR(ppr_face_has_template(ppr_context, face, &has_template))
                if (!has_template)
                    continue;

                JANUS_TRY_PPR(ppr_add_face(ppr_context, &gallery->ppr_gallery, face, ids[i], face_id++))
            }
        }
    }

    return JANUS_SUCCESS;
}

janus_error janus_serialize_gallery(const janus_gallery &gallery, janus_data *&data, size_t &gallery_bytes)
{
    gallery_bytes = 0;

    ppr_flat_data_type flat_data;
    JANUS_TRY_PPR(ppr_flatten_gallery(ppr_context, gallery->ppr_gallery, &flat_data))

    data = new janus_data[flat_data.length];
    memcpy(data, flat_data.data, flat_data.length);

    gallery_bytes = flat_data.length;

    return JANUS_SUCCESS;
}

janus_error janus_deserialize_gallery(const janus_data *data, const size_t gallery_bytes, janus_gallery &gallery)
{
    gallery = new janus_gallery_type();

    ppr_flat_data_type flat_data;
    JANUS_TRY_PPR(ppr_create_flat_data(gallery_bytes, &flat_data))
    memcpy(flat_data.data, data, gallery_bytes);

    JANUS_TRY_PPR(ppr_unflatten_gallery(ppr_context, flat_data, &gallery->ppr_gallery))

    return JANUS_SUCCESS;
}

janus_error janus_prepare_gallery(janus_gallery &)
{
    return JANUS_SUCCESS;
}

janus_error janus_gallery_insert(janus_gallery &gallery, const janus_template &template_, const janus_template_id &id)
{
    int face_id = 0;
    for (size_t i = 0; i < template_->ppr_face_lists.size(); i++) {
        const ppr_face_list_type &face_list = template_->ppr_face_lists[i];
        for (int j = 0; j < face_list.length; j++) {
            const ppr_face_type &face = face_list.faces[j];

            int has_template;
            JANUS_TRY_PPR(ppr_face_has_template(ppr_context, face, &has_template))
            if (!has_template)
                continue;

            JANUS_TRY_PPR(ppr_add_face(ppr_context, &gallery->ppr_gallery, face, id, face_id++))
        }
    }

    return JANUS_SUCCESS;
}

janus_error janus_gallery_remove(janus_gallery &gallery, const janus_template_id &id)
{
    JANUS_TRY_PPR(ppr_remove_subject(ppr_context, &gallery->ppr_gallery, id))
    return JANUS_SUCCESS;
}

janus_error janus_delete_serialized_gallery(janus_data *&gallery, const size_t)
{
    delete[] gallery;
    return JANUS_SUCCESS;
}

janus_error janus_delete_gallery(janus_gallery &gallery)
{
    ppr_free_gallery(gallery->ppr_gallery);
    delete gallery;
    return JANUS_SUCCESS;
}

janus_error janus_search(const janus_template &probe, const janus_gallery &gallery, const size_t num_requested_returns, vector<janus_template_id> &template_ids, vector<double> &similarities)
{
    ppr_gallery_type probe_gallery;
    JANUS_TRY_PPR(ppr_create_gallery(ppr_context, &probe_gallery))

    int face_id = 0;
    for (size_t i = 0; i < probe->ppr_face_lists.size(); i++) {
        const ppr_face_list_type &face_list = probe->ppr_face_lists[i];
        for (int j = 0; j < face_list.length; j++) {
            ppr_face_type face = face_list.faces[j];

            int has_template;
            JANUS_TRY_PPR(ppr_face_has_template(ppr_context, face, &has_template))
            if (!has_template)
                continue;

            JANUS_TRY_PPR(ppr_add_face(ppr_context, &probe_gallery, face, 0, face_id++))
        }
    }

    // No faces found in the probe template
    if (face_id == 0)
        return JANUS_SUCCESS;

    ppr_similarity_matrix_type simmat;
    JANUS_TRY_PPR(ppr_compare_galleries(ppr_context, probe_gallery, gallery->ppr_gallery, &simmat))

    ppr_id_list_type id_list;
    JANUS_TRY_PPR(ppr_get_subject_id_list(ppr_context, gallery->ppr_gallery, &id_list))

    vector<pair<float, janus_template_id> > scores;

    for (int i = 0; i < id_list.length; i++) {
        int target_subject_id = id_list.ids[i];
        float score;
        JANUS_TRY_PPR(ppr_get_subject_similarity_score(ppr_context, simmat, 0, target_subject_id, &score))
        scores.push_back(make_pair(score + 1.5, target_subject_id));
    }

    ppr_free_id_list(id_list);
    ppr_free_gallery(probe_gallery);
    ppr_free_similarity_matrix(simmat);

    sort(scores.begin(), scores.end(), sort_first_greater());

    const size_t keep = std::min(scores.size(), num_requested_returns);
    template_ids.reserve(keep); similarities.reserve(keep);
    for (size_t i = 0; i < keep; i++) {
        template_ids.push_back(scores[i].second);
        similarities.push_back((double)scores[i].first);
    }

    return JANUS_SUCCESS;
}
