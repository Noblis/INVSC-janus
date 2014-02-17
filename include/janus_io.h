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
Template_ID        , File_Name, Frame, <janus_attribute>, <janus_attribute>, ..., <janus_attribute>
<janus_template_id>, <string> , <int>, <double>         , <double>         , ..., <double>
<janus_template_id>, <string> , <int>, <double>         , <double>         , ..., <double>
...
<janus_template_id>, <string> , <int>, <double>         , <double>         , ..., <double>
\endverbatim
 *
 * Where:
 * - [Template_ID](\ref janus_template_id) is a unique integer identifier indicating rows that belong to the same template.
 * - \c File_Name is a path to the image or video file on disk.
 * - \c Frame is the video frame number and -1 (or empty string) for still images.
 *
 * Metadata files should adhere to the following "sane" conventions:
 * - All rows associated with the same \c Template_ID should occur sequentially.
 * - All rows associated with the same \c Template_ID and \c File_Name should occur sequentially ordered by \c Frame.
 * - A cell should be empty when no value is available for the specified #janus_attribute.
 *
 * \par Examples:
 * - [Kirchner.csv](https://raw.github.com/biometrics/janus/master/data/Kirchner.csv)
 * - [Toledo.csv](https://raw.github.com/biometrics/janus/master/data/Toledo.csv)
 */
typedef const char *janus_metadata;

/*!
 * \brief High-level function for enrolling a template from a metadata file.
 * \param [in] metadata #janus_metadata file to enroll.
 * \param [in,out] flat_template Constructed template with preallocated buffer.
 * \param [out] bytes Size of flat_template.
 */
JANUS_EXPORT janus_error janus_create_template(janus_metadata metadata, janus_flat_template flat_template, size_t *bytes);

/*!
 * \brief High-level function for enrolling a gallery from a metadata file.
 * \param [in] metadata #janus_metadata to enroll.
 * \param [in] gallery File to save the gallery to.
 */
JANUS_EXPORT janus_error janus_create_gallery(janus_metadata metadata, janus_gallery gallery);

/*!
 * \brief High-level function to create a similarity matrix.
 */
JANUS_EXPORT janus_error janus_create_simmat(janus_metadata gallery_metadata,
                                             janus_metadata probe_metadata,
                                             janus_gallery gallery,
                                             const char *simmat_file);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_IO_H */
