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

#ifndef JANUS_AUX_H
#define JANUS_AUX_H

#include <janus.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup janus_aux Janus Auxiliary
 * \brief Proposed interface additions for Phases 2 & 3.
 * \addtogroup janus_aux
 *  @{
 */

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
                                           janus_track track);

/*!
 * \brief Free the track and compute the detected objects.
 */
JANUS_EXPORT janus_error janus_finalize_track(janus_track track,
                                              janus_object_list *object_list);

/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* JANUS_AUX_H */
