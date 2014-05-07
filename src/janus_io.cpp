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

#include "janus_io.h"

using namespace std;

#define ENUM_CASE(X) case JANUS_##X: return #X;
#define ENUM_COMPARE(X,Y) if (!strcmp(#X, Y)) return JANUS_##X;

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
        ENUM_CASE(INVALID_IMAGE)
        ENUM_CASE(INVALID_VIDEO)
        ENUM_CASE(MISSING_TEMPLATE_ID)
        ENUM_CASE(MISSING_FILE_NAME)
        ENUM_CASE(NULL_ATTRIBUTE_LIST)
        ENUM_CASE(NUM_ERRORS)
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
    ENUM_COMPARE(INVALID_IMAGE, error)
    ENUM_COMPARE(INVALID_VIDEO, error)
    ENUM_COMPARE(MISSING_TEMPLATE_ID, error)
    ENUM_COMPARE(MISSING_FILE_NAME, error)
    ENUM_COMPARE(NULL_ATTRIBUTE_LIST, error)
    ENUM_COMPARE(NUM_ERRORS, error)
    return JANUS_UNKNOWN_ERROR;
}

const char *janus_attribute_to_string(janus_attribute attribute)
{
    switch (attribute) {
        ENUM_CASE(INVALID_ATTRIBUTE)
        ENUM_CASE(FRAME)
        ENUM_CASE(RIGHT_EYE_X)
        ENUM_CASE(RIGHT_EYE_Y)
        ENUM_CASE(LEFT_EYE_X)
        ENUM_CASE(LEFT_EYE_Y)
        ENUM_CASE(NOSE_BASE_X)
        ENUM_CASE(NOSE_BASE_Y)
        ENUM_CASE(NUM_ATTRIBUTES)
    }
    return "INVALID_ATTRIBUTE";
}

janus_attribute janus_attribute_from_string(const char *attribute)
{
    ENUM_COMPARE(INVALID_ATTRIBUTE, attribute)
    ENUM_COMPARE(FRAME, attribute)
    ENUM_COMPARE(RIGHT_EYE_X, attribute)
    ENUM_COMPARE(RIGHT_EYE_Y, attribute)
    ENUM_COMPARE(LEFT_EYE_X, attribute)
    ENUM_COMPARE(LEFT_EYE_Y, attribute)
    ENUM_COMPARE(NOSE_BASE_X, attribute)
    ENUM_COMPARE(NOSE_BASE_Y, attribute)
    ENUM_COMPARE(NUM_ATTRIBUTES, attribute)
    return JANUS_INVALID_ATTRIBUTE;
}

// For computing metrics
static vector<double> janus_initialize_template_samples;
static vector<double> janus_augment_samples;
static vector<double> janus_finalize_template_samples;
static vector<double> janus_read_image_samples;
static vector<double> janus_free_image_samples;
static vector<double> janus_verify_samples;
static vector<double> janus_template_size_samples;
static vector<double> janus_gallery_size_samples;
static vector<double> janus_compare_samples;

struct TemplateIterator
{
    vector<string> fileNames;
    vector<janus_template_id> templateIDs;
    vector<janus_attribute_list> attributeLists;
    size_t i;
    string dataPath;
    bool verbose;

    TemplateIterator(janus_metadata metadata, string dataPath, bool verbose)
        : i(0), dataPath(dataPath), verbose(verbose)
    {
        ifstream file(metadata);

        // Parse header
        string line;
        getline(file, line);
        istringstream attributeNames(line);
        string attributeName;
        getline(attributeNames, attributeName, ','); // TEMPLATE_ID
        getline(attributeNames, attributeName, ','); // FILE_NAME
        vector<janus_attribute> attributes;
        while (getline(attributeNames, attributeName, ',')) {
            attributeName.erase(remove_if(attributeName.begin(), attributeName.end(), ::isspace), attributeName.end());
            attributes.push_back(janus_attribute_from_string(attributeName.c_str()));
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
            for (int j=0; getline(attributeValues, attributeValue, ','); j++) {
                if (attributeValue.empty())
                    continue;
                attributeList.attributes[attributeList.size] = attributes[j];
                attributeList.values[attributeList.size] = atof(attributeValue.c_str());
                attributeList.size++;
            }
            attributeLists.push_back(attributeList);
        }

        if (verbose)
            fprintf(stderr, "\rEnrolling %zu/%zu", i, attributeLists.size());
    }

    janus_error next(janus_template *template_, janus_template_id *templateID)
    {
        if (i >= attributeLists.size()) {
            *template_ = NULL;
            *templateID = -1;
            fprintf(stderr, "\n");
        } else {
            *templateID = templateIDs[i];

            clock_t start = clock();
            JANUS_CHECK(janus_initialize_template(template_))
            janus_initialize_template_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

            while ((i < attributeLists.size()) && (templateIDs[i] == *templateID)) {
                janus_image image;

                start = clock();
                JANUS_CHECK(janus_read_image((dataPath + fileNames[i]).c_str(), &image))
                janus_read_image_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

                start = clock();
                JANUS_CHECK(janus_augment(image, attributeLists[i], *template_));
                janus_augment_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

                start = clock();
                janus_free_image(image);
                janus_free_image_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

                i++;
                if (verbose)
                    fprintf(stderr, "\rEnrolling %zu/%zu", i, attributeLists.size());
            }
        }
        return JANUS_SUCCESS;
    }
};

janus_error janus_create_template(janus_metadata metadata, janus_template *template_, janus_template_id *template_id)
{
    TemplateIterator ti(metadata, "", false);
    return ti.next(template_, template_id);
}

janus_error janus_create_gallery(const char *data_path, janus_metadata metadata, janus_gallery gallery)
{
    TemplateIterator ti(metadata, data_path, true);
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
        data->ref_count = 1;

        janus_data *buffer = new janus_data[janus_max_template_size()];

        const clock_t start = clock();
        data->error = janus_finalize_template(template_, buffer, &data->bytes);
        janus_finalize_template_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);
        janus_template_size_samples.push_back(data->bytes / 1024.0);

        data->flat_template = new janus_data[data->bytes];
        memcpy(data->flat_template, buffer, data->bytes);
        delete[] buffer;
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
            delete[] data->flat_template;
            delete data;
        }
    }

    janus_error compareTo(const FlatTemplate &other, float *similarity) const
    {
        const clock_t start = clock();
        janus_error error = janus_verify(data->flat_template, data->bytes, other.data->flat_template, other.data->bytes, similarity);
        janus_verify_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);
        return error;
    }
};

janus_error janus_write_matrix(void *data, int rows, int columns, int is_mask, janus_gallery target, janus_gallery query, janus_matrix matrix)
{
    ofstream stream(matrix);
    stream << "S2\n"
           << target << '\n'
           << query << '\n'
           << 'M' << (is_mask ? 'B' : 'F') << ' '
           << rows << ' ' << columns << ' ';
    int endian = 0x12345678;
    stream.write((const char*)&endian, 4);
    stream << '\n';
    stream.write((const char*)data, rows * columns * (is_mask ? 1 : 4));
    return JANUS_SUCCESS;
}

janus_error janus_evaluate(janus_gallery target, janus_gallery query, janus_matrix mask, janus_matrix simmat)
{
    size_t target_size, query_size;
    clock_t start = clock();
    JANUS_CHECK(janus_gallery_size(target, &target_size))
    janus_gallery_size_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

    start = clock();
    JANUS_CHECK(janus_gallery_size(query, &query_size))
    janus_gallery_size_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

    float *similarity_matrix = new float[target_size * query_size];
    janus_template_id *target_ids = new janus_template_id[target_size];
    janus_template_id *query_ids = new janus_template_id[query_size];

    start = clock();
    JANUS_CHECK(janus_compare(target, query, similarity_matrix, target_ids, query_ids))
    janus_compare_samples.push_back(1000.0 * (clock() - start) / CLOCKS_PER_SEC);

    JANUS_CHECK(janus_write_matrix(similarity_matrix, query_size, target_size, false, target, query, simmat))
    delete[] similarity_matrix;

    unsigned char *truth = new unsigned char[target_size * query_size];
    for (size_t i=0; i<query_size; i++)
        for (size_t j=0; j<target_size; j++)
            truth[i*target_size+j] = (query_ids[i] == target_ids[j] ? 0xff : 0x7f);
    JANUS_CHECK(janus_write_matrix(truth, query_size, target_size, true, target, query, mask))
    delete[] truth;

    delete[] query_ids;
    delete[] target_ids;
    return JANUS_SUCCESS;
}

static janus_metric calculateMetric(const vector<double> &samples)
{
    janus_metric metric;
    metric.count = samples.size();

    if (metric.count > 0) {
        metric.mean = 0;
        for (size_t i=0; i<samples.size(); i++)
            metric.mean += samples[i];
        metric.mean /= samples.size();

        metric.stddev = 0;
        for (size_t i=0; i<samples.size(); i++)
            metric.stddev += pow(samples[i] - metric.mean, 2.0);
        metric.stddev = sqrt(metric.stddev / samples.size());
    } else {
        metric.mean = std::numeric_limits<double>::quiet_NaN();
        metric.stddev = std::numeric_limits<double>::quiet_NaN();
    }

    return metric;
}

janus_metrics janus_get_metrics()
{
    janus_metrics metrics;
    metrics.janus_initialize_template_speed = calculateMetric(janus_initialize_template_samples);
    metrics.janus_augment_speed             = calculateMetric(janus_augment_samples);
    metrics.janus_finalize_template_speed   = calculateMetric(janus_finalize_template_samples);
    metrics.janus_read_image_speed          = calculateMetric(janus_read_image_samples);
    metrics.janus_free_image_speed          = calculateMetric(janus_free_image_samples);
    metrics.janus_verify_speed              = calculateMetric(janus_verify_samples);
    metrics.janus_gallery_size_speed        = calculateMetric(janus_gallery_size_samples);
    metrics.janus_compare_speed             = calculateMetric(janus_compare_samples);
    metrics.janus_template_size             = calculateMetric(janus_template_size_samples);
    return metrics;
}

static void printMetric(const char *name, janus_metric metric, bool speed = true)
{
    if (metric.count > 0)
        printf("%s\t%.2g\t%.2g\t%s\t%.2g\n", name, metric.mean, metric.stddev, speed ? "ms" : "KB", double(metric.count));
}

void janus_print_metrics(janus_metrics metrics)
{
    printf(     "API Symbol               \tMean\tStdDev\tUnits\tCount\n");
    printMetric("janus_initialize_template", metrics.janus_initialize_template_speed);
    printMetric("janus_augment            ", metrics.janus_augment_speed);
    printMetric("janus_finalize_template  ", metrics.janus_finalize_template_speed);
    printMetric("janus_read_image         ", metrics.janus_read_image_speed);
    printMetric("janus_free_image         ", metrics.janus_free_image_speed);
    printMetric("janus_verify             ", metrics.janus_verify_speed);
    printMetric("janus_gallery_size       ", metrics.janus_gallery_size_speed);
    printMetric("janus_compare            ", metrics.janus_compare_speed);
    printMetric("janus_flat_template      ", metrics.janus_template_size, false);
}
