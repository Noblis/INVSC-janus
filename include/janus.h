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

#include <inttypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \mainpage
 * \section overview Overview
 *
 * Janus is a \a C API consisting of three header files:
 *
 * Header      | Documentation  | Required | Description
 * ----------- | -------------  | -------- | -----------
 * janus.h     | \ref janus     | \b Yes   | \copybrief janus
 * janus_aux.h | \ref janus_aux | No       | \copybrief janus_aux
 * janus_io.h  | \ref janus_io  | No       | \copybrief janus_io
 *
 * \ref more_information
 *
 * \subsection license License
 * The API is provided under a [BSD-like license](LICENSE.txt) and is
 * _free for academic and commercial use_.
 *
 * \subsection Feedback
 * Feedback on the API is strongly desired.
 * Please direct any questions or comments to the current maintainer listed
 * below.
 *
 * \author Joshua C. Klontz (Joshua.Klontz@noblis.org) - Current Maintainer
 * // Todo: janus-dev@noblis.org
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
 * - Output parameters are passed by reference/pointer/address.
 * - Input parameters are passed by value.
 * - Parameters that won't be modified are marked \c const.
 * - #JANUS_TRY provides a light-weight mechanism for checking errors.
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

///@{
/*!
 * \anchor version
 * \name Version
 * Janus uses the <a href="http://semver.org/">Semantic Versioning</a>
 * convention for assigning and incrementing version numbers.
 */
#define JANUS_VERSION_MAJOR 0
#define JANUS_VERSION_MINOR 1
#define JANUS_VERSION_PATCH 0
///@}

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
    JANUS_MISSING_TEMPLATE_ID  = 10,  /*!< Expected a missing template ID */
    JANUS_MISSING_FILE_NAME    = 11, /*!< Expected a missing file name */
    JANUS_NULL_VALUE           = 12, /*!< Value of #janus_value was 0 */
    JANUS_NULL_ATTRIBUTE_LIST  = 13, /*!< Value of #janus_attribute_list was 0 */
    JANUS_TEMPLATE_ID_MISMATCH = 14  /*!< Expected matching template IDs */
} janus_error;

/*!
 * \brief Returns a human-readable error message.
 * \note Memory for the return value is managed internally and should not be
 *       freed.
 */
JANUS_EXPORT const char *janus_error_to_string(janus_error error);

/*!
 * \brief The \c JANUS_TRY macro provides a simlpe error handling mechanism.
 */
#define JANUS_TRY(JANUS_API_CALL)                           \
{                                                           \
    janus_error error = (JANUS_API_CALL);                   \
    if (error != JANUS_SUCCESS) {                           \
        printf("Janus error: %s\n\tFile: %s\n\tLine: %d\n", \
               janus_error_to_string(error),                \
               __FILE__,                                    \
               __LINE__);                                   \
        abort();                                            \
    }                                                       \
}

/*!
 * \brief Data buffer type.
 */
typedef uint8_t janus_data;

/*!
 * \brief Data size type.
 */
typedef uint32_t janus_size;

/*!
 * \brief Supported image formats.
 */
typedef enum janus_color_space
{
    JANUS_GRAY8 = 0, /*!< \brief 1 channel grayscale, 8-bit depth. */
    JANUS_BGR24 = 1  /*!< \brief 3 channel color (BGR order), 8-bit depth. */
} janus_color_space;

/*!
 * \brief Common representation for images.
 *
 * \section element_access Element Access
 * Element layout in the janus_image::data buffer with respect to decreasing
 * spatial locality is \a channel, \a column, \a row.
 * Thus an element at channel \c c, column \c x, row \c y, can be
 * retrieved like:
 *
\code
janus_image image = foo();
janus_size xStep = (image.image_format == JANUS_COLOR ? 3 : 1);
janus_size yStep = image.columns * xStep;
janus_size index = y*yStep + x*xStep + c;
janus_data intensity = i.data[index];
\endcode
 */
typedef struct janus_image
{
    janus_data *data;                /*!< \brief Data buffer. */
    janus_size width;                /*!< \brief Column count in pixels. */
    janus_size height;               /*!< \brief Row count in pixels. */
    janus_color_space color_space;   /*!< \brief Arrangement of #data. */
} janus_image;

/*!
 * \brief A measurement made on a #janus_image.
 */
typedef enum janus_attribute
{
    JANUS_INVALID_ATTRIBUTE   = 0, /*!< Catch-all error attribute code */
    JANUS_FRAME               = 1, /*!< Video frame number, 0 for images */

    JANUS_RIGHT_EYE_X         = 32, /*!< Face landmark (pixels) */
    JANUS_RIGHT_EYE_Y         = 33, /*!< Face landmark (pixels) */
    JANUS_LEFT_EYE_X          = 34, /*!< Face landmark (pixels) */
    JANUS_LEFT_EYE_Y          = 35, /*!< Face landmark (pixels) */
    JANUS_NOSE_BASE_X         = 36, /*!< Face landmark (pixels) */
    JANUS_NOSE_BASE_Y         = 37  /*!< Face landmark (pixels) */
} janus_attribute;

/*!
 * \brief The computed value for a #janus_attribute.
 */
typedef float janus_value;

/*!
 * \brief A list of #janus_attribute and #janus_value pairs all belonging to a
 *        the same object in a particular image.
 */
typedef struct janus_attribute_list
{
    janus_size size; /*!< \brief Size of #attributes and #values. */
    janus_attribute *attributes; /*!< \brief Array of #janus_attribute. */
    janus_value *values; /*!< \brief Array of #janus_value. */
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
JANUS_EXPORT void janus_finalize();

/*!
 * \brief Contains the partially-constructed recognition information for an
 *        object.
 */
typedef struct janus_partial_template_type *janus_partial_template;

/*!
 * \brief Contains the completed representation of the recognition information
 *        for an object.
 */
typedef janus_data *janus_template;

/*!
 * \brief The maximum allowed template size is 32 MB.
 */
#define JANUS_MAX_TEMPLATE_SIZE 33554432

/*!
 * \brief Create an empty template for enrollment.
 * \param[in] partial_template Address of the partial template to initialize for
 *                             enrollment.
 * \see janus_add_image janus_add_video janus_finalize_template
 */
JANUS_EXPORT janus_error janus_initialize_template(janus_partial_template *partial_template);

/*!
 * \brief Add information to the template.
 * \param[in] image The image containing the detected object.
 * \param[in] attributes The detected object to recognize.
 * \param[in,out] partial_template The template to contain the object's
 *                                 recognition information.
 * \see janus_initialize_template janus_add_video janus_finalize_template
 */
JANUS_EXPORT janus_error janus_add_image(const janus_image image,
                                         const janus_attribute_list attributes,
                                         janus_partial_template partial_template);

/*!
 * \brief Add information to the template.
 * \param[in] frames An array of frames containing the detected object.
 * \param[in] attributes The detected and tracked object to recognize.
 * \param[in] num_frames Lenth of \em frames and \em attributes.
 * \param[in,out] partial_template The template to contain the object's
 *                                 recognition information.
 * \see janus_initialize_template janus_add_image janus_finalize_template
 */
JANUS_EXPORT janus_error janus_add_video(const janus_image *frames,
                                         const janus_attribute_list *attributes,
                                         const janus_size num_frames,
                                         janus_partial_template partial_template);

/*!
 * \brief Create the final template representation.
 * \param[in,out] partial_template The recognition information to contruct the
 *                                 template from. Deallocated after the template
 *                                 is constructed.
 * \param[out] template_ A pre-allocated buffer no smaller than
 *                       #JANUS_MAX_TEMPLATE_SIZE to contain the final template.
 * \param[out] bytes Size of the buffer actually used to store the template.
 * \see janus_initialize_template janus_augment_template janus_free_template
 */
JANUS_EXPORT janus_error janus_finalize_template(janus_partial_template partial_template,
                                                 janus_template template_,
                                                 janus_size *bytes);

/*!
 * \brief Return a similarity score for two templates.
 * \param[in] a The first template to compare.
 * \param[in] a_bytes Size of template a.
 * \param[in] b The second template to compare.
 * \param[in] b_bytes Size of template b.
 * \param[out] similarity Higher values indicate greater similarity.
 * \see janus_search
 */
JANUS_EXPORT janus_error janus_verify(const janus_template a,
                                      const janus_size a_bytes,
                                      const janus_template b,
                                      const janus_size b_bytes,
                                      float *similarity);

/*!
 * \brief Contains the partially-constructed recognition information for an
 *        object.
 */
typedef struct janus_partial_gallery_type *janus_partial_gallery;

/*!
 * \brief Create an empty gallery for enrollment.
 * \param[in] partial_gallery Address of the partial gallery to initialize for
 *                             enrollment.
 * \see janus_augment_gallery janus_finalize_gallery
 */
JANUS_EXPORT janus_error janus_initialize_gallery(janus_partial_gallery *partial_gallery);

/*!
 * \brief Add information to the gallery.
 * \param[in] partial_template The template information.
 * \param[in,out] partial_gallery The gallery to contain the template.
 * \see janus_initialize_gallery janus_finalize_gallery
 */
JANUS_EXPORT janus_error janus_add_template(const janus_partial_template partial_template,
                                            janus_partial_gallery partial_gallery);

/*!
 * \brief Create the final gallery representation.
 * \param[in,out] partial_gallery The recognition information to contruct the
 *                                 gallery from. Deallocated after the gallery
 *                                 is constructed.
 * \param[out] gallery_file File path to contain the enrolled gallery.
 * \see janus_initialize_gallery janus_add_template
 */
JANUS_EXPORT janus_error janus_finalize_gallery(janus_partial_gallery partial_gallery,
                                                const char *gallery_file);
/*!
 * \brief Ranked search against a gallery for a template.
 * \param [in] template_ Probe to search for.
 * \param [in] gallery_file Gallery to search against.
 * \param [in] num_requested_returns The desired number of returned results.
 * \param [out] template_indicies Indicies of the returned templates.
 * \param [out] num_actual_returns The length of \em template_indicies.
 * \see janus_verify
 */
JANUS_EXPORT janus_error janus_search(const janus_template template_,
                                      const char *gallery_file,
                                      int num_requested_returns,
                                      int *template_indicies,
                                      int *num_actual_returns);

/*!
 * \brief Train a new model from the provided templates.
 * \param[in] partial_templates Training data to generate the model file.
 * \param[in] num_partial_templates Length of \em partial_templates.
 * \param[out] model_file File path to contain the trained model.
 */
JANUS_EXPORT janus_error janus_train(const janus_partial_template *partial_templates,
                                     const int num_partial_templates,
                                     const char *model_file);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_H */
