// These file is designed to have no dependencies outside the C++ Standard Library
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>

#include "janus_io.h"

using namespace std;

static janus_error readMetadataFile(janus_metadata metadata, vector<string> &fileNames, vector<janus_template_id> &templateIDs, vector<janus_attribute_list> &attributeLists)
{
    // Open file
    ifstream file(metadata);
    if (!file.is_open())
        return JANUS_OPEN_ERROR;

    // Parse header
    string line;
    getline(file, line);
    istringstream attributeNames(line);
    string attributeName;
    getline(attributeNames, attributeName, ','); // Template_ID
    getline(attributeNames, attributeName, ','); // File_Name
    vector<janus_attribute> attributes;
    while (getline(attributeNames, attributeName, ',')) {
        if      (attributeName == "Frame")       attributes.push_back(JANUS_FRAME);
        else if (attributeName == "Right_Eye_X") attributes.push_back(JANUS_RIGHT_EYE_X);
        else if (attributeName == "Right_Eye_Y") attributes.push_back(JANUS_RIGHT_EYE_Y);
        else if (attributeName == "Left_Eye_X")  attributes.push_back(JANUS_LEFT_EYE_X);
        else if (attributeName == "Left_Eye_Y")  attributes.push_back(JANUS_LEFT_EYE_Y);
        else if (attributeName == "Nose_Base_X") attributes.push_back(JANUS_NOSE_BASE_X);
        else if (attributeName == "Nose_Base_Y") attributes.push_back(JANUS_NOSE_BASE_Y);
        else                                     attributes.push_back(JANUS_INVALID_ATTRIBUTE);
    }

    // Parse rows
    while (getline(file, line)) {
        istringstream attributeValues(line);
        string templateID, fileName, attributeValue;
        getline(attributeValues, templateID, ',');
        getline(attributeValues, fileName, ',');
        templateIDs.push_back(atoi(templateID.c_str()));
        fileNames.push_back(fileName);

        // Construct attribute list, removing missing fields
        janus_attribute_list attributeList;
        attributeList.size = 0;
        attributeList.attributes = new janus_attribute[attributes.size()];
        attributeList.values = new double[attributes.size()];
        for (int i=2; getline(attributeValues, attributeValue, ','); i++) {
            if (attributeValue.empty())
                continue;
            const double value = atof(attributeValue.c_str());
            attributeList.attributes[attributeList.size] = attributes[i];
            attributeList.values[attributeList.size] = value;
            attributeList.size++;
        }
        attributeLists.push_back(attributeList);
    }

    file.close();
    return JANUS_SUCCESS;
}

class TemplateIterator
{
    vector<string> fileNames;
    vector<janus_template_id> templateIDs;
    vector<janus_attribute_list> attributeLists;
    size_t i;

public:
    janus_error error;

    TemplateIterator(janus_metadata metadata)
    {
        fprintf(stderr, "Enrolling 0/?");
        error = readMetadataFile(metadata, fileNames, templateIDs, attributeLists);
        i = 0;
    }

    janus_error next(janus_template *template_, janus_template_id *templateID)
    {
        if (i >= attributeLists.size()) {
            *template_ = NULL;
            *templateID = -1;
        } else {
            *templateID = templateIDs[i];
            JANUS_CHECK(janus_initialize_template(template_))
            while ((i < attributeLists.size()) && (templateIDs[i] == *templateID)) {
                janus_image image;
                JANUS_CHECK(janus_read_image(fileNames[i].c_str(), &image))
                JANUS_CHECK(janus_augment(image, attributeLists[i], *template_));
                janus_free_image(image);
                i++;
                fprintf(stderr, "\rEnrolling %zu/%zu", i, attributeLists.size());
            }
        }
        return JANUS_SUCCESS;
    }
};

janus_error janus_create_template(janus_metadata metadata, janus_template *template_, janus_template_id *template_id)
{
    TemplateIterator ti(metadata);
    return ti.next(template_, template_id);
}

janus_error janus_create_gallery(janus_metadata metadata, janus_gallery gallery)
{
    TemplateIterator ti(metadata);
    JANUS_CHECK(ti.error)
    janus_template template_;
    janus_template_id templateID;
    JANUS_CHECK(ti.next(&template_, &templateID))
    while (template_ != NULL) {
        JANUS_CHECK(janus_enroll(template_, templateID, gallery))
        JANUS_CHECK(ti.next(&template_, &templateID))
    }
    return JANUS_SUCCESS;
}

struct FlatTemplate
{
    struct Data {
        janus_flat_template flat_template;
        size_t bytes, ref_count;
        janus_error error;
    } *data;

    FlatTemplate(janus_template template_)
    {
        data = new Data();
        data->flat_template = new janus_data[janus_max_template_size()];
        data->ref_count = 1;
        data->error = janus_finalize_template(template_, data->flat_template, &data->bytes);
    }

    FlatTemplate(const FlatTemplate& other)
    {
        *this = other;
    }

    FlatTemplate& operator=(const FlatTemplate& rhs)
    {
        data = rhs.data;
        data->ref_count++;
        return *this;
    }

    ~FlatTemplate()
    {
        data->ref_count--;
        if (data->ref_count == 0) {
            delete data->flat_template;
            delete data;
        }
    }

    janus_error compareTo(const FlatTemplate &other, float *similarity) const
    {
        double score;
        return janus_verify(data->flat_template, data->bytes, other.data->flat_template, other.data->bytes, &score);
        *similarity = score;
    }
};

static janus_error getFlatTemplates(janus_metadata metadata, vector<FlatTemplate> &flatTemplates)
{
    TemplateIterator ti(metadata);
    JANUS_CHECK(ti.error)
    janus_template template_;
    janus_template_id templateID;
    JANUS_CHECK(ti.next(&template_, &templateID))
    while (template_ != NULL) {
        flatTemplates.push_back(template_);
        JANUS_CHECK(flatTemplates.back().data->error)
        JANUS_CHECK(ti.next(&template_, &templateID))
    }
    return JANUS_SUCCESS;
}

static void writeMat(void *data, int rows, int columns, bool isMask, janus_metadata target, janus_metadata query, const char *matrix)
{
    ofstream stream(matrix);
    stream << "S2\n"
           << target << '\n'
           << query << '\n'
           << 'M' << (isMask ? 'B' : 'F') << ' '
           << rows << ' '  << columns << ' ';
    int endian = 0x12345678;
    stream.write((const char*)&endian, 4);
    stream << '\n';
    stream.write((const char*)data, rows * columns * (isMask ? 1 : 4));
}

janus_error janus_create_simmat(janus_metadata gallery_metadata,
                                janus_metadata probe_metadata,
                                const char *simmat)
{
    vector<FlatTemplate> gallery, probe;
    JANUS_CHECK(getFlatTemplates(gallery_metadata, gallery))
    JANUS_CHECK(getFlatTemplates(probe_metadata, probe))
    float *scores = new float[gallery.size() * probe.size()];
    for (size_t i=0; i<probe.size(); i++)
        for (size_t j=0; j<gallery.size(); j++)
            JANUS_CHECK(probe[i].compareTo(gallery[j], &scores[i*gallery.size()+j]));
    writeMat(scores, probe.size(), gallery.size(), false, gallery_metadata, probe_metadata, simmat);
    delete[] scores;
    return JANUS_SUCCESS;
}
