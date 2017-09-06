// These file is designed to have no dependencies outside the C++ Standard Library
#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <stack>
#include <chrono>
#include <iomanip>
#include <assert.h>

#include <iarpa_janus_io.h>

using namespace std;

#define ENUM_CASE(X) case JANUS_##X: return #X;
#define ENUM_COMPARE(X,Y) if (!strcmp(#X, Y)) return JANUS_##X;

// For computing timing metrics
// Adapted from: http://stackoverflow.com/questions/13485266/how-to-have-matlab-tic-toc-in-c
using namespace std::chrono;
static stack<steady_clock::time_point> tictoc_stack;
#define JANUS_HARNESS_TIC    tictoc_stack.push(steady_clock::now());
#define JANUS_HARNESS_TOC(x) metrics.x.samples.push_back(static_cast<double>(duration_cast<nanoseconds>(steady_clock::now() - tictoc_stack.top()).count()) / 1.0e6); tictoc_stack.pop()

/*!
 * \brief A statistic.
 * \see janus_metrics
 */
struct janus_metric
{
    vector<double> samples;  /*!< \brief Samples. */

    /*!
     * \brief Sample Average
     */
    inline double mean() {
        double accumulator = 0.0;
        if (samples.size() == 0) return numeric_limits<double>::quiet_NaN();

        for (auto &s : samples) accumulator += s;
        return accumulator / ((double) samples.size());
    }

    /*!
     * \brief Sample Standard Deviation
     */
    inline double stddev() {
        double stddev = 0.0;
        double mean = this->mean();
        if (samples.size() == 0) return numeric_limits<double>::quiet_NaN();

        for (auto &s : samples)
            stddev += pow((double) s - mean, 2.0);
        stddev = sqrt(stddev / ((double) samples.size()));
        return stddev;
    }
};

/*!
 * \brief All statistics.
 */
struct janus_metrics
{
    struct janus_metric janus_initialize; /*!< \brief ms */
    struct janus_metric janus_load_media; /*!< \brief ms */
    struct janus_metric janus_free_media; /*!< \brief ms */
    struct janus_metric janus_detect; /*!< \brief ms */
    struct janus_metric janus_create_template; /*!< \brief ms */
    struct janus_metric janus_serialize_template; /*!< \brief ms */
    struct janus_metric janus_deserialize_template; /*!< \brief ms */
    struct janus_metric janus_delete_serialized_template; /*!< \brief ms */
    struct janus_metric janus_delete_template; /*!< \brief ms */
    struct janus_metric janus_verify; /*!< \brief ms */
    struct janus_metric janus_create_gallery; /*!< \brief ms */
    struct janus_metric janus_prepare_gallery; /*!< \brief ms */
    struct janus_metric janus_gallery_insert; /*!< \brief ms */
    struct janus_metric janus_gallery_remove; /*!< \brief ms */
    struct janus_metric janus_serialize_gallery; /*!< \brief ms */
    struct janus_metric janus_deserialize_gallery; /*!< \brief ms */
    struct janus_metric janus_delete_serialized_gallery; /*!< \brief ms */
    struct janus_metric janus_delete_gallery; /*!< \brief ms */
    struct janus_metric janus_search; /*!< \brief ms */
    struct janus_metric janus_cluster; /*!< \brief ms */
    struct janus_metric janus_finalize; /*!< \brief ms */

    struct janus_metric janus_gallery; /*!< \brief KB */
    struct janus_metric janus_template; /*!< \brief KB */
    int                 janus_missing_attributes_count; /*!< \brief Count of
                                                             \ref JANUS_MISSING_ATTRIBUTES */
    int                 janus_failure_to_detect_count; /*!< \brief Count of
                                                            \ref JANUS_FAILURE_TO_DETECT */
    int                 janus_failure_to_enroll_count; /*!< \brief Count of
                                                            \ref JANUS_FAILURE_TO_ENROLL */
    int                 janus_other_errors_count; /*!< \brief Count of \ref janus_error excluding
                                                       \ref JANUS_MISSING_ATTRIBUTES,
                                                       \ref JANUS_FAILURE_TO_ENROLL, and
                                                       \ref JANUS_SUCCESS */

    #define PRINT_METRIC_ROW(x, unit) if (x.samples.size() > 0 ) { \
                                          cerr <<                              setw(35) << left << setprecision(4) \
                                               << string(#x)                << setw(12) << left << setprecision(4) \
                                               << x.mean()                  << setw(12) << left << setprecision(4) \
                                               << x.stddev()                << setw(12) << left                    \
                                               << unit                      << setw(12) << left                    \
                                               << (double) x.samples.size() << endl;                               \
                                      }
    /*!
     * \brief Print metrics table
     */
    inline void print_metrics() {
        cerr <<                 setw(35) << left
             << "API Symbol" << setw(12) << left
             << "Mean"       << setw(12) << left
             << "StDev"      << setw(12) << left
             << "Units"      << setw(12) << left
             << "Count"      << endl;

        PRINT_METRIC_ROW(janus_initialize,                 "ms");
        PRINT_METRIC_ROW(janus_load_media,                 "ms");
        PRINT_METRIC_ROW(janus_free_media,                 "ms");
        PRINT_METRIC_ROW(janus_detect,                     "ms");
        PRINT_METRIC_ROW(janus_create_template,            "ms");
        PRINT_METRIC_ROW(janus_serialize_template,         "ms");
        PRINT_METRIC_ROW(janus_deserialize_template,       "ms");
        PRINT_METRIC_ROW(janus_delete_serialized_template, "ms");
        PRINT_METRIC_ROW(janus_delete_template,            "ms");
        PRINT_METRIC_ROW(janus_verify,                     "ms");
        PRINT_METRIC_ROW(janus_create_gallery,             "ms");
        PRINT_METRIC_ROW(janus_prepare_gallery,            "ms");
        PRINT_METRIC_ROW(janus_gallery_insert,             "ms");
        PRINT_METRIC_ROW(janus_gallery_remove,             "ms");
        PRINT_METRIC_ROW(janus_serialize_gallery,          "ms");
        PRINT_METRIC_ROW(janus_deserialize_gallery,        "ms");
        PRINT_METRIC_ROW(janus_delete_serialized_gallery,  "ms");
        PRINT_METRIC_ROW(janus_delete_gallery,             "ms");
        PRINT_METRIC_ROW(janus_search,                     "ms");
        PRINT_METRIC_ROW(janus_cluster,                    "ms");
        PRINT_METRIC_ROW(janus_finalize,                   "ms");
        PRINT_METRIC_ROW(janus_gallery,                    "KB");
        PRINT_METRIC_ROW(janus_template,                   "KB");

        cerr << endl << endl
             << "janus_error                     \tCount"                              << endl
             << "JANUS_MISSING_ATTRIBUTES        \t" << janus_missing_attributes_count << endl
             << "JANUS_FAILURE_TO_ENROLL         \t" << janus_failure_to_enroll_count  << endl
             << "All other errors                \t" << janus_other_errors_count       << endl;
    }
    #undef PRINT_METRIC_ROW
};

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
        ENUM_CASE(INVALID_MEDIA)
        ENUM_CASE(MISSING_TEMPLATE_ID)
        ENUM_CASE(MISSING_FILE_NAME)
        ENUM_CASE(NULL_ATTRIBUTES)
        ENUM_CASE(MISSING_ATTRIBUTES)
        ENUM_CASE(FAILURE_TO_DETECT)
        ENUM_CASE(FAILURE_TO_ENROLL)
        ENUM_CASE(FAILURE_TO_SERIALIZE)
        ENUM_CASE(FAILURE_TO_DESERIALIZE)
        ENUM_CASE(NUM_ERRORS)
        ENUM_CASE(NOT_IMPLEMENTED)
    }
    return "UNKNOWN_ERROR";
}

janus_error janus_error_from_string(const char *error)
{
    ENUM_COMPARE(SUCCESS, error)
    ENUM_COMPARE(UNKNOWN_ERROR, error)
    ENUM_COMPARE(OUT_OF_MEMORY, error)
    ENUM_COMPARE(INVALID_SDK_PATH, error)
    ENUM_COMPARE(OPEN_ERROR, error)
    ENUM_COMPARE(READ_ERROR, error)
    ENUM_COMPARE(WRITE_ERROR, error)
    ENUM_COMPARE(PARSE_ERROR, error)
    ENUM_COMPARE(INVALID_MEDIA, error)
    ENUM_COMPARE(MISSING_TEMPLATE_ID, error)
    ENUM_COMPARE(MISSING_FILE_NAME, error)
    ENUM_COMPARE(NULL_ATTRIBUTES, error)
    ENUM_COMPARE(MISSING_ATTRIBUTES, error)
    ENUM_COMPARE(FAILURE_TO_DETECT, error)
    ENUM_COMPARE(FAILURE_TO_ENROLL, error)
    ENUM_COMPARE(FAILURE_TO_SERIALIZE, error)
    ENUM_COMPARE(FAILURE_TO_DESERIALIZE, error)
    ENUM_COMPARE(NUM_ERRORS, error)
    ENUM_COMPARE(NOT_IMPLEMENTED, error)
    return JANUS_UNKNOWN_ERROR;
}

static janus_metrics metrics;

#ifndef JANUS_CUSTOM_INITIALIZE
janus_error janus_initialize_helper(const std::string &sdk_path,
                                    const std::string &temp_path,
                                    const std::string &algorithm,
                                    const int gpu_dev) {
    janus_error error;

    JANUS_HARNESS_TIC;
    error = janus_initialize(sdk_path, temp_path, algorithm, gpu_dev);
    JANUS_HARNESS_TOC(janus_initialize);   

    return error;
}
#endif // JANUS_CUSTOM_INITIALIZE

#ifndef JANUS_CUSTOM_FINALIZE
janus_error janus_finalize_helper() {
    janus_error error;
    JANUS_HARNESS_TIC;
    error = janus_finalize();
    JANUS_HARNESS_TOC(janus_finalize);   

    return error;
}
#endif // JANUS_CUSTOM_FINALIZE

#ifndef JANUS_CUSTOM_DETECT

janus_error janus_detect_helper(const string &data_path, janus_metadata metadata, const size_t min_face_size, const string &detection_list_file, bool verbose)
{
    ifstream file(metadata);
    ofstream output(detection_list_file);

    // Parse the header
    string line;
    getline(file, line);

    while (getline(file, line)) {
        istringstream attributes(line);
        string filename;
        getline(attributes, filename, ',');

        janus_media media;

        JANUS_HARNESS_TIC;
        JANUS_ASSERT(janus_load_media(data_path + filename, media))
        JANUS_HARNESS_TOC(janus_load_media);

        vector<janus_track> tracks;

        JANUS_HARNESS_TIC;
        janus_error error = janus_detect(media, min_face_size, tracks);
        JANUS_HARNESS_TOC(janus_detect);

        if (error == JANUS_FAILURE_TO_DETECT) {
            metrics.janus_failure_to_detect_count++;
            continue;
        } else if (error != JANUS_SUCCESS) {
            metrics.janus_other_errors_count++;
            continue;
        }

        for (size_t i = 0; i < tracks.size(); i++) {
            const janus_track &track = tracks[i];
            for (size_t j = 0; j < track.track.size(); j++) {
                const janus_attributes &attrs = track.track[j];
                output << filename << ","
                       << attrs.face_x << ","
                       << attrs.face_y << ","
                       << attrs.face_width << ","
                       << attrs.face_height << ","
                       << track.detection_confidence << "\n";
            }
        }
        janus_free_media(media);
    }

    file.close();
    output.close();

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_DETECT

struct TemplateMetadata
{
    janus_template_id templateID; // Template id for a template
    int subjectID;                // Subject id for a template
    map<uint32_t,
        pair<vector<string>, janus_track>
       > metadata;                // A collection of media files and associated metadata
                                  // organized by sightingID.

    TemplateMetadata() : templateID(numeric_limits<janus_template_id>::max()), subjectID(-1) {}

    void merge(const TemplateMetadata &other)
    {
        // TODO: we should ensure these are the same
        templateID = other.templateID;
        subjectID = other.subjectID;

        for (const auto &entry : other.metadata) {
            uint32_t sightingID = entry.first;
            pair<vector<string>, janus_track> data = entry.second;

            if (metadata.find(sightingID) != metadata.end()) {
                vector<string> &filenames = metadata[sightingID].first;
                janus_track &track = metadata[sightingID].second;

                filenames.insert(filenames.end(), data.first.begin(), data.first.end());

                track.age = data.second.age;
                track.gender = data.second.gender;
                track.skin_tone = data.second.skin_tone;
                track.track.insert(track.track.end(), data.second.track.begin(), data.second.track.end());
            } else {
                metadata.insert(entry);
            }
        }
    }

    void clear()
    {
        templateID = numeric_limits<size_t>::max();
        subjectID = -1;
        metadata.clear();
    }
};

struct TemplateIterator
{
    vector<TemplateMetadata> templates;

    size_t i;
    bool verbose;

    TemplateIterator(janus_metadata metadata, bool verbose)
        : i(0), verbose(verbose)
    {
        ifstream file(metadata);

        // Parse header
        string line;
        getline(file, line);
        istringstream attributeNames(line);
        string attributeName;
        vector<string> header;
        while (getline(attributeNames, attributeName, ','))
            header.push_back(attributeName);

        // Parse rows
        while (getline(file, line)) {
            TemplateMetadata tmpl;

            istringstream attributeValues(line);
            string filename, vfilename, sightingID, attributeValue;

            // Construct a track from the metadata
            janus_track track;
            janus_attributes attributes;
            for (int j = 0; getline(attributeValues, attributeValue, ','); j++) {
                double value = attributeValue.empty() ? NAN : atof(attributeValue.c_str());
                if (header[j] == "TEMPLATE_ID")
                    tmpl.templateID = atoi(attributeValue.c_str());
                else if (header[j] == "SUBJECT_ID")
                    tmpl.subjectID = atoi(attributeValue.c_str());
                else if (header[j] == "FILENAME")
                    filename = attributeValue;
                else if (header[j] == "SIGHTING_ID")
                    sightingID = attributeValue;
                else if (header[j] == "VIDEO_FILENAME")
                    vfilename = attributeValue;
                else if (header[j] == "FACE_X")
                    attributes.face_x = value;
                else if (header[j] == "FACE_Y")
                    attributes.face_y = value;
                else if (header[j] == "FACE_WIDTH")
                    attributes.face_width = value;
                else if (header[j] == "FACE_HEIGHT")
                    attributes.face_height = value;
                else if (header[j] == "RIGHT_EYE_X")
                    attributes.right_eye_x = value;
                else if (header[j] == "RIGHT_EYE_Y")
                    attributes.right_eye_y = value;
                else if (header[j] == "LEFT_EYE_X")
                    attributes.left_eye_x = value;
                else if (header[j] == "LEFT_EYE_Y")
                    attributes.left_eye_y = value;
                else if (header[j] == "NOSE_BASE_X")
                    attributes.nose_base_x = value;
                else if (header[j] == "NOSE_BASE_Y")
                    attributes.nose_base_y = value;
                else if (header[j] == "FACE_YAW")
                    attributes.face_yaw = value;
                else if (header[j] == "FOREHEAD_VISIBLE")
                    attributes.forehead_visible = value;
                else if (header[j] == "EYES_VISIBLE")
                    attributes.eyes_visible = value;
                else if (header[j] == "NOSE_MOUTH_VISIBLE")
                    attributes.nose_mouth_visible = value;
                else if (header[j] == "INDOOR")
                    attributes.indoor = value;
                else if (header[j] == "GENDER")
                    track.gender = value;
                else if (header[j] == "AGE")
                    track.age = value;
                else if (header[j] == "SKIN_TONE")
                    track.skin_tone = value;
                else if (header[j] == "FRAME_NUM")
                    attributes.frame_number = value;
            }
            track.track.push_back(attributes);

            // prefer video filename
            if (!vfilename.empty()) 
                filename = vfilename;
            tmpl.metadata.insert(make_pair(atoi(sightingID.c_str()), make_pair(vector<string>{filename}, track)));

            if (!templates.empty() && templates.back().templateID == tmpl.templateID) {
                templates.back().merge(tmpl);
            } else {
                templates.push_back(tmpl);
            }
        }

        if (verbose)
            fprintf(stderr, "\rEnrolling %zu/%zu", i, templates.size());
    }

    TemplateMetadata next()
    {
        TemplateMetadata templateMetadata;
        if (i >= templates.size()) {
            fprintf(stderr, "\n");
        } else {
            templateMetadata = templates[i++];
            if (verbose)
                fprintf(stderr, "\rEnrolling %zu/%zu", i, templates.size());
        }
        return templateMetadata;
    }

    static janus_error create(const string &data_path, const TemplateMetadata templateMetadata, const janus_template_role role, janus_template *template_)
    {
        // A set to hold all of the media and metadata required to make a full template
        vector<janus_association> associations;

        // Create a set of all the media used for this template
        for (const auto &entry : templateMetadata.metadata) {
            pair<vector<string>, janus_track> metadata = entry.second;

            janus_media media;
            for (size_t i = 0; i < metadata.first.size(); i++) {
                if (i == 0) {
                    JANUS_HARNESS_TIC;
                    JANUS_ASSERT(janus_load_media(data_path + metadata.first[i], media))
                    JANUS_HARNESS_TOC(janus_load_media);
                } else {
                    janus_media temp;

                    JANUS_HARNESS_TIC;
                    JANUS_ASSERT(janus_load_media(data_path + metadata.first[i], temp))
                    JANUS_HARNESS_TOC(janus_load_media);

                    media.data.push_back(temp.data.front());
                }
            }

            janus_association association;
            association.media = media;
            association.metadata = metadata.second;
            associations.push_back(association);
        }

        // Create the template
        JANUS_HARNESS_TIC;
        janus_error error = janus_create_template(associations, role, *template_);
        JANUS_HARNESS_TOC(janus_create_template);

        // Check the result for errors
        if (error == JANUS_MISSING_ATTRIBUTES)     metrics.janus_missing_attributes_count++;
        else if (error == JANUS_FAILURE_TO_ENROLL) metrics.janus_failure_to_enroll_count++;
        else if (error != JANUS_SUCCESS)           metrics.janus_other_errors_count++;

        // Free the media
        for (size_t i = 0; i < associations.size(); i++) {
            JANUS_HARNESS_TIC;
            JANUS_ASSERT(janus_free_media(associations[i].media));
            JANUS_HARNESS_TOC(janus_free_media);
        }

        return JANUS_SUCCESS;
    }
};

#ifndef JANUS_CUSTOM_CREATE_TEMPLATES

janus_error janus_create_templates_helper(const string &data_path, janus_metadata metadata, const string &templates_path, const string &templates_list_file, const janus_template_role role, bool verbose)
{
    // Create an iterator to loop through the metadata
    TemplateIterator ti(metadata, true);

    // Preallocate the template
    janus_template template_;

    // Set up file I/O
    ofstream templates_list_stream(templates_list_file.c_str(), ios::out | ios::ate);

    TemplateMetadata templateMetadata = ti.next();
    while (templateMetadata.subjectID != -1) {
        JANUS_CHECK(TemplateIterator::create(data_path, templateMetadata, role, &template_))

        // Useful strings
        char templateIDBuffer[10], subjectIDBuffer[10];
        sprintf(templateIDBuffer, "%zu", templateMetadata.templateID);
        sprintf(subjectIDBuffer, "%d", templateMetadata.subjectID);

        const string templateIDString(templateIDBuffer);
        const string subjectIDString(subjectIDBuffer);
        const string templateOutputFile = templates_path + templateIDString + ".template";

        // Serialize the template to a file
        ofstream template_stream(templateOutputFile.c_str(), ios::out | ios::binary);
        
        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_serialize_template(template_, template_stream));
        JANUS_HARNESS_TOC(janus_serialize_template);
        
        template_stream.close();

        // Write the template metadata to the list
        templates_list_stream << templateIDString << "," << subjectIDString << "," << templateOutputFile << "\n";

        // Delete the template
        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_delete_template(template_));
        JANUS_HARNESS_TOC(janus_delete_template);

        // Move to the next template
        templateMetadata = ti.next();
    }
    templates_list_stream.close();

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_CREATE_TEMPLATES

static janus_error janus_load_templates_from_file(const string &templates_list_file, vector<janus_template> &templates, vector<janus_template_id> &template_ids, vector<int> &subject_ids)
{
    ifstream templates_list_stream(templates_list_file.c_str());
    string line;

    while (getline(templates_list_stream, line)) {
        istringstream row(line);
        string template_id, subject_id, template_file;
        getline(row, template_id, ',');
        getline(row, subject_id, ',');
        getline(row, template_file, ',');

        template_ids.push_back(atoi(template_id.c_str()));
        subject_ids.push_back(atoi(subject_id.c_str()));

        // Load the serialized template from disk
        ifstream template_stream(template_file.c_str(), ios::in | ios::binary);
        janus_template template_ = NULL;
        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_deserialize_template(template_, template_stream));
        JANUS_HARNESS_TOC(janus_deserialize_template);
        template_stream.close();

        templates.push_back(template_);
    }
    templates_list_stream.close();

    return JANUS_SUCCESS;
}

#ifndef JANUS_CUSTOM_CREATE_GALLERY

janus_error janus_create_gallery_helper(const string &templates_list_file, const string &gallery_file, bool verbose)
{
    vector<janus_template> templates;
    vector<janus_template_id> template_ids;
    vector<int> subject_ids;
    JANUS_CHECK(janus_load_templates_from_file(templates_list_file, templates, template_ids, subject_ids));

    // Create the gallery
    janus_gallery gallery = NULL;
    JANUS_HARNESS_TIC;
    JANUS_CHECK(janus_create_gallery(templates, template_ids, gallery));
    JANUS_HARNESS_TOC(janus_create_gallery);

    // Prepare the gallery for searching
    JANUS_HARNESS_TIC;
    janus_prepare_gallery(gallery);
    JANUS_HARNESS_TOC(janus_prepare_gallery);

    // Serialize the gallery to a file.
    ofstream gallery_stream(gallery_file.c_str(), ios::out | ios::binary);
    JANUS_HARNESS_TIC;
    JANUS_CHECK(janus_serialize_gallery(gallery, gallery_stream));
    JANUS_HARNESS_TOC(janus_serialize_gallery);
    gallery_stream.close();

    // Delete the actual gallery
    JANUS_HARNESS_TIC;
    JANUS_CHECK(janus_delete_gallery(gallery));
    JANUS_HARNESS_TOC(janus_delete_gallery);

    for (size_t i = 0; i < templates.size() ; i++) {
        JANUS_HARNESS_TIC;
        janus_delete_template(templates[i]);
        JANUS_HARNESS_TOC(janus_delete_template);
    }

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_CREATE_GALLERY

#ifndef JANUS_CUSTOM_VERIFY

janus_error janus_verify_helper(const string &templates_list_file_a, const string &templates_list_file_b, const string &scores_file, bool verbose)
{
    // Load the template sets
    vector<janus_template> templates_a, templates_b;
    vector<janus_template_id> template_ids_a, template_ids_b;
    vector<int> subject_ids_a, subject_ids_b;

    JANUS_CHECK(janus_load_templates_from_file(templates_list_file_a, templates_a, template_ids_a, subject_ids_a));
    JANUS_CHECK(janus_load_templates_from_file(templates_list_file_b, templates_b, template_ids_b, subject_ids_b));

    assert(templates_a.size() == templates_b.size());

    // Compare the templates and write the results to the scores file
    ofstream scores_stream(scores_file.c_str(), ios::out | ios::ate);
    for (size_t i = 0; i < templates_a.size(); i++) {
        double similarity;
        JANUS_HARNESS_TIC;
        janus_verify(templates_a[i], templates_b[i], similarity);
        JANUS_HARNESS_TOC(janus_verify);

        scores_stream << template_ids_a[i] << "," << template_ids_b[i] << "," << similarity << ","
                      << (subject_ids_a[i] == subject_ids_b[i] ? "true" : "false") << "\n";
    }
    scores_stream.close();

    for (size_t i = 0; i < templates_a.size(); i++){
        JANUS_HARNESS_TIC;
        janus_delete_template(templates_a[i]);
        JANUS_HARNESS_TOC(janus_delete_template);
    }

    for (size_t i = 0; i < templates_b.size(); i++) {
        JANUS_HARNESS_TIC;
        janus_delete_template(templates_b[i]);
        JANUS_HARNESS_TOC(janus_delete_template);
    }

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_VERIFY

#ifndef JANUS_CUSTOM_SEARCH

janus_error janus_ensure_size(const vector<janus_template_id> &all_ids, vector<janus_template_id> &return_ids, vector<double> &similarities)
{
    set<janus_template_id> return_lookup(return_ids.begin(), return_ids.end());

    return_ids.reserve(all_ids.size()); similarities.reserve(all_ids.size());
    for (size_t i = 0; i < all_ids.size(); i++) {
        janus_template_id id = all_ids[i];
        if (return_lookup.find(id) == return_lookup.end()) {
            return_ids.push_back(id);
            similarities.push_back(0.0);
        }
    }

    return JANUS_SUCCESS;
}

janus_error janus_search_helper(const string &probes_list_file, const string &gallery_list_file, const string &gallery_file, int num_requested_returns, const string &candidate_list_file, bool verbose)
{
    // Vectors to hold loaded data
    vector<janus_template> probe_templates, gallery_templates;
    vector<janus_template_id> probe_template_ids, gallery_template_ids;
    vector<int> probe_subject_ids, gallery_subject_ids;

    JANUS_CHECK(janus_load_templates_from_file(probes_list_file, probe_templates, probe_template_ids, probe_subject_ids));
    JANUS_CHECK(janus_load_templates_from_file(gallery_list_file, gallery_templates, gallery_template_ids, gallery_subject_ids))

    // Build template_id -> subject_id LUT for the gallery
    map<janus_template_id, int> subjectIDLUT;
    for (size_t i = 0; i < gallery_template_ids.size(); i++) {
        subjectIDLUT.insert(make_pair(gallery_template_ids[i], gallery_subject_ids[i]));

        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_delete_template(gallery_templates[i]))
        JANUS_HARNESS_TOC(janus_delete_template);
    }

    // Load the serialized gallery from disk
    ifstream gallery_stream(gallery_file.c_str(), ios::in | ios::binary);
    janus_gallery gallery = NULL;
    JANUS_HARNESS_TIC;
    JANUS_CHECK(janus_deserialize_gallery(gallery, gallery_stream));
    JANUS_HARNESS_TOC(janus_deserialize_gallery);

    ofstream candidate_stream(candidate_list_file.c_str(), ios::out | ios::ate);
    for (size_t i = 0; i < probe_templates.size(); i++) {
        vector<janus_template_id> return_template_ids;
        vector<double> similarities;
        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_search(probe_templates[i], gallery, num_requested_returns, return_template_ids, similarities));
        JANUS_HARNESS_TOC(janus_search);

        janus_ensure_size(gallery_template_ids, return_template_ids, similarities);

        for (size_t j = 0; j < num_requested_returns; j++)
            candidate_stream << probe_template_ids[i] << "," << j << "," << return_template_ids[j] << "," << similarities[j]
                             << "," << (probe_subject_ids[i] == subjectIDLUT[return_template_ids[j]] ? "true" : "false") << "\n";

        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_delete_template(probe_templates[i]))
        JANUS_HARNESS_TOC(janus_delete_template);
    }
    JANUS_HARNESS_TIC;
    JANUS_CHECK(janus_delete_gallery(gallery));
    JANUS_HARNESS_TOC(janus_delete_gallery);
    candidate_stream.close();

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_SEARCH

#ifndef JANUS_CUSTOM_CLUSTER
janus_error janus_cluster_helper(const string &templates_list_file, const size_t hint, const string &clusters_output_list, bool verbose)
{
    vector<janus_template> cluster_templates;
    vector<janus_template_id> cluster_template_ids;
    vector<int> cluster_subject_ids;

    vector<cluster_pair> cluster_pairs;

    // load templates
    JANUS_CHECK(janus_load_templates_from_file(templates_list_file,
                                               cluster_templates,
                                               cluster_template_ids,
                                               cluster_subject_ids));

    // perform clustering
    JANUS_HARNESS_TIC;
    JANUS_CHECK(janus_cluster(cluster_templates, hint, cluster_pairs));
    JANUS_HARNESS_TOC(janus_cluster);

    // write results and delete templates
    ofstream cluster_stream(clusters_output_list.c_str(), ios::out | ios::ate);
    for (size_t i = 0; i < cluster_templates.size(); i++) {
        // write a row
        cluster_stream << cluster_template_ids[i] << ","
                       << cluster_subject_ids[i]  << ","
                       << cluster_pairs[i].first  << ","
                       << cluster_pairs[i].second << std::endl;

        // delete templates
        JANUS_HARNESS_TIC;
        JANUS_CHECK(janus_delete_template(cluster_templates[i]))
        JANUS_HARNESS_TOC(janus_delete_template);
    }

    cluster_stream.close();

    return JANUS_SUCCESS;
}
#endif // JANUS_CUSTOM_SEARCH

void janus_print_metrics()
{
    metrics.print_metrics();
}
