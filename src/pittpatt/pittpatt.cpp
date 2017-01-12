#include <cstring>
#include <limits>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <assert.h>

#include "pittpatt_errors.h"
#include "pittpatt_license.h"
#include "pittpatt_sdk.h"

#include <iarpa_janus.h>
#include <iarpa_janus_io.h>
#include <opencv_io.h>
#include <opencv2/highgui/highgui.hpp>

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

ppr_error_type make_ppr_image(ppr_image_type *ppr_image, uint8_t *data, const uint32_t channels, const uint32_t width, const uint32_t height)
{
    ppr_raw_image_type raw_image;
    raw_image.bytes_per_line = channels * width;
    raw_image.color_space = (channels == 3 ? PPR_RAW_IMAGE_BGR24 : PPR_RAW_IMAGE_GRAY8);
    raw_image.data = data;
    raw_image.height = height;
    raw_image.width = width;

    ppr_error_type error = ppr_create_image(raw_image, ppr_image);
    if (error != PPR_SUCCESS)
        return error;

    return PPR_SUCCESS;
}

uint8_t *crop_frame(janus_media_type &media,
                    uint32_t &x, uint32_t &y, uint32_t &w, uint32_t &h) // IN (requested ROI) /OUT (actual ROI)
{
    cv::Mat tmp;

    int channels = media.channels; 

    // determine ROI
    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;
    w = (x + w > media.width ) ? media.width  - x : w;
    h = (y + h > media.height) ? media.height - y : h;
    cv::Rect roi(x, y, w, h);

    // get full frame
    media.next(tmp);

    // crop the frame
    tmp = tmp(roi).clone();

    // copy into a buffer we control
    uint8_t *data = new uint8_t[w * h * channels];
    memcpy(data, tmp.data, w * h * channels);

    return data;
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

struct sort_first_greater {
    bool operator()(const std::pair<float,uint32_t> &left, const std::pair<float,uint32_t> &right) {
        return left.first > right.first;
    }
    bool operator()(const std::pair<float,ppr_face_attributes_type> &left, const std::pair<float,ppr_face_attributes_type> &right) {
        return left.first > right.first;
    }
};

janus_error janus_detect(const janus_media &media, const size_t min_face_size, std::vector<janus_track> &tracks)
{
    janus_media_type &m = *media;

    ppr_context_type *context = &ppr_context;

    for (size_t i = 0; i < m.frames; i++) {
        cv::Mat tmp;
        ppr_image_type ppr_image;
        ppr_face_list_type face_list;

        // get frame
        m.next(tmp);
        JANUS_TRY_PPR(make_ppr_image(&ppr_image, tmp.data, m.channels, m.width, m.height));
        ppr_detect_faces(ppr_context, ppr_image, &face_list);

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
            attributes.face_x = (int) (face_confidences[j].second.position.x - face_confidences[j].second.dimensions.width/2.f);
            attributes.face_y = (int) (face_confidences[j].second.position.y - face_confidences[j].second.dimensions.height/2.f);
            attributes.face_width = (int) face_confidences[j].second.dimensions.width;
            if (attributes.face_width < (int)min_face_size)
                continue;

            attributes.face_height = (int) face_confidences[j].second.dimensions.height;
            attributes.frame_number = i;
            track.track.push_back(attributes);
            track.detection_confidence = face_confidences[j].first;
            tracks.push_back(track);
        }

        ppr_free_face_list(face_list);
        ppr_free_image(ppr_image);
    }

    return JANUS_SUCCESS;
}

janus_error janus_create_template(std::vector<janus_track> &tracks, const janus_template_role, janus_template &template_)
{
    template_ = new janus_template_type();

    for (size_t i = 0; i < tracks.size(); i++) {
        janus_media_type &media = *(tracks[i].media);
        vector<janus_attributes> &metadata_vec = tracks[i].track;
        for (size_t j = 0; j < media.frames; j++) {
            const janus_attributes &fa = metadata_vec[j];
            uint32_t face_x, face_y, face_w, face_h;
            uint8_t *data;
            ppr_image_type ppr_image;

            // Get default dimensions
            // crop_frame(m,x,y,w,h) may change these values if the 
            // initial request is outside the bounds of the image
            face_x = fa.face_x ;     face_y = fa.face_y;
            face_w = fa.face_width ; face_h = fa.face_height;

            data = crop_frame(media, face_x, face_y, face_w, face_h);
            JANUS_TRY_PPR(make_ppr_image(&ppr_image, data, media.channels, face_w, face_h));

            ppr_face_list_type face_list;
            JANUS_TRY_PPR(ppr_detect_faces(ppr_context, ppr_image, &face_list))

            for (int k = 0; k < face_list.length; k++) {
                ppr_face_type &face = face_list.faces[k];

                int extractable;
                JANUS_TRY_PPR(ppr_is_template_extractable(ppr_context, face, &extractable))
                if (extractable) { // Only extract a single face template
                    ppr_extract_face_template(ppr_context, ppr_image, &face);
                    break;
                }
            }

            template_->ppr_face_lists.push_back(face_list);

            // clean up
            ppr_free_image(ppr_image);
            delete[] data;

            // only support images for now
            break;
        }
    }
    return JANUS_SUCCESS;
}

janus_error janus_create_template(const janus_media &, const janus_template_role, vector<janus_template> &, vector<janus_track> &)
{
    return JANUS_NOT_IMPLEMENTED;
}


janus_error janus_serialize_template(const janus_template &template_, std::ostream &stream) 
{
    size_t bytes = 0;

    vector<ppr_flat_data_type> data_list;
    for (size_t i = 0; i < template_->ppr_face_lists.size(); i++) {
        const ppr_face_list_type &face_list = template_->ppr_face_lists[i];

        ppr_flat_data_type flat_data;
        JANUS_TRY_PPR(ppr_flatten_face_list(ppr_context, face_list, &flat_data))
        data_list.push_back(flat_data);

        bytes += sizeof(size_t);
        bytes += flat_data.length;
    }

    // total bytes (not including itself)
    stream.write(reinterpret_cast<const char *>(&bytes), sizeof(size_t));

    for (size_t i = 0; i < data_list.size(); i++) {
        const ppr_flat_data_type &flat_data = data_list[i];

        const size_t templateBytes = flat_data.length;

        // size of this template
        stream.write(reinterpret_cast<const char *>(&templateBytes), sizeof(size_t));
        // the template
        stream.write(reinterpret_cast<const char *>(flat_data.data), templateBytes);

        // free the template data
        ppr_free_flat_data(flat_data);
    }

    return JANUS_SUCCESS;
}

janus_error janus_deserialize_template(janus_template &template_, std::istream &stream)
{
    size_t total_bytes = 0;
    size_t template_size = 0;

    template_ = new janus_template_type();

    // total bytes
    stream.read(reinterpret_cast<char *>(&total_bytes), sizeof(size_t));

    while (total_bytes > 0) {
        // read size of template
        stream.read(reinterpret_cast<char *>(&template_size), sizeof(size_t));
        total_bytes -= sizeof(size_t);

        if (total_bytes == 0) return JANUS_FAILURE_TO_DESERIALIZE;

        // read template
        ppr_flat_data_type flat_data;
        JANUS_TRY_PPR(ppr_create_flat_data(template_size, &flat_data))
        stream.read(reinterpret_cast<char *>(flat_data.data), template_size);
        total_bytes -= template_size;

        // add to face list
        ppr_face_list_type face_list;
        JANUS_TRY_PPR(ppr_unflatten_face_list(ppr_context, flat_data, &face_list))
        template_->ppr_face_lists.push_back(face_list);
        ppr_free_flat_data(flat_data);
    }
    return JANUS_SUCCESS;
}

janus_error janus_delete_serialized_template(uint8_t *&template_, const size_t)
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

janus_error janus_create_gallery(const vector<janus_template> &templates, const vector<uint32_t> &ids, janus_gallery &gallery)
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

janus_error janus_serialize_gallery(const janus_gallery &gallery, std::ostream &stream)
{
    size_t gallery_bytes = 0;

    ppr_flat_data_type flat_data;
    JANUS_TRY_PPR(ppr_flatten_gallery(ppr_context, gallery->ppr_gallery, &flat_data))

    gallery_bytes = flat_data.length;

    // gallery_size
    stream.write(reinterpret_cast<const char *>(&gallery_bytes), sizeof(size_t));

    // gallery
    stream.write(reinterpret_cast<const char *>(flat_data.data), gallery_bytes);

    ppr_free_flat_data(flat_data);

    return JANUS_SUCCESS;
}

janus_error janus_deserialize_gallery(janus_gallery &gallery, std::istream &stream)
{
    size_t gallery_bytes = 0;
    gallery = new janus_gallery_type();

    // get the size of the gallery
    stream.read(reinterpret_cast<char *>(&gallery_bytes), sizeof(size_t));

    // read the gallery
    ppr_flat_data_type flat_data;
    JANUS_TRY_PPR(ppr_create_flat_data(gallery_bytes, &flat_data))
    stream.read(reinterpret_cast<char *>(flat_data.data), gallery_bytes);

    JANUS_TRY_PPR(ppr_unflatten_gallery(ppr_context, flat_data, &gallery->ppr_gallery))
    ppr_free_flat_data(flat_data);

    return JANUS_SUCCESS;
}

janus_error janus_prepare_gallery(janus_gallery &)
{
    return JANUS_SUCCESS;
}

janus_error janus_gallery_insert(janus_gallery &gallery, const janus_template &template_, const uint32_t &id)
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

janus_error janus_gallery_remove(janus_gallery &gallery, const uint32_t &id)
{
    JANUS_TRY_PPR(ppr_remove_subject(ppr_context, &gallery->ppr_gallery, id))
    return JANUS_SUCCESS;
}

janus_error janus_delete_serialized_gallery(uint8_t *&gallery, const size_t)
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

janus_error janus_search(const janus_template &probe, const janus_gallery &gallery, const size_t num_requested_returns, vector<uint32_t> &template_ids, vector<double> &similarities)
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

    vector<pair<float, uint32_t> > scores;

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

janus_error janus_cluster(const vector<janus_media> &input,
                          const vector<uint32_t> &input_ids,
                          const size_t hint,
                          vector<janus_cluster_item> &clusters)
{
    return JANUS_NOT_IMPLEMENTED;
}

janus_error janus_cluster(const vector<janus_template> &templates,
                          const vector<uint32_t> &template_ids,
                          const size_t hint,
                          vector<janus_cluster_item> &clusters)
{
    // PP5 arguments/data structures
    int clustering_aggressiveness = PPR_MAX_CLUSTERING_AGGRESSIVENESS;
    ppr_cluster_list_type cluster_list;


    // janus data structures
    janus_gallery cluster_gallery;
    janus_error error = JANUS_SUCCESS;
    vector<uint32_t> t_ids(templates.size());

    // set the clustering aggressiveness
    if (clustering_aggressiveness > hint)
        clustering_aggressiveness = static_cast<int>(hint);

    error = janus_create_gallery(templates, template_ids, cluster_gallery);

    if (error != JANUS_SUCCESS)
        return error;


    JANUS_TRY_PPR(ppr_cluster_gallery(ppr_context,
                                      reinterpret_cast<ppr_gallery_type *>(cluster_gallery),
                                      clustering_aggressiveness,
                                      &cluster_list));
    
    // unpack clusters
    for (int cluster_id = 0; cluster_id < cluster_list.length; cluster_id++) {
        ppr_id_list_type cluster = cluster_list.clusters[cluster_id];

        for (int j = 0; j < cluster.length; j++) {
            janus_cluster_item item;
            int t_id = cluster.ids[j];

            item.confidence = -1.5;
            item.cluster_id = (uint32_t) cluster_id;
            item.source_id = (uint32_t) t_id;

            clusters.push_back(item);
        }
    }

    // free PP5 structures
    ppr_free_cluster_list(cluster_list);

    // free janus structures
    error = janus_delete_gallery(cluster_gallery);
    return error;
}

