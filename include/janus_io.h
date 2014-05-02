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

#ifndef JANUS_IO_H
#define JANUS_IO_H

#include <janus.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 * \brief The \c JANUS_CHECK macro provides a simple recoverable error
 *        handling mechanism
 * \see JANUS_ASSERT
 */
#define JANUS_CHECK(EXPRESSION)             \
{                                           \
    const janus_error error = (EXPRESSION); \
    if (error != JANUS_SUCCESS)             \
        return error;                       \
}                                           \

/*! \page janus_enum Enum Naming Convention
 * #janus_attribute, #janus_color_space, #janus_error and enum values follow a
 * \c CAPITAL_CASE naming convention. Functions #janus_attribute_to_string and
 * #janus_error_to_string return a string for the corresponding enum by
 * removing the leading \c JANUS_:
 * \code
 * janus_attribute_to_string(JANUS_RIGHT_EYE_X); // returns "RIGHT_EYE_X"
 * \endcode
 * Functions #janus_attribute_from_string and #janus_error_from_string provide
 * the opposite functionality:
 * \code
 * janus_attribute_from_string("RIGHT_EYE_X"); // returns JANUS_RIGHT_EYE_X
 * \endcode
 * \note #janus_attribute_from_string is used to decode #janus_metadata
 * files, so attribute column names should follow this naming convention.
 */

/*!
 * \brief #janus_error to string.
 * \param[in] error Error code to stringify.
 * \note Memory for the return value is managed internally and should not be
 *       freed.
 * \see janus_enum
 */
JANUS_EXPORT const char *janus_error_to_string(janus_error error);

/*!
 * \brief #janus_error from string.
 * \param[in] error String to decode.
 * \see janus_enum
 */
JANUS_EXPORT janus_error janus_error_from_string(const char *error);

/*!
 * \brief #janus_attribute to string
 * \param[in] attribute Attribute code to stringify.
 * \note Memory for the return value is managed internally and should not be
 *       freed.
 * \see janus_enum
 */
JANUS_EXPORT const char *janus_attribute_to_string(janus_attribute attribute);

/*!
 * \brief #janus_attribute from string.
 * \param[in] attribute String to decode.
 * \see janus_enum
 */
JANUS_EXPORT janus_attribute janus_attribute_from_string(const char *attribute);

/*!
 * \brief Read an image from disk.
 * \param[in] file_name Path to the image file.
 * \param[out] image Address to store the decoded image.
 * \see janus_free_image
 */
JANUS_EXPORT janus_error janus_read_image(const char *file_name, janus_image *image);

/*!
 * \brief Frees the memory previously allocated for a #janus_image.
 * \param[in] image #janus_image to free.
 * \see janus_allocate_image
 */
JANUS_EXPORT void janus_free_image(janus_image image);

/*!
 * \brief Handle to a private video decoding type.
 */
typedef struct janus_video_type *janus_video;

/*!
 * \brief Returns a video ready for reading.
 * \param[in] file_name Path to image file.
 * \param[out] video Address to store the allocated video.
 * \see janus_read_frame janus_close_video
 */
JANUS_EXPORT janus_error janus_open_video(const char *file_name, janus_video *video);

/*!
 * \brief Returns the current frame and advances the video to the next frame.
 * \param[in] video Video to decode.
 * \param[out] image Address to store the allocated image.
 * \see janus_open_video janus_free_image
 */
JANUS_EXPORT janus_error janus_read_frame(janus_video video, janus_image *image);

/*!
 * \brief Closes a video previously opened by \ref janus_open_video.
 * \param[in] video The video to close.
 * Call this function to deallocate the memory allocated to decode the video
 * after the desired frames have been read.
 */
JANUS_EXPORT void janus_close_video(janus_video video);

/*!
 * \brief File name for a Janus Metadata File
 *
 * A *Janus Metadata File* is a *Comma-Separated Value* (CSV) text file with the following format:
 *
\verbatim
TEMPLATE_ID        , FILE_NAME, FRAME, <janus_attribute>, <janus_attribute>, ..., <janus_attribute>
<janus_template_id>, <string> , <int>, <double>         , <double>         , ..., <double>
<janus_template_id>, <string> , <int>, <double>         , <double>         , ..., <double>
...
<janus_template_id>, <string> , <int>, <double>         , <double>         , ..., <double>
\endverbatim
 *
 * Where:
 * - [TEMPLATE_ID](\ref janus_template_id) is a unique integer identifier indicating rows that belong to the same template.
 * - \c FILE_NAME is a path to the image or video file on disk.
 * - \c FRAME is the video frame number and -1 (or empty string) for still images.
 * - \a \<janus_attribute\> adheres to \ref janus_enum.
 * - All rows associated with the same \c TEMPLATE_ID occur sequentially.
 * - All rows associated with the same \c TEMPLATE_ID and \c FILE_NAME occur sequentially ordered by \c FRAME.
 * - A cell is empty when no value is available for the specified #janus_attribute.
 *
 * \par Examples:
 * - [Kirchner.csv](https://raw.github.com/biometrics/janus/master/data/Kirchner.csv)
 * - [Toledo.csv](https://raw.github.com/biometrics/janus/master/data/Toledo.csv)
 */
typedef const char *janus_metadata;

/*!
 * \brief High-level function for enrolling a template from a metadata file.
 * \param [in] metadata #janus_metadata file to enroll.
 * \param [out] template_ Constructed template.
 * \param [out] template_id Template ID from metadata.
 */
JANUS_EXPORT janus_error janus_create_template(janus_metadata metadata, janus_template *template_, janus_template_id *template_id);

/*!
 * \brief High-level function for enrolling a gallery from a metadata file.
 * \param [in] metadata #janus_metadata to enroll.
 * \param [in] gallery File to save the gallery to.
 */
JANUS_EXPORT janus_error janus_create_gallery(janus_metadata metadata, janus_gallery gallery);

/*!
 * \brief A dense 2D matrix file.
 *
 * Can be either the \a similarity or \a mask matrix format described in
 * <a href="http://openbiometrics.org/doxygen/latest/MBGC_file_overview.pdf#page=12">MBGC File Overview</a>.
 * \see janus_create_mask janus_create_simmat
 */
typedef const char *janus_matrix;

/*!
 * \brief Create a mask matrix from two galleries.
 *
 * The \c Template_ID field is used to determine ground truth match/non-match.
 * \param[in] target_metadata Templates to constitute the columns of the matrix.
 * \param[in] query_metadata Templates to constitute the rows for the matrix.
 * \param[in] mask Mask matrix file to be created.
 * \see janus_create_simmat
 */
JANUS_EXPORT janus_error janus_create_mask(janus_metadata target_metadata,
                                           janus_metadata query_metadata,
                                           janus_matrix mask);

/*!
 * \brief Create a similarity matrix from two galleries.
 *
 * Similarity scores are computed using #janus_verify.
 * \param[in] target_metadata Templates to constitute the columns of the matrix.
 * \param[in] query_metadata Templates to constitute the rows for the matrix.
 * \param[in] simmat Similarity matrix file to be created.
 * \param[in] data_path Optional prefix path to image location.
 * \see janus_create_mask
 */
JANUS_EXPORT janus_error janus_create_simmat(janus_metadata target_metadata,
                                             janus_metadata query_metadata,
                                             janus_matrix simmat,
                                             const char *data_path);

/*!
 * \brief A statistic.
 * \see janus_metrics
 */
struct janus_metric
{
    size_t count;
    double mean, stddev;
};

/*!
 * \brief All statistics.
 * \see janus_get_metrics
 */
struct janus_metrics
{
    janus_metric janus_augment_speed; /*!< \brief calls / millisecond */
    janus_metric janus_verify_speed;  /*!< \brief calls / millisecond */
};

/*! \brief Retrieve and reset performance metrics. */
JANUS_EXPORT janus_metrics janus_get_metrics();

/*!
 * \brief Print metrics to stdout.
 * \note Will only print metrics with count > 0 occurrences.
 */
JANUS_EXPORT void janus_print_metrics(janus_metrics metrics);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_IO_H */
