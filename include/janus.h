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
 * - \ref janus "API Specification"
 * - <a href="http://www.iarpa.gov/Programs/sc/Janus/janus.html">Program
 *   Homepage</a> [iarpa.gov]
 */

/*!
 * \defgroup janus Janus
 * \brief \c C API for the IARPA Janus program.
 * \author Joshua C. Klontz (Joshua.Klontz@noblis.org) - Current Maintainer
 *
 * \section license License
 * The API is provided under a <a href="LICENSE.txt">BSD-like license</a> and is
 * free for academic and commercial use.
 *
 * \section header Header
 * \code #include <janus.h> \endcode
 *
 * \section thread_safety Thread Safety
 * All functions are thread-safe unless noted otherwise.
 *
 * \section function_attributes Function Attributes
 * API functions may be marked with the following attributes which indicate
 * restrictions on their usage.
 * \subsection not_thread-safe Not Thread-Safe
 * This function can not be called from multiple threads at the same time.
 * \subsection single-shot Single-Shot
 * Calling this function more than once will result in undefined behaviour.
 * Implies \ref not_thread-safe.
 *
 * \section notes_for_developers Notes for Developers
 * - API return values are generally either \c void or #janus_error.
 * - Output parameters are passed by reference/pointer/address.
 * - Input parameters are passed by value.
 * - Parameters that won't be modified are marked \c const.
 *
 * \section notes_for_implementers Notes for Implementers
 * The following are considered "best practices" for Janus implementations:
 * - Define \c JANUS_LIBRARY during compilation to export Janus symbols and
 *   compile a Unix implementation with \c \-fvisibility=hidden.
 * - Follow the <a href="http://www.pathname.com/fhs/">Filesystem Hierarchy
 *   Standard</a> by organizing the implementation into \c bin, \c include,
 *   \c lib, \c share and \c src sub-folders.
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
    JANUS_SUCCESS             = 0, /*!< No error */
    JANUS_UNKNOWN_ERROR       = 1, /*!< Catch-all error code */
    JANUS_OUT_OF_MEMORY       = 2, /*!< Memorry allocation failed */
    JANUS_INVALID_SDK_PATH    = 3, /*!< Incorrect location provided to
                                     #janus_initialize */
    JANUS_NULL_CONTEXT        = 8, /*!< Value of #janus_context was 0 */
    JANUS_NULL_IMAGE          = 9, /*!< Value of #janus_image was 0 */
    JANUS_NULL_ATTRIBUTE_LIST = 10, /*!< Value of #janus_attribute_list was 0 */
    JANUS_NULL_OBJECT         = 11, /*!< Value of #janus_object was 0 */
    JANUS_NULL_OBJECT_LIST    = 12 /*!< Value of #janus_object_list was 0 */
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
    #define JANUS_TRY(JANUS_API_CALL) (JANUS_API_CALL);
#else
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
 * \brief Common representation for images.
 *
 * \section element_access Element Access
 * Element layout in the janus_image::data buffer with respect to decreasing
 * spatial locality is \a channel, \a column, \a row.
 * Thus an element at channel \c c, column \c x, row \c y, can be
 * retrieved like:
 *
\code
janus_image m = foo();
janus_size columnStep = m.channels;
janus_size rowStep = m.channels * columnStep;
janus_size index = y*rowStep + x*columnStep + c;
janus_data intensity = m.data[index];
\endcode
 *
 * \section channel_order Channel Order
 * Valid janus_image::channels values are 1 or 3.
 * janus_image::channels = 1 indicates grayscale.
 * janus_image::channels = 3 indicates \c BGR color.
 */
typedef struct janus_image_type
{
    janus_data *data;    /*!< \brief Data buffer. */
    janus_size channels; /*!< \brief Channel count. \see \ref channel_order. */
    janus_size width;    /*!< \brief Column count in pixels. */
    janus_size height;   /*!< \brief Row count in pixels. */
} *janus_image;

/*!
 * \brief Returns a #janus_image capable of storing \em channels * \em columns *
 *        \em rows elements in \em data.
 * \param[in] channels Desired value for janus_image::channels.
 * \param[in] width Desired value for janus_image::width.
 * \param[in] height Desired value for janus_image::height.
 * \param[out] image Address to store the allocated image.
 * \note Memory will be allocated, but not initialized, for
 *       janus_image::data.
 * \see janus_free_image
 */
JANUS_EXPORT janus_error janus_allocate_image(const janus_size channels,
                                              const janus_size width,
                                              const janus_size height,
                                              janus_image *image);

/*!
 * \brief Frees the memory previously allocated for a #janus_image.
 * \param[in] image #janus_image to free.
 * \see janus_allocate_image
 */
JANUS_EXPORT void janus_free_image(janus_image image);

/*!
 * \brief A measurement made on a #janus_image.
 */
typedef enum janus_attribute
{
    JANUS_INVALID_ATTRIBUTE = 0, /*!< Catch-all attribute code */

    JANUS_FACE_X          = 16, /*!< Face detection bounding box */
    JANUS_FACE_Y          = 17, /*!< Face detection bounding box */
    JANUS_FACE_WIDTH      = 18, /*!< Face detection bounding box */
    JANUS_FACE_HEIGHT     = 19, /*!< Face detection bounding box */
    JANUS_FACE_CONFIDENCE = 20, /*!< Face detection confidence */
    JANUS_FACE_ROLL       = 21, /*!< Face pose */
    JANUS_FACE_PITCH      = 22, /*!< Face pose */
    JANUS_FACE_YAW        = 23, /*!< Face pose */

    JANUS_RIGHT_EYE_X         = 32, /*!< Face landmark */
    JANUS_RIGHT_EYE_Y         = 33, /*!< Face landmark */
    JANUS_LEFT_EYE_X          = 34, /*!< Face landmark */
    JANUS_LEFT_EYE_Y          = 35, /*!< Face landmark */
    JANUS_NOSE_BASE_X         = 36, /*!< Face landmark */
    JANUS_NOSE_BASE_Y         = 37, /*!< Face landmark */
    JANUS_NOSE_BRIDGE_X       = 38, /*!< Face landmark */
    JANUS_NOSE_BRIDGE_Y       = 39, /*!< Face landmark */
    JANUS_EYE_NOSE_X          = 40, /*!< Face landmark */
    JANUS_EYE_NOSE_Y          = 41, /*!< Face landmark */
    JANUS_RIGHT_UPPER_CHEEK_X = 42, /*!< Face landmark */
    JANUS_RIGHT_UPPER_CHEEK_Y = 43, /*!< Face landmark */
    JANUS_RIGHT_LOWER_CHEEK_X = 44, /*!< Face landmark */
    JANUS_RIGHT_LOWER_CHEEK_Y = 45, /*!< Face landmark */
    JANUS_LEFT_UPPER_CHEEK_X  = 46, /*!< Face landmark */
    JANUS_LEFT_UPPER_CHEEK_Y  = 47, /*!< Face landmark */
    JANUS_LEFT_LOWER_CHEEK_X  = 48, /*!< Face landmark */
    JANUS_LEFT_LOWER_CHEEK_Y  = 49  /*!< Face landmark */
} janus_attribute;

/*!
 * \brief The computed value for a #janus_attribute.
 */
typedef float janus_value;

/*!
 * \brief A list of #janus_attribute and #janus_value pairs all belonging to a
 *        the same #janus_object in a particular #janus_image.
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
 * \see janus_free_attribute_list
 */
JANUS_EXPORT janus_error janus_allocate_attribute_list(const janus_size size,
                                          janus_attribute_list *attribute_list);

/*!
 * \brief Frees the memory previously allocated for the attribute list.
 * \param[in] attribute_list #janus_attribute_list to free.
 * \see janus_allocate_object
 */
JANUS_EXPORT void janus_free_attribute_list(
                                           janus_attribute_list attribute_list);

/*!
 * \brief A collection of #janus_attribute_list all associated with the same
 *        object.
 */
typedef struct janus_object_type
{
    janus_size size; /*!< \brief Size of #attribute_lists. */
    janus_attribute_list *attribute_lists; /*!< \brief Array of
                                                       #janus_attribute_list. */
} *janus_object;

/*!
 * \brief Allocates memory for a #janus_object capable of storing \em size
 *        attribute lists.
 * \param[in] size Desired value for janus_object::size.
 * \param[out] object Address to store the allocated object.
 * \note Memory will be allocated, but not initialized, for
 *       janus_object::attribute_lists.
 * \see janus_free_object
 */
JANUS_EXPORT janus_error janus_allocate_object(const janus_size size,
                                               janus_object *object);

/*!
 * \brief Frees the memory previously allocated for the object.
 * \param[in] object #janus_object to free.
 * \note #janus_free_attribute_list will be called for each attribute list in
 *       #janus_object::attribute_lists. If this behavior is undesired, set
 *       #janus_object::size to 0 before calling this function or set individual
 *       elements in #janus_object::attribute_lists to \c NULL.
 * \see janus_allocate_object
 */
JANUS_EXPORT void janus_free_object(janus_object object);

/*!
 * \brief A list of #janus_object.
 */
typedef struct janus_object_list_type
{
    janus_size size; /*!< \brief Number of elements in #objects. */
    janus_object *objects; /*!< \brief Array of #janus_object. */
} *janus_object_list;

/*!
 * \brief Allocates memory for a #janus_object_list capable of storing \em size
 *        #janus_object.
 * \param[in] size Desired value for janus_object_list::size.
 * \param[out] object_list Address to store the allocated object list.
 * \note Memory will be allocated, but not initialized, for
 *       janus_object_list::objects.
 * \see janus_free_object_list
 */
JANUS_EXPORT janus_error janus_allocate_object_list(const janus_size size,
                                                janus_object_list *object_list);

/*!
 * \brief Frees the memory previously allocated for the object list.
 * \param[in] object_list #janus_object_list to free.
 * \note #janus_free_object will be called for each object in
 *       #janus_object_list::objects. If this behavior is undesired, set
 *       #janus_object_list::size to 0 before calling this function or set
 *       individual elements in #janus_object_list::objects to \c NULL;
 * \see janus_allocate_object_list
 */
JANUS_EXPORT void janus_free_object_list(janus_object_list object_list);

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
 * \brief Detect objects in a #janus_image.
 * \see janus_free_object_list
 */
JANUS_EXPORT janus_error janus_detect(const janus_context context,
                                      const janus_image image,
                                      janus_object_list *object_list);

/*!
 * \brief Contains tracking information for objects in a video.
 */
typedef struct janus_track_type *janus_track;

/*!
 * \brief Create a new track.
 */
JANUS_EXPORT janus_error janus_initialize_track(janus_track *track);

/*!
 * \brief Add a frame to the track.
 */
JANUS_EXPORT janus_error janus_track_frame(const janus_context context,
                                           const janus_image frame,
                                           janus_track *track);

/*!
 * \brief Free the track and compute the detected objects.
 */
JANUS_EXPORT janus_error janus_finalize_track(janus_track track,
                                              janus_object_list *object_list);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_H */
