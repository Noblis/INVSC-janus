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
 * \param[out] image Address to store the decoded image.
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
 * \brief Helper function that provides timing information for janus_initialize.
 *
 * \param[in] sdk_path Path to the \em read-only directory containing the
 *                     janus-compliant SDK as provided by the implementer.
 * \param[in] temp_path Path to an existing empty \em read-write directory for
 *                      use as temporary file storage by the implementation.
 *                      This path is guaranteed until \ref janus_finalize.
 * \param[in] algorithm An empty string indicating the default algorithm, or an
 *                      implementation-defined string indicating an alternative
 *                      configuration.
 * \param[in] gpu_dev The GPU device number to be used by all subsequent
 * 			          implementation function calls
 * \remark This function is \ref thread_unsafe and should only be called once.
 * \see janus_finalize_helper
 */
JANUS_EXPORT janus_error janus_initialize_helper(const std::string &sdk_path, const std::string &temp_path, const std::string &algorithm, const int gpu_dev);

/*!
 * \brief Helper function that provides timing information for janus_finalize.
 * \remark This function is \ref thread_unsafe and should only be called once.
 * \see janus_initialize_helper
 */
JANUS_EXPORT janus_error janus_finalize_helper();

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
JANUS_EXPORT janus_error janus_verify_helper(const std::string& probe_data_path, const std::string& reference_data_path, const std::string& templates_matches_file, const std::string& scores_file, bool verbose);

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
 * \brief Print metrics to stderr
 * \note Will only print metrics with count > 0 occurrences.
 * \remark This function is \ref thread_unsafe.
 */
JANUS_EXPORT void janus_print_metrics();

/*! @}*/

#endif /* IARPA_JANUS_IO_H */
