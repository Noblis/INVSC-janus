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

vector<janus_value> valuesFromStrings(const vector<string> &strings, size_t templateIDIndex, size_t fileNameIndex)
{
    vector<janus_value> values;
    for (size_t i=0; i<strings.size(); i++) {
        if ((i == templateIDIndex) || (i == fileNameIndex))
            continue;
        const string &str = strings[i];
        if (str.empty()) values.push_back(-numeric_limits<janus_value>::max());
        else             values.push_back(atof(str.c_str()));
    }
    return values;
}

janus_error janus_enroll_template(const char *file_name, janus_template *template_, janus_size *bytes)
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
    int templateID;
    vector<string> fileNames;
    vector<janus_attribute_list> attributeLists;
    while (getline(file, line)) {
        const vector<string> words = split(line, ',');

        // Make sure all files have the same template ID
        if (fileNames.empty()) {
            templateID = atoi(words[templateIDIndex].c_str());
        } else {
            if (atoi(words[templateIDIndex].c_str()) != templateID)
                return JANUS_TEMPLATE_ID_MISMATCH;
        }

        fileNames.push_back(words[fileNameIndex]);
        vector<janus_value> values = valuesFromStrings(words, templateIDIndex, fileNameIndex);
        if (values.size() != attributes.size())
            return JANUS_PARSE_ERROR;
        const size_t n = attributes.size();

        // Construct attribute list, removing missing fields
        janus_attribute_list attributeList;
        attributeList.size = 0;
        attributeList.attributes = new janus_attribute[n];
        attributeList.values = new janus_value[n];
        for (size_t i=0; i<n; i++) {
            if (values[n] == -numeric_limits<janus_value>::max())
                continue;
            attributeList.attributes[attributeList.size] = attributes[n];
            attributeList.values[attributeList.size] = attributes[n];
            attributeList.size++;
        }

        attributeLists.push_back(attributeList);
    }

    janus_partial_template partial_template;
    JANUS_TRY(janus_initialize_template(&partial_template))

    for (size_t i=0; i<fileNames.size(); i++) {
        janus_image image;
        JANUS_TRY(janus_read_image(fileNames[i].c_str(), &image))
        JANUS_TRY(janus_add_image(image, attributeLists[i], partial_template));
        janus_free_image(image);
    }

    JANUS_TRY(janus_finalize_template(partial_template, *template_, bytes));
    return JANUS_SUCCESS;
}
