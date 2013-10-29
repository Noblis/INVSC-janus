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
 *
 * \page more_information More Information
 * \brief Additional technical considerations.
 *
 * \section api_conventions API Conventions
 * \subsection thread_safety Thread Safety
 * All functions are thread-safe unless noted otherwise.
 *
 * \subsection function_attributes Function Attributes
 * API functions may be marked with the following attributes which indicate
 * restrictions on their usage.
 *
 * \subsubsection not_thread-safe Not Thread-Safe
 * This function can not be called from multiple threads at the same time.
 *
 * \subsubsection single-shot Single-Shot
 * Calling this function more than once will result in undefined behaviour.
 * Implies \ref not_thread-safe.
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
    JANUS_SUCCESS             = 0,  /*!< No error */
    JANUS_UNKNOWN_ERROR       = 1,  /*!< Catch-all error code */
    JANUS_OUT_OF_MEMORY       = 2,  /*!< Memorry allocation failed */
    JANUS_INVALID_SDK_PATH    = 3,  /*!< Incorrect location provided to
                                         #janus_initialize */
    JANUS_INVALID_IMAGE       = 4,  /*!< Could not decode image file */
    JANUS_INVALID_VIDEO       = 5,  /*!< Could not decode video file */
    JANUS_NULL_CONTEXT        = 8,  /*!< Value of #janus_context was 0 */
    JANUS_NULL_VALUE          = 10, /*!< Value of #janus_value was 0 */
    JANUS_NULL_ATTRIBUTE_LIST = 11  /*!< Value of #janus_attribute_list was 0 */
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
#ifdef JANUS_DISABLE_TRY
#  define JANUS_TRY(JANUS_API_CALL) (JANUS_API_CALL);
#else
#  define JANUS_TRY(JANUS_API_CALL)                            \
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
#endif

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
    JANUS_MEDIA_ID            = 1, /*!< Unique integer identifier for source
                                        image or video file */
    JANUS_FRAME               = 2, /*!< Video frame number, -1 for images */
    JANUS_TRACKING_CONFIDENCE = 3, /*!< Object tracking confidence, -1 for
                                        images */

    JANUS_FACE_X          = 16, /*!< Face detection bounding box (pixels) */
    JANUS_FACE_Y          = 17, /*!< Face detection bounding box (pixels) */
    JANUS_FACE_WIDTH      = 18, /*!< Face detection bounding box (pixels) */
    JANUS_FACE_HEIGHT     = 19, /*!< Face detection bounding box (pixels) */
    JANUS_FACE_CONFIDENCE = 20, /*!< Face detection confidence */
    JANUS_FACE_ROLL       = 21, /*!< Face pose (degrees) */
    JANUS_FACE_PITCH      = 22, /*!< Face pose (degrees) */
    JANUS_FACE_YAW        = 23, /*!< Face pose (degrees) */

    JANUS_RIGHT_EYE_X         = 32, /*!< Face landmark (pixels) */
    JANUS_RIGHT_EYE_Y         = 33, /*!< Face landmark (pixels) */
    JANUS_LEFT_EYE_X          = 34, /*!< Face landmark (pixels) */
    JANUS_LEFT_EYE_Y          = 35, /*!< Face landmark (pixels) */
    JANUS_NOSE_BASE_X         = 36, /*!< Face landmark (pixels) */
    JANUS_NOSE_BASE_Y         = 37, /*!< Face landmark (pixels) */
    JANUS_NOSE_BRIDGE_X       = 38, /*!< Face landmark (pixels) */
    JANUS_NOSE_BRIDGE_Y       = 39, /*!< Face landmark (pixels) */
    JANUS_EYE_NOSE_X          = 40, /*!< Face landmark (pixels) */
    JANUS_EYE_NOSE_Y          = 41, /*!< Face landmark (pixels) */
    JANUS_RIGHT_UPPER_CHEEK_X = 42, /*!< Face landmark (pixels) */
    JANUS_RIGHT_UPPER_CHEEK_Y = 43, /*!< Face landmark (pixels) */
    JANUS_RIGHT_LOWER_CHEEK_X = 44, /*!< Face landmark (pixels) */
    JANUS_RIGHT_LOWER_CHEEK_Y = 45, /*!< Face landmark (pixels) */
    JANUS_LEFT_UPPER_CHEEK_X  = 46, /*!< Face landmark (pixels) */
    JANUS_LEFT_UPPER_CHEEK_Y  = 47, /*!< Face landmark (pixels) */
    JANUS_LEFT_LOWER_CHEEK_X  = 48, /*!< Face landmark (pixels) */
    JANUS_LEFT_LOWER_CHEEK_Y  = 49  /*!< Face landmark (pixels) */
} janus_attribute;

/*!
 * \brief The computed value for a #janus_attribute.
 */
typedef float janus_value;

/*!
 * \brief A list of #janus_attribute and #janus_value pairs all belonging to a
 *        the same object in a particular image.
 */
typedef struct janus_attribute_list_type
{
    janus_size size; /*!< \brief Size of #attributes and #values. */
    janus_attribute *attributes; /*!< \brief Array of #janus_attribute. */
    janus_value *values; /*!< \brief Array of #janus_value. */
} *janus_attribute_list;

/*!
 * \brief Allocates memory for a #janus_attribute_list capable of storing \em
 *        size attributes.
 * \param[in] size Desired value for janus_attribute_list::size.
 * \param[out] attribute_list Address to store the allocated attribute list.
 * \note Memory will be allocated, but not initialized, for
 *       janus_attribute_list::attributes and janus_attribute_list::values.
 * \see janus_copy_attribute_list janus_free_attribute_list
 */
JANUS_EXPORT janus_error janus_allocate_attribute_list(const janus_size size,
                                          janus_attribute_list *attribute_list);

/*!
 * \brief Create a copy of an attribute list.
 * \param[in] input Attribute list to copy.
 * \param[out] output Address to store the copied attribute list.
 * \see janus_allocate_attribute_list janus_free_attribute_list
 */
JANUS_EXPORT janus_error janus_copy_attribute_list(const janus_attribute_list input,
                                                   janus_attribute_list *output);

/*!
 * \brief Frees the memory previously allocated for the attribute list.
 * \param[in] attribute_list #janus_attribute_list to free.
 * \see janus_allocate_attribute_list janus_copy_attribute_list
 */
JANUS_EXPORT void janus_free_attribute_list(janus_attribute_list attribute_list);

/*!
 * \brief Retrieve the value for an attribute in an attribute list.
 * \param[in] attribute_list The attribute list to search.
 * \param[in] attribute The attribute to search for.
 * \param[out] value The value for the requested attribute.
 */
JANUS_EXPORT janus_error janus_get_value(const janus_attribute_list attribute_list,
                                         const janus_attribute attribute,
                                         janus_value *value);

/*!
 * \brief Call once at the start of the application, before making any other
 * calls to the API.
 *
 * \param[in] sdk_path Path to the \em read-only directory containing the
 *                     janus-compliant SDK as provided by the implementer.
 * \note \ref single-shot
 * \see janus_finalize
 */
JANUS_EXPORT janus_error janus_initialize(const char *sdk_path);

/*!
 * \brief Call once at the end of the application, after making all other calls
 * to the API.
 * \note \ref single-shot
 * \see janus_initialize
 */
JANUS_EXPORT void janus_finalize();

/*!
 * \brief Resources associated with the thread of execution.
 */
typedef struct janus_context_type *janus_context;

/*!
 * \brief Create a new context.
 * A context is used to store per-thread resources.
 * \note \ref not_thread-safe
 * \see janus_finalize_context
 */
JANUS_EXPORT janus_error janus_initialize_context(janus_context *context);

/*!
 * \brief Release the memory previously initialized for a context.
 * \see janus_initialize_context
 */
JANUS_EXPORT void janus_finalize_context(janus_context context);

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
 * \see janus_augment_template janus_finalize_template
 */
JANUS_EXPORT janus_error janus_initialize_template(janus_partial_template *partial_template);

/*!
 * \brief Add information to the template.
 * \param[in] attributes The detected object to recognize.
 * \param[in] image The image containing the detected object.
 * \param[in,out] partial_template The template to contain the object's
 *                                 recognition information.
 * \see janus_initialize_template janus_finalize_template
 */
JANUS_EXPORT janus_error janus_augment_template(const janus_attribute_list attributes,
                                                const janus_image image,
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
 */
JANUS_EXPORT janus_error janus_verify(const janus_template a,
                                      const janus_size a_bytes,
                                      const janus_template b,
                                      const janus_size b_bytes,
                                      float *similarity);

/*!
 * \brief A gallery.
 */
typedef janus_data *janus_gallery;

/*!
 * \brief Create a gallery from an array of templates.
 * \param[in] templates Array of templates to construct the gallery from.
 * \param[in] template_sizes Array containing the size of each template.
 * \param[in] num_templates Length of templates and template_sizes.
 * \param[out] gallery A pre-allocated buffer no smaller than num_templates *
 *             #JANUS_MAX_TEMPLATE_SIZE to contain the final gallery.
 * \param[out] gallery_size Bytes of the gallery buffer actually used.
 */
JANUS_EXPORT janus_error janus_create_gallery(const janus_template *templates,
                                              const janus_size *template_sizes,
                                              const janus_size num_templates,
                                              janus_gallery gallery,
                                              janus_size *gallery_size);

/*!
 * \brief Compare a template against a gallery.
 * \param[in] probe The template to compare against the gallery.
 * \param[in] probe_size Length of probe in bytes.
 * \param[in] gallery The gallery to compare the probe against.
 * \param[in] gallery_size Length of gallery in bytes.
 * \param[out] similarities A pre-allocated buffer no smaller than the number of templates
 *             in the gallery.
 */
JANUS_EXPORT janus_error janus_search(const janus_template probe,
                                      const janus_size probe_size,
                                      const janus_gallery gallery,
                                      const janus_size gallery_size,
                                      float *similarities);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_H */
