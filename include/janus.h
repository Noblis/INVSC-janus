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
 * \section suggestions_for_implementers Suggestions for Implementers
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
    JANUS_SUCCESS          = 0, /*!< No error */
    JANUS_UNKNOWN_ERROR    = 1, /*!< Catch-all error code */
    JANUS_INVALID_SDK_PATH = 2  /*!< Incorrect location provided to
                                     #janus_initialize */
} janus_error;

/*!
 * \brief Data buffer type.
 */
typedef uint8_t janus_data;

/*!
 * \brief Data size type.
 */
typedef uint32_t janus_size;

/*!
 * \brief Common representation for images and videos.
 *
 * An \a image is a janus_media with #frames = 1.
 * A \a video is a janus_media with #frames > 1.
 * This API generally does not distinguish between images and videos, and refers
 * to both collectively as \a media.
 *
 * \section element_access Element Access
 * Element layout in the #data buffer with respect to decreasing spatial
 * locality is \a channel, \a column, \a row, \a frame.
 * Thus an element at channel \c c, column \c x, row \c y, and frame \c t can be
 * retrieved like:
 *
\code
janus_media m = foo();
janus_size columnStep = m.channels;
janus_size rowStep = m.channels * columnStep;
janus_size frameStep = m.rows * rowStep;
janus_size index = t*frameStep + y*rowStep + x*columnStep + c;
janus_data intensity = m.data[index];
\endcode
 *
 * \section channel_order Channel Order
 * Valid #channels values are 1 or 3.
 * #channels = 1 indicates grayscale.
 * #channels = 3 indicates \c BGR color.
 */
typedef struct janus_media
{
    janus_data *data;    /*!< \brief Data buffer. */
    janus_size channels; /*!< \brief Channel count. \see \ref channel_order. */
    janus_size columns;  /*!< \brief Column count. */
    janus_size rows;     /*!< \brief Row count. */
    janus_size frames;   /*!< \brief Frame count. */
} janus_media;

/*!
 * A measurement made on a #janus_media.
 */
typedef enum janus_attribute
{
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
    JANUS_RIGHT_UPPER_CHEEK_X = 40, /*!< Face landmark */
    JANUS_RIGHT_UPPER_CHEEK_Y = 41, /*!< Face landmark */
    JANUS_RIGHT_LOWER_CHEEK_X = 42, /*!< Face landmark */
    JANUS_RIGHT_LOWER_CHEEK_Y = 43, /*!< Face landmark */
    JANUS_LEFT_UPPER_CHEEK_X  = 44, /*!< Face landmark */
    JANUS_LEFT_UPPER_CHEEK_Y  = 45, /*!< Face landmark */
    JANUS_LEFT_LOWER_CHEEK_X  = 46, /*!< Face landmark */
    JANUS_LEFT_LOWER_CHEEK_Y  = 47  /*!< Face landmark */
} janus_attribute;

/*!
 * \brief The computed value for a #janus_attribute.
 */
typedef float janus_value;

/*!
 * \brief A list of associated #janus_attribute and #janus_value pairs.
 */
typedef struct janus_object
{
    janus_size size; /*!< \brief Size of #attributes and #values. */
    janus_attribute *attributes; /*!< \brief Array of #janus_attribute. */
    janus_value *values; /*!< \brief Array of #janus_value. */
} janus_object;

/*!
 * \brief A list of #janus_object.
 */
typedef struct janus_object_list
{
    janus_size size; /*!< \brief Number of elements in #objects. */
    janus_object *objects; /*!< \brief Array of #janus_object. */
} janus_object_list;

/*!
 * \brief Contains the extracted representation of a subject.
 *
 * Computed during enrollment and used for comparison.
 */
typedef struct janum_template_impl *janus_template;

/*!
 * \brief Call once at the start of the application, before making any other
 * calls to the API.
 *
 * \param sdk_path Path to the \em read-only directory containing the
 *                 janus-compliant SDK as provided by the implementer.
 * \returns #JANUS_SUCCESS, #JANUS_INVALID_SDK_PATH, or another #janus_error.
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
 * \brief Detect objects in a #janus_media.
 */
JANUS_EXPORT janus_object_list janus_detect(const janus_media *media);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_H */
