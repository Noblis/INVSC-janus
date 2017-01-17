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
#include <assert.h>

#include <janus_harness.h>

using namespace std;

// For computing metrics
static vector<double> janus_load_media_samples;
static vector<double> janus_delete_media_samples;
static vector<double> janus_detection_samples;
static vector<double> janus_create_template_samples;
static vector<double> janus_template_size_samples;
static vector<double> janus_serialize_template_samples;
static vector<double> janus_deserialize_template_samples;
static vector<double> janus_delete_serialized_template_samples;
static vector<double> janus_delete_template_samples;
static vector<double> janus_verify_samples;
static vector<double> janus_create_gallery_samples;
static vector<double> janus_prepare_gallery_samples;
static vector<double> janus_gallery_size_samples;
static vector<double> janus_gallery_insert_samples;
static vector<double> janus_gallery_remove_samples;
static vector<double> janus_serialize_gallery_samples;
static vector<double> janus_deserialize_gallery_samples;
static vector<double> janus_delete_serialized_gallery_samples;
static vector<double> janus_delete_gallery_samples;
static vector<double> janus_search_samples;
static vector<double> janus_cluster_samples;
static int janus_missing_attributes_count = 0;
static int janus_failure_to_detect_count = 0;
static int janus_failure_to_enroll_count = 0;
static int janus_other_errors_count = 0;

static void _janus_add_sample(vector<double> &samples, double sample);

#ifndef JANUS_CUSTOM_ADD_SAMPLE

static void _janus_add_sample(vector<double> &samples, double sample)
{
    samples.push_back(sample);
}

#endif // JANUS_CUSTOM_ADD_SAMPLE

#ifndef JANUS_CUSTOM_DETECT

janus_error janus_harness_detect(const string &data_path, janus_metadata metadata, const size_t min_face_size, const string &detection_list_file, bool verbose)
{
    clock_t start;

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

        start = clock();
        JANUS_ASSERT(janus_load_media(data_path + filename, media))
        _janus_add_sample(janus_load_media_samples, 1000.0 * (clock() - start) / CLOCKS_PER_SEC);

        vector<janus_track> tracks;

        start = clock();
        janus_error error = janus_detect(media, min_face_size, tracks);
        _janus_add_sample(janus_detection_samples, 1000.0 * (clock() - start) / CLOCKS_PER_SEC);

        if (error == JANUS_FAILURE_TO_DETECT) {
            janus_failure_to_detect_count++;
            continue;
        } else if (error != JANUS_SUCCESS) {
            janus_other_errors_count++;
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
        janus_delete_media(media);
    }

    file.close();
    output.close();

    if (verbose)
        janus_print_metrics(janus_get_metrics());

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_DETECT

struct TemplateMetadata
{
    uint32_t templateID;                        // Template id for a template
    int subjectID;                              // Subject id for a template
    vector<pair<string, janus_track>> metadata; // A collection of filenames and metadata
                                                // for a template

    TemplateMetadata() : templateID(numeric_limits<uint32_t>::max()), subjectID(-1) {}

    void merge(const TemplateMetadata &other)
    {
        // TODO: we should ensure these are the same
        templateID = other.templateID;
        subjectID = other.subjectID;

        // merge
        metadata.push_back(other.metadata.back());
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

            tmpl.metadata.push_back(make_pair(filename, track));

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
        clock_t start;

        // A set to hold all of the media and metadata required to make a full template
        vector<janus_track> associations;

        // Create a set of all the media used for this template
        for (const auto &name_track : templateMetadata.metadata) {
            janus_media temp;
            string filename = name_track.first;
            janus_track association = name_track.second;

            start = clock();
            JANUS_ASSERT(janus_load_media(data_path + filename, temp))
            _janus_add_sample(janus_load_media_samples, 1000.0 * (clock() - start) / CLOCKS_PER_SEC);
            association.media = temp;
            associations.push_back(association);
        }

        // Create the template
        start = clock();
        janus_error error = janus_create_template(associations, role, *template_);
        _janus_add_sample(janus_create_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

        // Check the result for errors
        if (error == JANUS_MISSING_ATTRIBUTES)     janus_missing_attributes_count++;
        else if (error == JANUS_FAILURE_TO_ENROLL) janus_failure_to_enroll_count++;
        else if (error != JANUS_SUCCESS)           janus_other_errors_count++;

        // Free the media
        for (size_t i = 0; i < associations.size(); i++) {
            start = clock();
            JANUS_ASSERT(janus_delete_media(associations[i].media));
            _janus_add_sample(janus_delete_media_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
        }

        return JANUS_SUCCESS;
    }
};

#ifndef JANUS_CUSTOM_CREATE_TEMPLATES

janus_error janus_harness_create_templates(const string &data_path, janus_metadata metadata, const string &templates_path, const string &templates_list_file, const janus_template_role role, bool verbose)
{
    clock_t start;

    // Create an iterator to loop through the metadata
    TemplateIterator ti(metadata, true);

    // Preallocate the template
    janus_template template_;

    // Set up file I/O
    ofstream templates_list_stream(templates_list_file.c_str(), ios::out | ios::ate);

    TemplateMetadata templateMetadata = ti.next();
    while (templateMetadata.templateID != -1) {
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
        
        start = clock();
        JANUS_CHECK(janus_serialize_template(template_, template_stream));
        _janus_add_sample(janus_serialize_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
        
        template_stream.close();

        // Write the template metadata to the list
        templates_list_stream << templateIDString << "," << subjectIDString << "," << templateOutputFile;

        if (role == janus_template_role::CLUSTERING) // add filename column if role == clustering
            templates_list_stream << "," << templateMetadata.metadata[0].first; // will always be single image templates

        templates_list_stream << endl;

        // Delete the template
        start = clock();
        JANUS_CHECK(janus_delete_template(template_));
        _janus_add_sample(janus_delete_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

        // Move to the next template
        templateMetadata = ti.next();
    }
    templates_list_stream.close();

    if (verbose)
        janus_print_metrics(janus_get_metrics());

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_CREATE_TEMPLATES

static janus_error janus_load_templates_from_file(const string &templates_list_file, vector<janus_template> &templates, vector<uint32_t> &template_ids, vector<uint32_t> &subject_ids, vector<string> &file_names)
{
    clock_t start;

    ifstream templates_list_stream(templates_list_file.c_str());
    string line;

    while (getline(templates_list_stream, line)) {
        istringstream row(line);
        string template_id, subject_id, template_file, media_file;
        getline(row, template_id, ',');
        getline(row, subject_id, ',');
        getline(row, template_file, ',');
        getline(row, media_file, ',');
        file_names.push_back(media_file);   

        template_ids.push_back(atoi(template_id.c_str()));
        subject_ids.push_back(atoi(subject_id.c_str()));

        // Load the serialized template from disk
        ifstream template_stream(template_file.c_str(), ios::in | ios::binary);
        janus_template template_ = NULL;
        start = clock();
        JANUS_CHECK(janus_deserialize_template(template_, template_stream));
        _janus_add_sample(janus_deserialize_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
        template_stream.close();

        templates.push_back(template_);
    }
    templates_list_stream.close();

    return JANUS_SUCCESS;
}

static janus_error janus_load_templates_from_file(const string &templates_list_file, vector<janus_template> &templates, vector<uint32_t> &template_ids, vector<uint32_t> &subject_ids)
{
    vector<string> tmp_filenames;
    return janus_load_templates_from_file(templates_list_file, templates, template_ids, subject_ids, tmp_filenames);
}

static janus_error janus_load_media_from_file(const string &data_path, const string &media_list_file, vector<janus_media> &media, vector<uint32_t> &template_ids, vector<string> &filenames)
{
    uint32_t id = 0;

    ifstream media_list_stream(media_list_file.c_str());
    string media_file;

    while (getline(media_list_stream, media_file)) {
        janus_media tmp;
        janus_load_media(data_path + media_file, tmp);

        media.push_back(tmp);
        template_ids.push_back(id++);
        filenames.push_back(media_file);
    }
    media_list_stream.close();
    return JANUS_SUCCESS;
}

#ifndef JANUS_CUSTOM_CREATE_GALLERY

janus_error janus_harness_create_gallery(const string &templates_list_file, const string &gallery_file, bool verbose)
{
    clock_t start;

    vector<janus_template> templates;
    vector<uint32_t> template_ids;
    vector<uint32_t> subject_ids;
    JANUS_CHECK(janus_load_templates_from_file(templates_list_file, templates, template_ids, subject_ids));

    // Create the gallery
    janus_gallery gallery = NULL;
    start = clock();
    janus_create_gallery(templates, template_ids, gallery);
    _janus_add_sample(janus_create_gallery_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

    // Prepare the gallery for searching
    start = clock();
    janus_prepare_gallery(gallery);
    _janus_add_sample(janus_prepare_gallery_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

    // Serialize the gallery to a file.
    ofstream gallery_stream(gallery_file.c_str(), ios::out | ios::binary);
    start = clock();
    JANUS_CHECK(janus_serialize_gallery(gallery, gallery_stream));
    _janus_add_sample(janus_serialize_gallery_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
    gallery_stream.close();

    // Delete the actual gallery
    start = clock();
    JANUS_CHECK(janus_delete_gallery(gallery));
    _janus_add_sample(janus_delete_gallery_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

    if (verbose)
        janus_print_metrics(janus_get_metrics());

    for (size_t i = 0; i < templates.size() ; i++)
        janus_delete_template(templates[i]);

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_CREATE_GALLERY

#ifndef JANUS_CUSTOM_VERIFY
janus_error janus_load_template(const string &data_path, uint32_t template_id, janus_template &tmpl)
{
    string template_file = data_path + to_string(template_id) + ".template";
    ifstream template_stream(template_file.c_str(), ios::in | ios::binary);
    if (!template_stream)
        printf("WARNING! Template %s does not exist on disk\n", template_file.c_str());

    clock_t start = clock();
    JANUS_CHECK(janus_deserialize_template(tmpl, template_stream));
    _janus_add_sample(janus_deserialize_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

    template_stream.close();

    return JANUS_SUCCESS;
}

janus_error janus_harness_verify(const string& probe_data_path, const string& reference_data_path, const string& templates_matches_file, const string& scores_file, bool verbose)
{
    clock_t start;
    
    // Finally, compare the templates and write the results to the scores file
    ofstream scores_stream(scores_file.c_str(), ios::out);
    
    ifstream matches_stream(templates_matches_file.c_str());
    string line;

    map<uint32_t, janus_template> probe_template_lut;
    map<uint32_t, janus_template> reference_template_lut;

    while (getline(matches_stream, line)) {
        istringstream row(line);
        string probe_id_str, reference_id_str;
        getline(row, probe_id_str, ',');
        getline(row, reference_id_str, ',');

        uint32_t probe_id = atoi(probe_id_str.c_str());
        uint32_t reference_id = atoi(reference_id_str.c_str());

        auto probe_template_it = probe_template_lut.find(probe_id);
        if (probe_template_it == probe_template_lut.end()) {
            janus_template probe_template = nullptr;
            JANUS_CHECK(janus_load_template(probe_data_path, probe_id, probe_template))
            probe_template_it = probe_template_lut.insert(make_pair(probe_id, probe_template)).first;
        }
        
        auto reference_template_it = reference_template_lut.find(reference_id);
        if (reference_template_it == reference_template_lut.end()) {
            janus_template reference_template = nullptr;
            JANUS_CHECK(janus_load_template(reference_data_path, reference_id, reference_template))
            reference_template_it = reference_template_lut.insert(make_pair(reference_id, reference_template)).first;
        }

        double similarity;
        start = clock();
        if (janus_verify(probe_template_it->second, reference_template_it->second, similarity) != JANUS_SUCCESS)
            similarity = numeric_limits<double>::lowest();
        _janus_add_sample(janus_verify_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
        scores_stream << probe_id << "," << reference_id << "," << similarity << "\n";
    }
    scores_stream.close();

    // free probe templates
    for (auto &temp : probe_template_lut)
        janus_delete_template(temp.second);
    
    // free reference templates
    for (auto &temp : reference_template_lut)
        janus_delete_template(temp.second);

    if (verbose)
        janus_print_metrics(janus_get_metrics());

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_VERIFY

#ifndef JANUS_CUSTOM_SEARCH

janus_error janus_ensure_size(const vector<uint32_t> &all_ids, vector<uint32_t> &return_ids, vector<double> &similarities)
{
    set<uint32_t> return_lookup(return_ids.begin(), return_ids.end());

    return_ids.reserve(all_ids.size()); similarities.reserve(all_ids.size());
    for (size_t i = 0; i < all_ids.size(); i++) {
        uint32_t id = all_ids[i];
        if (return_lookup.find(id) == return_lookup.end()) {
            return_ids.push_back(id);
            similarities.push_back(0.0);
        }
    }

    return JANUS_SUCCESS;
}

janus_error janus_harness_search(const string &probes_list_file, const string &gallery_list_file, const string &gallery_file, int num_requested_returns, const string &candidate_list_file, bool verbose)
{
    clock_t start;

    // Vectors to hold loaded data
    vector<janus_template> probe_templates, gallery_templates;
    vector<uint32_t> probe_template_ids, gallery_template_ids;
    vector<uint32_t> probe_subject_ids, gallery_subject_ids;

    JANUS_CHECK(janus_load_templates_from_file(probes_list_file, probe_templates, probe_template_ids, probe_subject_ids));
    JANUS_CHECK(janus_load_templates_from_file(gallery_list_file, gallery_templates, gallery_template_ids, gallery_subject_ids))

    // Build template_id -> subject_id LUT for the gallery
    map<uint32_t, uint32_t> subjectIDLUT;
    for (size_t i = 0; i < gallery_template_ids.size(); i++) {
        subjectIDLUT.insert(make_pair(gallery_template_ids[i], gallery_subject_ids[i]));

        start = clock();
        JANUS_CHECK(janus_delete_template(gallery_templates[i]))
        _janus_add_sample(janus_delete_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
    }

    // Load the serialized gallery from disk
    ifstream gallery_stream(gallery_file.c_str(), ios::in | ios::binary);
    janus_gallery gallery = NULL;
    start = clock();
    JANUS_CHECK(janus_deserialize_gallery(gallery, gallery_stream));
    _janus_add_sample(janus_deserialize_gallery_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

    ofstream candidate_stream(candidate_list_file.c_str(), ios::out | ios::ate);
    for (size_t i = 0; i < probe_templates.size(); i++) {
        vector<uint32_t> return_template_ids;
        vector<double> similarities;
        start = clock();
        JANUS_CHECK(janus_search(probe_templates[i], gallery, num_requested_returns, return_template_ids, similarities));
        _janus_add_sample(janus_search_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

        janus_ensure_size(gallery_template_ids, return_template_ids, similarities);

        for (size_t j = 0; j < return_template_ids.size(); j++)
            candidate_stream << probe_template_ids[i] << "," << j << "," << return_template_ids[j] << "," << similarities[j]
                             << "," << (probe_subject_ids[i] == subjectIDLUT[return_template_ids[j]] ? "true" : "false") << "\n";

        start = clock();
        JANUS_CHECK(janus_delete_template(probe_templates[i]))
        _janus_add_sample(janus_delete_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
    }
    janus_delete_gallery(gallery);
    candidate_stream.close();

    if (verbose)
        janus_print_metrics(janus_get_metrics());

    return JANUS_SUCCESS;
}

#endif // JANUS_CUSTOM_SEARCH

#ifndef JANUS_CUSTOM_CLUSTER
janus_error janus_harness_cluster(const string &list_file, const bool is_template_list, const string &data_path, const size_t hint, const string &clusters_output_list, bool verbose)
{
    // common variables
    clock_t start;
    vector<uint32_t> cluster_template_ids;
    vector<uint32_t> cluster_subject_ids;
    vector<janus_cluster_item> cluster_items;
    vector<string> filenames;

    // test 7 variables
    vector<janus_template> cluster_templates;
    map<uint32_t, string> filename_map;

    // test 8 variables
    vector<janus_media> cluster_media;

    ofstream cluster_stream(clusters_output_list.c_str(), ios::out | ios::ate);
    if (is_template_list) { // TEST 7
        // write header
        cluster_stream << "TEMPLATE_ID,FILENAME,CLUSTER_INDEX,CONFIDENCE" << endl;

        // load templates
        JANUS_CHECK(janus_load_templates_from_file(list_file,
                                                   cluster_templates,
                                                   cluster_template_ids,
                                                   cluster_subject_ids,
                                                   filenames));

        // create a template id <-> filename map
        transform(cluster_template_ids.begin(), cluster_template_ids.end(),
                  filenames.begin(), inserter(filename_map, filename_map.begin()),
                  [] (uint32_t id, string fn) { return make_pair(id, fn); });

        // perform clustering
        start = clock();
        JANUS_CHECK(janus_cluster(cluster_templates, cluster_template_ids, hint, cluster_items));
        _janus_add_sample(janus_cluster_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

        // write results and delete templates
        for (int i = 0; i < cluster_items.size(); i++) {
            janus_cluster_item &item = cluster_items[i];

            // write a row
            cluster_stream << item.source_id               << ","    // TEMPLATE_ID
                           << filename_map[item.source_id] << ","    // FILENAME
                           << item.cluster_id              << ","    // CLUSTER_ID
                           << item.confidence              << endl;  // CONFIDENCE
        }

        // delete templates
        for (janus_template &tmpl_ : cluster_templates) {
            start = clock();
            JANUS_CHECK(janus_delete_template(tmpl_))
            _janus_add_sample(janus_delete_template_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);
        }
        cluster_templates.clear();
    } else { // TEST 8
        // write header
        cluster_stream << "TEMPLATE_ID,FILENAME,CLUSTER_INDEX,CONFIDENCE,FACE_X,FACE_Y,FACE_WIDTH,FACE_HEIGHT,FRAME_NUMBER" << endl;

        // load media
        JANUS_CHECK(janus_load_media_from_file(data_path, list_file, cluster_media, cluster_template_ids, filenames));

        // create a template id <-> filename map
        transform(cluster_template_ids.begin(), cluster_template_ids.end(),
                  filenames.begin(), inserter(filename_map, filename_map.begin()),
                  [] (uint32_t id, string fn) { return make_pair(id, fn); });

        // perform clustering
        start = clock();
        JANUS_CHECK(janus_cluster(cluster_media, cluster_template_ids, hint, cluster_items));
        _janus_add_sample(janus_cluster_samples, 1000 * (clock() - start) / CLOCKS_PER_SEC);

        // write results and delete templates
        for (auto &item : cluster_items) {
            for (auto &attributes : item.track.track) {
                cluster_stream << item.source_id               << ","     // TEMPLATE_ID
                               << filename_map[item.source_id] << ","     // FILENAME
                               << item.cluster_id              << ","     // CLUSTER_INDEX
                               << item.confidence              << ","     // CONFIDENCE
                               << attributes.face_x            << ","     // FACE_X
                               << attributes.face_y            << ","     // FACE_Y
                               << attributes.face_width        << ","     // FACE_WIDTH
                               << attributes.face_height       << ","     // FACE_HEIGHT
                               << attributes.frame_number      << endl;   // FRAME_NUMBER
            }
        }

        for (size_t i = 0; i < cluster_media.size(); i++)
            janus_delete_media(cluster_media[i]);
        cluster_media.clear();
    }

    cluster_stream.close();

    if (verbose)
        janus_print_metrics(janus_get_metrics());

    return JANUS_SUCCESS;
}
#endif // JANUS_CUSTOM_SEARCH

static janus_metric calculateMetric(const vector<double> &samples)
{
    janus_metric metric;
    metric.count = samples.size();

    if (metric.count > 0) {
        metric.mean = 0;
        for (size_t i = 0; i < samples.size(); i++)
            metric.mean += samples[i];
        metric.mean /= samples.size();

        metric.stddev = 0;
        for (size_t i = 0; i < samples.size(); i++)
            metric.stddev += pow(samples[i] - metric.mean, 2.0);
        metric.stddev = sqrt(metric.stddev / samples.size());
    } else {
        metric.mean = numeric_limits<double>::quiet_NaN();
        metric.stddev = numeric_limits<double>::quiet_NaN();
    }

    return metric;
}

janus_metrics janus_get_metrics()
{
    janus_metrics metrics;
    metrics.janus_load_media_speed                 = calculateMetric(janus_load_media_samples);
    metrics.janus_delete_media_speed               = calculateMetric(janus_delete_media_samples);
    metrics.janus_detection_speed                  = calculateMetric(janus_detection_samples);
    metrics.janus_create_template_speed            = calculateMetric(janus_create_template_samples);
    metrics.janus_template_size                    = calculateMetric(janus_template_size_samples);
    metrics.janus_serialize_template_speed         = calculateMetric(janus_serialize_template_samples);
    metrics.janus_deserialize_template_speed       = calculateMetric(janus_deserialize_template_samples);
    metrics.janus_delete_serialized_template_speed = calculateMetric(janus_delete_serialized_template_samples);
    metrics.janus_delete_template_speed            = calculateMetric(janus_delete_template_samples);
    metrics.janus_verify_speed                     = calculateMetric(janus_verify_samples);
    metrics.janus_create_gallery_speed             = calculateMetric(janus_create_gallery_samples);
    metrics.janus_prepare_gallery_speed            = calculateMetric(janus_prepare_gallery_samples);
    metrics.janus_gallery_size                     = calculateMetric(janus_gallery_size_samples);
    metrics.janus_gallery_insert_speed             = calculateMetric(janus_gallery_insert_samples);
    metrics.janus_gallery_remove_speed             = calculateMetric(janus_gallery_remove_samples);
    metrics.janus_serialize_gallery_speed          = calculateMetric(janus_serialize_gallery_samples);
    metrics.janus_deserialize_gallery_speed        = calculateMetric(janus_deserialize_gallery_samples);
    metrics.janus_delete_serialized_gallery_speed  = calculateMetric(janus_delete_serialized_gallery_samples);
    metrics.janus_delete_gallery_speed             = calculateMetric(janus_delete_gallery_samples);
    metrics.janus_search_speed                     = calculateMetric(janus_search_samples);
    metrics.janus_cluster_speed                    = calculateMetric(janus_cluster_samples);
    metrics.janus_missing_attributes_count         = janus_missing_attributes_count;
    metrics.janus_failure_to_enroll_count          = janus_failure_to_enroll_count;
    metrics.janus_other_errors_count               = janus_other_errors_count;
    return metrics;
}

static void printMetric(FILE *file, const char *name, janus_metric metric, bool speed = true)
{
    if (metric.count > 0)
        fprintf(file, "%s\t%.2g\t%.2g\t%s\t%.2g\n", name, metric.mean, metric.stddev, speed ? "ms" : "KB", double(metric.count));
}

void janus_print_metrics(janus_metrics metrics)
{
    fprintf(stderr,     "API Symbol                      \tMean\tStdDev\tUnits\tCount\n");
    printMetric(stderr, "janus_load_media                ", metrics.janus_load_media_speed);
    printMetric(stderr, "janus_delete_media              ", metrics.janus_delete_media_speed);
    printMetric(stderr, "janus_detection                 ", metrics.janus_detection_speed);
    printMetric(stderr, "janus_create_template           ", metrics.janus_create_template_speed);
    printMetric(stderr, "janus_template_size             ", metrics.janus_template_size, false);
    printMetric(stderr, "janus_serialize_template        ", metrics.janus_serialize_template_speed);
    printMetric(stderr, "janus_deserialize_template      ", metrics.janus_deserialize_template_speed);
    printMetric(stderr, "janus_delete_serialized_template", metrics.janus_delete_serialized_template_speed);
    printMetric(stderr, "janus_delete_template           ", metrics.janus_delete_template_speed);
    printMetric(stderr, "janus_verify                    ", metrics.janus_verify_speed);
    printMetric(stderr, "janus_create_gallery            ", metrics.janus_create_gallery_speed);
    printMetric(stderr, "janus_prepare_gallery           ", metrics.janus_prepare_gallery_speed);
    printMetric(stderr, "janus_gallery_size              ", metrics.janus_gallery_size, false);
    printMetric(stderr, "janus_gallery_insert            ", metrics.janus_gallery_insert_speed);
    printMetric(stderr, "janus_gallery_remove            ", metrics.janus_gallery_remove_speed);
    printMetric(stderr, "janus_serialize_gallery         ", metrics.janus_serialize_gallery_speed);
    printMetric(stderr, "janus_deserialize_gallery       ", metrics.janus_deserialize_gallery_speed);
    printMetric(stderr, "janus_delete_serialized_gallery ", metrics.janus_delete_serialized_gallery_speed);
    printMetric(stderr, "janus_delete_gallery            ", metrics.janus_delete_gallery_speed);
    printMetric(stderr, "janus_search                    ", metrics.janus_search_speed);
    printMetric(stderr, "janus_cluster                   ", metrics.janus_cluster_speed);
    fprintf(stderr,     "\n\n");
    fprintf(stderr,     "janus_error                     \tCount\n");
    fprintf(stderr,     "JANUS_MISSING_ATTRIBUTES        \t%d\n", metrics.janus_missing_attributes_count);
    fprintf(stderr,     "JANUS_FAILURE_TO_ENROLL         \t%d\n", metrics.janus_failure_to_enroll_count);
    fprintf(stderr,     "All other errors                \t%d\n", metrics.janus_other_errors_count);
}
