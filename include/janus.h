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

// Standard idiom for exporting symbols, define JANUS_LIBRARY when compiling a
// Janus compliant shared library.
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

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup janus_api Janus API
 * \brief C API for the IARPA Janus program
 *
 * \code #include <janus.h> \endcode
 */

/*!
 * \addtogroup janus_api
 *  @{
 */

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
 * \anchor error_type
 * \brief Return type for functions that indicate error status.
 * \see \ref error_codes "Error Codes"
 */
typedef int32_t janus_error;

///@{
/*!
 * \anchor error_codes
 * \name Error Codes
 * All error values are negative, with the exception of \c JANUS_SUCCESS which
 * indicates no errors.
 * Values in the inclusive interval \f$\left[-2^{16},-2^{31}\right]\f$ are
 * reserved for performer use.
 * \see \ref error_type "Error Type"
 */
#define JANUS_SUCCESS 0
#define JANUS_ERROR -1
#define JANUS_INVALID_SDK_PATH -2
///@}

/*!
 * \mainpage
 * IARPA Janus Program API
 */

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
struct janus_media
{
    janus_data *data;  /**< \brief Data buffer */
    janus_size channels; /**< \brief Channel count, see \ref channel_order. */
    janus_size columns; /**< \brief Column count */
    janus_size rows; /**< \brief Row count */
    janus_size frames; /**< \brief Frame count */
};

/*!
 * \brief Call once before making other API calls.
 *
 * \param sdk_path Path to the root of the janus-compliant SDK.
 *
 */
JANUS_EXPORT janus_error janus_initialize(const char *sdk_path);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif // JANUS_H
