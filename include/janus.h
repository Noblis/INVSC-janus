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

#define JANUS_VERSION_MAJOR 0
#define JANUS_VERSION_MINOR 1
#define JANUS_VERSION_PATCH 0

/*!
 * \mainpage
 * IARPA Janus Program API
 */

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
 * \code
 * janus_media m = foo();
 * int columnStep = m.channels;
 * int rowStep = m.channels * columnStep;
 * int frameStep = m.rows * rowStep;
 * int index = t*frameStep + y*rowStep + x*columnStep + c;
 * unsigned char intensity = m.data[index];
 * \endcode
 *
 * \section channel_order Channel Order
 * Valid #channels values are 1 or 3.
 * #channels = 1 indicates grayscale.
 * #channels = 3 indicates \c BGR color.
 */
struct janus_media
{
    unsigned char *data;  /**< \brief Data buffer */
    int channels; /**< \brief Channel count, see \ref channel_order. */
    int columns; /**< \brief Column count */
    int rows; /**< \brief Row count */
    int frames; /**< \brief Frame count */
};

JANUS_EXPORT void janus_initialize();

#ifdef __cplusplus
}
#endif

#endif // JANUS_H
