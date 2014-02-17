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

#ifndef JANUS_H
#define JANUS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \mainpage
 * \section overview Overview
 *
 * Janus is a *C* API consisting of three header files:
 *
 * Header      | Documentation  | Required | Description
 * ----------- | -------------  | -------- | -----------
 * janus.h     | \ref janus     | **Yes**  | \copybrief janus
 * janus_io.h  | \ref janus_io  | No       | \copybrief janus_io
 * janus_aux.h | \ref janus_aux | No       | \copybrief janus_aux
 *
 * [<b>Source Code</b>](https://github.com/biometrics/janus)

 * \ref more_information
 *
 * \subsection license License
 * The API is provided under a [BSD-like license](LICENSE.txt) and is
 * *free for academic and commercial use*.
 *
 * \subsection Feedback
 * Feedback on the API is strongly desired, please direct any questions or
 * comments to [janus-dev@googlegroups.com]
 * (https://groups.google.com/forum/#!forum/janus-dev).
 *
 * \page more_information More Information
 * \brief Additional technical considerations.
 *
 * \section api_conventions API Conventions
 * \subsection thread_safety Thread Safety
 * All functions are thread-safe unless noted otherwise.
 *
 * \section notes_to_users Notes to Users
 * - Function return values are either \c void or #janus_error.
 * - Input parameters are passed by value.
 * - Output parameters are passed by address.
 * - #JANUS_TRY provides a light-weight mechanism for error checking.
 *
 * \section notes_to_implementers Notes for Implementers
 * - Define \c JANUS_LIBRARY during compilation to export Janus symbols and
 *   compile a Unix implementation with \c \-fvisibility=hidden.
 * - Follow the <a href="http://www.pathname.com/fhs/">Filesystem Hierarchy
 *   Standard</a> by organizing the implementation into \c bin, \c include,
 *   \c lib, \c share and \c src sub-folders.
 *
 * \section miscellaneous Miscellaneous
 * - [Janus Program Homepage](http://www.iarpa.gov/Programs/sc/Janus/janus.html)
 *   [iarpa.gov]
 */

/*!
 * \defgroup janus Janus
 * \brief Mandatory interface for Phase 1.
 */
#if defined JANUS_LIBRARY
#  if defined _WIN32 || defined __CYGWIN__
#    define JANUS_EXPORT __declspec(dllexport)
#  else
#    define JANUS_EXPORT __attribute__((visibility("default")))
#  endif
#else
#  if defined _WIN32 || defined __CYGWIN__
#    define JANUS_EXPORT __declspec(dllimport)
#  else
#    define JANUS_EXPORT
#  endif
#endif

#define JANUS_VERSION_MAJOR 0
#define JANUS_VERSION_MINOR 1
#define JANUS_VERSION_PATCH 0

/*!
 * \brief Return type for functions that indicate an error status.
 *
 * All error values are positive integers, with the exception of #JANUS_SUCCESS
 * = 0 which indicates no error.
 * Values in the inclusive interval \f$\left[2^{16},2^{32}-1\right]\f$ are
 * reserved for implementer use.
 */
typedef enum janus_error
{
    JANUS_SUCCESS              = 0,  /*!< No error */
    JANUS_UNKNOWN_ERROR        = 1,  /*!< Catch-all error code */
    JANUS_OUT_OF_MEMORY        = 2,  /*!< Memorry allocation failed */
    JANUS_INVALID_SDK_PATH     = 3,  /*!< Incorrect location provided to
                                         #janus_initialize */
    JANUS_OPEN_ERROR           = 4,  /*!< Failed to open a file */
    JANUS_READ_ERROR           = 5,  /*!< Failed to read from a file */
    JANUS_WRITE_ERROR          = 6,  /*!< Failed to write to a file */
    JANUS_PARSE_ERROR          = 7,  /*!< Failed to parse file */
    JANUS_INVALID_IMAGE        = 8,  /*!< Could not decode image file */
    JANUS_INVALID_VIDEO        = 9,  /*!< Could not decode video file */
    JANUS_MISSING_TEMPLATE_ID  = 10, /*!< Expected a missing template ID */
    JANUS_MISSING_FILE_NAME    = 11, /*!< Expected a missing file name */
    JANUS_NULL_ATTRIBUTE_LIST  = 13, /*!< Null #janus_attribute_list */
    JANUS_TEMPLATE_ID_MISMATCH = 14  /*!< Expected matching template IDs */
} janus_error;

/*!
 * \brief Returns the stringified error.
 * \note Memory for the return value is managed internally and should not be
 *       freed.
 */
JANUS_EXPORT const char *janus_error_to_string(janus_error error);

/*!
 * \brief The \c JANUS_TRY macro provides a simple error handling mechanism.
 */
#define JANUS_TRY(JANUS_API_CALL)                           \
{                                                           \
    const janus_error error = (JANUS_API_CALL);             \
    if (error != JANUS_SUCCESS) {                           \
        printf("Janus error: %s\n\tFile: %s\n\tLine: %d\n", \
               janus_error_to_string(error),                \
               __FILE__,                                    \
               __LINE__);                                   \
        abort();                                            \
    }                                                       \
}                                                           \

/*!
 * \brief Data buffer type.
 */
typedef uint8_t janus_data;

/*!
 * \brief Supported image formats.
 */
typedef enum janus_color_space
{
    JANUS_GRAY8 = 0, /*!< \brief 1 channel grayscale, 8-bit depth. */
    JANUS_BGR24 = 1  /*!< \brief 3 channel color (BGR order), 8-bit depth. */
} janus_color_space;

/*!
 * \brief Common representation for still images and video frames.
 *
 * Pixels are stored \em continuously in the janus_image::data buffer.
 * Pixel layout with respect to decreasing spatial locality is \a channel,
 * \a column, \a row.
 * Thus pixel intensity can be retrieved as follows:
 *
\code
janus_data get_intensity(janus_image image, size_t channel, size_t column,
                                                                     size_t row)
{
    const size_t columnStep = (image.image_format == JANUS_COLOR ? 3 : 1);
    const size_t rowStep = image.columns * columnStep;
    const size_t index = row*rowStep + column*columnStep + channel;
    return image.data[index];
}
\endcode
 *
 * (0, 0) corresponds to the top-left corner of the image.
 * (width-1, height-1) corresponds to the bottom-right corner of the image.
 */
typedef struct janus_image
{
    janus_data *data;              /*!< \brief Data buffer. */
    size_t width;                  /*!< \brief Column count in pixels. */
    size_t height;                 /*!< \brief Row count in pixels. */
    janus_color_space color_space; /*!< \brief Arrangement of #data. */
} janus_image;

/*!
 * \brief A measurement made on a #janus_image.
 *
 * These attributes are provided from manual annotation for Phase 1.
 * Phases 2 and 3 will introduce API calls for automated detection.
 * Additional attributes and training data will be added over the duration of
 * the program.
 */
typedef enum janus_attribute
{
    JANUS_INVALID_ATTRIBUTE   = 0,  /*!< Catch-all error code */
    JANUS_FRAME               = 1,  /*!< Video frame number, -1 (or not present)
                                         for still images */
    JANUS_RIGHT_EYE_X         = 32, /*!< Face landmark (pixels) */
    JANUS_RIGHT_EYE_Y         = 33, /*!< Face landmark (pixels) */
    JANUS_LEFT_EYE_X          = 34, /*!< Face landmark (pixels) */
    JANUS_LEFT_EYE_Y          = 35, /*!< Face landmark (pixels) */
    JANUS_NOSE_BASE_X         = 36, /*!< Face landmark (pixels) */
    JANUS_NOSE_BASE_Y         = 37  /*!< Face landmark (pixels) */
} janus_attribute;

/*!
 * \brief Returns the stringified attribute.
 * \note Memory for the return value is managed internally and should not be
 *       freed.
 */
JANUS_EXPORT const char *janus_attribute_to_string(janus_attribute attribute);

/*!
 * \brief A list of #janus_attribute and value pairs all belonging to a the same
 *        object in a particular image.
 */
typedef struct janus_attribute_list
{
    size_t size; /*!< \brief Size of #attributes and #values. */
    janus_attribute *attributes; /*!< \brief Array of #janus_attribute. */
    double *values; /*!< \brief Array of corresponding attribute values. */
} janus_attribute_list;

/*!
 * \brief Call once at the start of the application, before making any other
 * calls to the API.
 *
 * \param[in] sdk_path Path to the \em read-only directory containing the
 *                     janus-compliant SDK as provided by the implementer.
 * \param[in] model_file Path to a trained model file created by
 *                       \ref janus_train or an empty string indicating the
 *                       default algorithm.
 * \note This function should only be called once.
 * \see janus_finalize
 */
JANUS_EXPORT janus_error janus_initialize(const char *sdk_path,
                                          const char *model_file);

/*!
 * \brief Call once at the end of the application, after making all other calls
 * to the API.
 * \note This function should only be called once.
 * \see janus_initialize
 */
JANUS_EXPORT janus_error janus_finalize();

/*!
 * \brief Contains the recognition information for an object.
 *
 * Create a new template with \ref janus_initialize_template.
 * Add images and videos to the template using \ref janus_add_image and
 * \ref janus_set_tracking.
 * Finalize the template for comparison with \ref janus_finalize_template.
 * \see janus_flat_template
 */
typedef struct janus_template_type *janus_template;

/*!
 * \brief A finalized representation of a template suitable for comparison.
 * \see janus_template
 */
typedef janus_data *janus_flat_template;

/*!
 * \brief The maximum allowed template size is 32 MB.
 */
#define JANUS_MAX_TEMPLATE_SIZE_LIMIT 33554432

/*!
 * \brief The maximum size of templates generated by
 *        \ref janus_finalize_template.
 *
 * Should be less than or equal to \ref JANUS_MAX_TEMPLATE_SIZE_LIMIT.
 */
JANUS_EXPORT size_t janus_max_template_size();

/*!
 * \brief Create an empty template for enrollment.
 * \param[in] template_ The template to initialize for enrollment.
 */
JANUS_EXPORT janus_error janus_initialize_template(janus_template *template_);

/*!
 * \brief Add information to the template.
 * \param[in] image The image containing the detected object.
 * \param[in] attributes Location and metadata associated with the detected
 *                       object to recognize.
 * \param[in,out] template_ The template to contain the object's recognition
 *                          information.
 * \see janus_set_tracking
 */
JANUS_EXPORT janus_error janus_add_image(const janus_image image,
                                         const janus_attribute_list attributes,
                                         janus_template template_);

/*!
 * \brief Enable or disable object tracking.
 * \param[in] template_ The template to contain the tracked object.
 * \param[in] enabled Subsequent calls to \ref janus_add_image will have a
 *                    temporal relationship.
 * \see janus_add_image
 */
JANUS_EXPORT janus_error janus_set_tracking(janus_template template_,
                                            int enabled);

/*!
 * \brief Create the final template representation.
 * \param[in] template_ The recognition information to contruct the
 *                      template from.
 * \param[in,out] flat_template A pre-allocated buffer no smaller than
 *                              \ref janus_max_template_size to contain the
 *                              final template.
 * \param[out] bytes Size of the buffer actually used to store the template.
 * \note template_ is deallocated by this function.
 */
JANUS_EXPORT janus_error janus_finalize_template(janus_template template_,
                                                 janus_flat_template
                                                                  flat_template,
                                                 size_t *bytes);

/*!
 * \brief Return a similarity score for two templates.
 * \param[in] a The first template to compare.
 * \param[in] a_bytes Size of template a.
 * \param[in] b The second template to compare.
 * \param[in] b_bytes Size of template b.
 * \param[out] similarity Higher values indicate greater similarity.
 * \see janus_search
 */
JANUS_EXPORT janus_error janus_verify(const janus_flat_template a,
                                      const size_t a_bytes,
                                      const janus_flat_template b,
                                      const size_t b_bytes,
                                      double *similarity);

/*!
 * \brief Unique identifier for a \ref janus_template.
 *
 * Associate a template with a unique id using \ref janus_enroll.
 * Retrieve the unique id from a search using \ref janus_search.
 */
typedef int janus_template_id;

/*!
 * \brief A set of \ref janus_template in a file.
 *
 * Add templates to the gallery using \ref janus_enroll.
 */
typedef const char *janus_gallery;

/*!
 * \brief Add a template to the gallery.
 * \param[in] template_ The template to add.
 * \param[in] template_id A unique identifier for the template.
 * \param[in] gallery The gallery to take ownership of the template. The file
 *                    will be created if it does not already exist.
 * \note template_ is deallocated by this function.
 */
JANUS_EXPORT janus_error janus_enroll(const janus_template template_,
                                      const janus_template_id template_id,
                                      janus_gallery gallery);

/*!
 * \brief Ranked search for a template against a gallery.
 * \param [in] template_ Probe to search for.
 * \param [in] gallery Gallery to search against.
 * \param [in] requested_returns The desired number of returned results.
 * \param [out] template_ids Buffer to contain the unique identifiers of the top
 *                           matching templates.
 * \param [out] similarities Buffer to contain the similarity score for each of
 *                           the top matching templates.
 * \param [out] actual_returns The number of populated elements in template_ids
 *                             and similarities.
 * \note template_ids and similaries should be pre-allocated buffers large
 *       enough to contain requested_returns elements. actual_returns will be
 *       less than or equal to requested_returns.
 * \see janus_verify
 */
JANUS_EXPORT janus_error janus_search(const janus_template template_,
                                      janus_gallery gallery,
                                      int requested_returns,
                                      janus_template_id *template_ids,
                                      double *similarities,
                                      int *actual_returns);

/*!
 * \brief Train a new model from the provided templates.
 * \param[in] templates Training data to generate the model file.
 * \param[in] num_templates Length of \em partial_templates.
 * \param[out] model_file File path to contain the trained model.
 */
JANUS_EXPORT janus_error janus_train(const janus_template *templates,
                                     const int num_templates,
                                     const char *model_file);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_H */
