#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>

#include "janus_io.h"

using namespace std;

// These functions have no external dependencies

vector<string> split(const string &str, char delim)
{
    vector<string> elems;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

vector<janus_attribute> attributesFromStrings(const vector<string> &strings, int *templateIDIndex, int *fileNameIndex)
{
    vector<janus_attribute> attributes;
    for (size_t i=0; i<strings.size(); i++) {
        const string &str = strings[i];
        if      (str == "Template_ID") *templateIDIndex = i;
        else if (str == "File_Name")   *fileNameIndex = i;
        else if (str == "Frame")       attributes.push_back(JANUS_FRAME);
        else if (str == "Right_Eye_X") attributes.push_back(JANUS_RIGHT_EYE_X);
        else if (str == "Right_Eye_Y") attributes.push_back(JANUS_RIGHT_EYE_Y);
        else if (str == "Left_Eye_X")  attributes.push_back(JANUS_LEFT_EYE_X);
        else if (str == "Left_Eye_Y")  attributes.push_back(JANUS_LEFT_EYE_Y);
        else if (str == "Nose_Base_X") attributes.push_back(JANUS_NOSE_BASE_X);
        else if (str == "Nose_Base_Y") attributes.push_back(JANUS_NOSE_BASE_Y);
        else                           attributes.push_back(JANUS_INVALID_ATTRIBUTE);
    }
    return attributes;
}

vector<double> valuesFromStrings(const vector<string> &strings, size_t templateIDIndex, size_t fileNameIndex)
{
    vector<double> values;
    for (size_t i=0; i<strings.size(); i++) {
        if ((i == templateIDIndex) || (i == fileNameIndex))
            continue;
        const string &str = strings[i];
        if (str.empty()) values.push_back(numeric_limits<float>::quiet_NaN());
        else             values.push_back(atof(str.c_str()));
    }
    return values;
}

janus_error readMetadataFile(janus_metadata_file file_name, vector<string> &fileNames, vector<janus_template_id> &templateIDs, vector<janus_attribute_list> &attributeLists)
{
    // Open file
    ifstream file(file_name);
    if (!file.is_open())
        return JANUS_OPEN_ERROR;

    // Parse header
    string line;
    getline(file, line);
    int templateIDIndex = -1, fileNameIndex = -1;
    vector<janus_attribute> attributes = attributesFromStrings(split(line, ','), &templateIDIndex, &fileNameIndex);
    if (templateIDIndex == -1) return JANUS_MISSING_TEMPLATE_ID;
    if (fileNameIndex == -1) return JANUS_MISSING_FILE_NAME;

    // Parse rows
    while (getline(file, line)) {
        const vector<string> words = split(line, ',');

        // Make sure all files have the same template ID
        fileNames.push_back(words[fileNameIndex]);
        templateIDs.push_back(atoi(words[templateIDIndex].c_str()));
        vector<double> values = valuesFromStrings(words, templateIDIndex, fileNameIndex);
        if (values.size() != attributes.size())
            return JANUS_PARSE_ERROR;
        const size_t n = attributes.size();

        // Construct attribute list, removing missing fields
        janus_attribute_list attributeList;
        attributeList.size = 0;
        attributeList.attributes = new janus_attribute[n];
        attributeList.values = new double[n];
        for (size_t i=0; i<n; i++) {
            if (values[i] != values[i]) /* NaN */ continue;
            attributeList.attributes[attributeList.size] = attributes[i];
            attributeList.values[attributeList.size] = values[i];
            attributeList.size++;
        }

        attributeLists.push_back(attributeList);
    }

    file.close();
    return JANUS_SUCCESS;
}

janus_template createTemplate(const vector<string> &fileNames,
                              const vector<janus_template_id> &templateIDs,
                              const vector<janus_attribute_list> &attributeLists)
{
    for (size_t i=1; i<templateIDs.size(); i++)
        if (templateIDs[i] != templateIDs[0])
            JANUS_TRY(JANUS_TEMPLATE_ID_MISMATCH)

    janus_template template_;
    JANUS_TRY(janus_initialize_template(&template_))

    for (size_t i=0; i<fileNames.size(); i++) {
        janus_image image;
        JANUS_TRY(janus_read_image(fileNames[i].c_str(), &image))
        JANUS_TRY(janus_add_image(image, attributeLists[i], template_));
        janus_free_image(image);
    }

    return template_;
}

janus_error janus_create_template(janus_metadata_file name_file, janus_flat_template flat_template, size_t *bytes)
{
    vector<string> fileNames;
    vector<janus_template_id> templateIDs;
    vector<janus_attribute_list> attributeLists;
    janus_error error = readMetadataFile(name_file, fileNames, templateIDs, attributeLists);
    if (error != JANUS_SUCCESS)
        return error;
    janus_template template_ = createTemplate(fileNames, templateIDs, attributeLists);
    return janus_finalize_template(template_, flat_template, bytes);
}

janus_error janus_create_gallery(janus_metadata_file metadata_file, janus_gallery_file gallery_file)
{
    fprintf(stderr, "Enrolling 0/?");
    vector<string> fileNames;
    vector<janus_template_id> templateIDs;
    vector<janus_attribute_list> attributeLists;
    janus_error error = readMetadataFile(metadata_file, fileNames, templateIDs, attributeLists);
    if (error != JANUS_SUCCESS)
        return error;

    janus_gallery gallery;
    error = janus_initialize_gallery(&gallery);
    if (error != JANUS_SUCCESS)
        return error;

    size_t i = 0;
    while (i < attributeLists.size()) {
        fprintf(stderr, "\rEnrolling %zu/%zu", i, attributeLists.size());
        size_t j = i;
        while ((j < attributeLists.size()) && (templateIDs[j] == templateIDs[i]))
            j++;
        janus_template template_ = createTemplate(vector<string>(fileNames.begin()+i, fileNames.begin()+j),
                                                  vector<janus_template_id>(templateIDs.begin()+i, templateIDs.begin()+j),
                                                  vector<janus_attribute_list>(attributeLists.begin()+i, attributeLists.begin()+j));
        error = janus_enroll(template_, templateIDs[i], gallery);
        if (error != JANUS_SUCCESS)
            return error;
        i = j;
    }
    fprintf(stderr, "\rEnrolling %zu/%zu\n", i, attributeLists.size());

    return janus_finalize_gallery(gallery, gallery_file);
}

janus_error janus_create_simmat(janus_gallery_file gallery,
                                janus_metadata_file probes,
                                const char *simmat_file)
{
    fprintf(stderr, "Searching 0/?");
    vector<string> fileNames;
    vector<janus_template_id> templateIDs;
    vector<janus_attribute_list> attributeLists;
    janus_error error = readMetadataFile(probes, fileNames, templateIDs, attributeLists);
    if (error != JANUS_SUCCESS)
        return error;

    size_t i = 0;
    while (i < attributeLists.size()) {
        fprintf(stderr, "\rSearching %zu/%zu", i, attributeLists.size());
        size_t j = i;
        while ((j < attributeLists.size()) && (templateIDs[j] == templateIDs[i]))
            j++;
        janus_template template_ = createTemplate(vector<string>(fileNames.begin()+i, fileNames.begin()+j),
                                                  vector<janus_template_id>(templateIDs.begin()+i, templateIDs.begin()+j),
                                                  vector<janus_attribute_list>(attributeLists.begin()+i, attributeLists.begin()+j));
        (void) template_;
//        error = janus_search(template_, gallery_file, )
        if (error != JANUS_SUCCESS)
            return error;
        i = j;
    }
    fprintf(stderr, "\rSearching %zu/%zu\n", i, attributeLists.size());

    (void) gallery;
    (void) simmat_file;
    return JANUS_SUCCESS;
}
