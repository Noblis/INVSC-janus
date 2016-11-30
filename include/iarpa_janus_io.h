/*******************************************************************************
 * Copyright (c) 2013 Noblis, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 ******************************************************************************/

#ifndef IARPA_JANUS_IO_H
#define IARPA_JANUS_IO_H

#include <iarpa_janus.h>

/*!
 * \defgroup janus_io Janus I/O
 * \brief Media decoding and evaluation harness.
 * \addtogroup janus_io
 *  @{
 */

/*!
 * \brief The \c JANUS_ASSERT macro provides a simple unrecoverable error
 *        handling mechanism.
 * \see JANUS_CHECK
 */
#define JANUS_ASSERT(EXPRESSION)                                     \
{                                                                    \
    const janus_error error = (EXPRESSION);                          \
    if (error != JANUS_SUCCESS) {                                    \
        fprintf(stderr, "Janus error: %s\n\tFile: %s\n\tLine: %d\n", \
                janus_error_to_string(error),                        \
                __FILE__,                                            \
                __LINE__);                                           \
        abort();                                                     \
    }                                                                \
}                                                                    \

/*!
 * \brief The \c JANUS_CHECK macro provides a simple recoverable error handling
 *        mechanism.
 * \see JANUS_ASSERT
 */
#define JANUS_CHECK(EXPRESSION)             \
{                                           \
    const janus_error error = (EXPRESSION); \
    if (error != JANUS_SUCCESS)             \
        return error;                       \
}                                           \

/*!
 * \brief #janus_error to string.
 * \param[in] error Error code to stringify.
 * \note Memory for the return value is static and should not be freed.
 * \remark This function is \ref thread_safe.
 */
JANUS_EXPORT const char *janus_error_to_string(janus_error error);

/*!
 * \brief #janus_error from string.
 * \param[in] error String to decode.
 * \remark This function is \ref thread_safe.
 * \see janus_enum
 */
JANUS_EXPORT janus_error janus_error_from_string(const char *error);

/*!
 * \brief Read an image from disk.
 * \param[in] file_name Path to the image file.
 * \param[out] image Address to store the decoded image. This is allocated during
 *                   the function call.
 * \remark This function is \ref reentrant.
 * \see janus_free_image
 */
JANUS_EXPORT janus_error janus_load_media(const std::string &filename, janus_media &media);

/*!
 * \brief Frees the memory previously allocated for a #janus_image.
 * \param[in] image #janus_image to free.
 * \remark This function is \ref reentrant.
 * \see janus_allocate_image
 */
JANUS_EXPORT janus_error janus_free_media(janus_media &media);

/*!
 * \brief File name for a Janus Metadata File
 *
 * A *Janus Metadata File* is a *Comma-Separated Value* (CSV) text file with the following format:
 *
\verbatim
TEMPLATE_ID        , SUBJECT_ID, FILE_NAME, MEDIA_ID, FRAME, <janus_attribute>, <janus_attribute>, ..., <janus_attribute>
<janus_template_id>, <int>     , <string> , <int>   , <int>, <double>         , <double>         , ..., <double>
<janus_template_id>, <int>     , <string> , <int>   , <int>, <double>         , <double>         , ..., <double>
...
<janus_template_id>, <int>     , <string> , <int>   , <int>, <double>         , <double>         , ..., <double>
\endverbatim
 *
 * Where:
 * - [TEMPLATE_ID](\ref janus_template_id) is a unique integer identifier indicating rows that belong to the same template.
 * - \c SUBJECT_ID is a unique integer identifier used to establish ground truth match/non-match.
 *      For the purpose of experimentation, multiple \c TEMPLATE_ID may have the same \c SUBJECT_ID.
 * - \c FILE_NAME is a path to the image or video file on disk.
 * - \c MEDIA_ID is a unique integer identifier indicating rows that belong to the same piece of media (image or video clip).
 * - \c FRAME is the video frame number and -1 (or empty string) for still images.
 * - All rows associated with the same \c TEMPLATE_ID occur sequentially.
 * - All rows associated with the same \c TEMPLATE_ID and \c FILE_NAME occur sequentially ordered by \c FRAME.
 * - A cell is empty when no value is available for the specified attribute.
 *
 * \par Examples:
 * - [meds.csv](https://raw.githubusercontent.com/biometrics/janus/master/data/meds.csv)
 * - [Kirchner.csv](https://raw.githubusercontent.com/biometrics/janus/master/data/Kirchner.csv)
 * - [Toledo.csv](https://raw.githubusercontent.com/biometrics/janus/master/data/Toledo.csv)
 */
typedef const char *janus_metadata;

/*!
 * \brief High-level helper function for running face detection on a list of images
 * \param [in] data_path Prefix path to files in metadata.
 * \param [in] metadata #janus_metadata to detect faces in
 * \param [in] min_face_size The minimum width, in pixels, for detected faces
 * \param [in] detection_list_file The path to the file to store detection information
 * \param [in] verbose Print information and warnings during detection
 * \remark This function is \ref thread_unsafe
 */
JANUS_EXPORT janus_error janus_detect_helper(const std::string &data_path, janus_metadata metadata, const size_t min_face_size, const std::string &detection_list_file, bool verbose);

/*!
 * \brief High-level helper function for enrolling templates from a metadata file and writing templates to disk.
 * \param [in] data_path Prefix path to files in metadata.
 * \param [in] metadata #janus_metadata to enroll.
 * \param [in] output_path Directory to save the templates to
 * \param [in] output_file CSV file to hold the filenames, template ids, and subject_ids for the saved templates.
 *                         The format is templateID,subjectID,filename\n
 * \param [in] role The role for the templates
 * \param [in] verbose Print information and warnings during template enrollment.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT janus_error janus_create_templates_helper(const std::string &data_path, janus_metadata metadata, const std::string &templates_path, const std::string &templates_list_file, const janus_template_role role, bool verbose);

/*!
 * \brief High-level helper function for enrolling a gallery from a metadata file.
 * \param [in] templates_file Text file of templates to enroll into the gallery
 * \param [in] gallery_file File to save the gallery to
 * \param [in] verbose Print information and warnings during gallery enrollment.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT janus_error janus_create_gallery_helper(const std::string &templates_list_file, const std::string &gallery_file, bool verbose);

/*!
 * \brief High-level helper function for running verification on two equal sized lists of templates
 * \param [in] templates_list_file_a The first list of templates
 * \param [in] templates_list_file_b The second list of templates
 * \param [in] scores_file The file to write scores to. Scores are written template_id_a,template_id_b,similarity,genuine_match\n
 * \param [in] verbose Print information and warnings during verification.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT janus_error janus_verify_helper(const std::string &templates_list_file_a, const std::string &templates_list_file_b, const std::string &scores_file, bool verbose);

/*!
 * \brief High-level helper function for running verification on two equal sized lists of templates
 * \param [in] templates_list_file List of templates to enroll as probes
 * \param [in] gallery_file Path to the gallery to search against
 * \param [in] candidate_list File to write the candidate lists to. Each line in the file has the format probe_template_id,rank,gallery_template_id,similarity,genuine_match\n
 * \param [in] verbose Print information and warnings during search.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT janus_error janus_search_helper(const std::string &probes_list_file, const std::string &gallery_list_file, const std::string &gallery_file, int num_requested_returns, const std::string &candidate_list, bool verbose);

/*!
 * \brief High-level helper function for clustering templates
 * \param [in] templates_list_file The list of templates to be clustered
 * \param [in] hint Clustering hint
 * \param [in] clusters_output_list File to write cluster information to.
 * \param [in] verbose Print information and warnings during verification.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT janus_error janus_cluster_helper(const std::string &templates_list_file, const size_t hint, const std::string &clusters_output_list, bool verbose);

/*!
 * \brief A statistic.
 * \see janus_metrics
 */
struct janus_metric
{
    size_t count;  /*!< \brief Number of samples. */
    double mean;   /*!< \brief Sample average. */
    double stddev; /*!< \brief Sample standard deviation. */
};

/*!
 * \brief All statistics.
 * \see janus_get_metrics
 */
struct janus_metrics
{
    struct janus_metric janus_load_media_speed; /*!< \brief ms */
    struct janus_metric janus_free_media_speed; /*!< \brief ms */
    struct janus_metric janus_detection_speed; /*!< \brief ms */
    struct janus_metric janus_create_template_speed; /*!< \brief ms */
    struct janus_metric janus_serialize_template_speed; /*!< \brief ms */
    struct janus_metric janus_deserialize_template_speed; /*!< \brief ms */
    struct janus_metric janus_delete_serialized_template_speed; /*!< \brief ms */
    struct janus_metric janus_delete_template_speed; /*!< \brief ms */
    struct janus_metric janus_verify_speed; /*!< \brief ms */
    struct janus_metric janus_create_gallery_speed; /*!< \brief ms */
    struct janus_metric janus_prepare_gallery_speed; /*!< \brief ms */
    struct janus_metric janus_gallery_insert_speed; /*!< \brief ms */
    struct janus_metric janus_gallery_remove_speed; /*!< \brief ms */
    struct janus_metric janus_serialize_gallery_speed; /*!< \brief ms */
    struct janus_metric janus_deserialize_gallery_speed; /*!< \brief ms */
    struct janus_metric janus_delete_serialized_gallery_speed; /*!< \brief ms */
    struct janus_metric janus_delete_gallery_speed; /*!< \brief ms */
    struct janus_metric janus_search_speed; /*!< \brief ms */
    struct janus_metric janus_cluster_speed; /*!< \brief ms */

    struct janus_metric janus_gallery_size; /*!< \brief KB */
    struct janus_metric janus_template_size; /*!< \brief KB */
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
};

/*!
 * \brief Retrieve and reset performance metrics.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT struct janus_metrics janus_get_metrics();

/*!
 * \brief Print metrics to stdout.
 * \note Will only print metrics with count > 0 occurrences.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT void janus_print_metrics(struct janus_metrics metrics);

/*! @}*/

#endif /* IARPA_JANUS_IO_H */
