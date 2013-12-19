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
 * \brief Image and video decoding interface.
 * \addtogroup janus_io
 *  @{
 */

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
 * \brief High-level function for enrolling a template from a metadata file.
 * \param [in] metadata_file Path to the \c csv metadata file.
 * \param [in,out] template_ Constructed template with preallocated buffer.
 * \param [out] bytes Size of template_.
 */
JANUS_EXPORT janus_error janus_enroll_template(const char *metadata_file, janus_template template_, size_t *bytes);

/*!
 * \brief High-level function for enrolling a gallery from a metadata file.
 * \param [in] metadata_file Path to the \c csv metadata file.
 * \param [in] gallery_file File to save the gallery to.
 */
JANUS_EXPORT janus_error janus_enroll_gallery(const char *metadata_file, const char *gallery_file);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_IO_H */
