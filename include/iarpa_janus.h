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

#ifndef IARPA_JANUS_H
#define IARPA_JANUS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>

/*!
 * \mainpage
 * \section overview Overview
 *
 * *libjanus* is a *C++* API for the IARPA Janus program consisting of two header
 * files:
 *
 * Header            | Documentation  | Required               | Description
 * ----------------- | -------------  | ---------------------- | -----------
 * iarpa_janus.h     | \ref janus     | **Yes**                | \copybrief janus
 * iarpa_janus_io.h  | \ref janus_io  | No (Provided)          | \copybrief janus_io
 *
 * - [<b>Source Code</b>](https://github.com/biometrics/janus) [github.com]
 * - [<b>Program Homepage</b>]
 *          (http://www.iarpa.gov/index.php/research-programs/janus) [iarpa.gov]
 *
 * \subsection about About
 * Intelligence analysts often rely on facial images to assist in establishing
 * the identity of an individual, but too often, just examining the sheer volume
 * of possibly relevant images and videos can be daunting. While biometric tools
 * like automated face recognition could assist analysts in this task, current
 * tools perform best on the well-posed, frontal facial photos taken for
 * identification purposes. IARPA’s Janus program aims to dramatically improve
 * the current performance of face recognition tools by fusing the rich spatial,
 * temporal, and contextual information available from the multiple views
 * captured by today’s “media in the wild”. The program will move beyond largely
 * two-dimensional image matching methods used currently into more model-based
 * matching that fuses all views from whatever video and stills are available.
 * Data volume now becomes an integral part of the solution instead of an
 * oppressive burden.
 *
 * The program is seeking to fund rigorous, high-quality research which uses
 * innovative and promising approaches drawn from a variety of fields to develop
 * novel representational models capable of encoding the shape, texture, and
 * dynamics of a face. Instead of relying on a “single best frame approach,”
 * these representations must address the challenges of Aging, Pose,
 * Illumination, and Expression (A-PIE) by exploiting all available imagery.
 * Technologies must support analysts working with partial information by
 * addressing the uncertainties which arise when working with possibly
 * incomplete, erroneous, and ambiguous data. The goal of the program is to test
 * and validate techniques which have the potential to significantly improve the
 * performance of biometric recognition in unconstrained imagery, to that end,
 * the program will involve empirical testing of recognition performance across
 * unconstrained videos, camera stills, and scanned photos exhibiting a broad
 * range of real-world imaging conditions.
 *
 * It is anticipated that successful teams will transcend conventional
 * approaches to biometric recognition by drawing on the multidisciplinary
 * expertise of researchers from the fields of pattern recognition and machine
 * learning; computer vision and image processing; computer graphics and
 * animation; mathematical statistics and modeling; and data visualization and
 * analytics.
 *
 * \subsection license License
 * The API is provided under a [BSD-like license](LICENSE.txt) and is
 * *free for academic and commercial use*.
 *
 * \subsection attribution Attribution
 * This research is based upon work supported by the Office of the Director of
 * National Intelligence (ODNI), Intelligence Advanced Research Projects
 * Activity (IARPA), via the Army Research Laboratory. The views and conclusions
 * contained herein are those of the authors and should not be interpreted as
 * necessarily representing the official policies or endorsements, either
 * expressed or implied, of ODNI, IARPA, or the U.S. Government. The U.S.
 * Government is authorized to reproduce and distribute reprints for
 * Governmental purposes notwithstanding any copyright annotation thereon.
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
#define JANUS_VERSION_MINOR 5
#define JANUS_VERSION_PATCH 1 

/*!
 * \defgroup janus Janus
 * \brief Mandatory interface for Phase 2 of the Janus program.
 *
 * \section Overview
 * A Janus application begins with a call to \ref janus_initialize.
 * Image attributes such as face and landmark locations are found with
 * \ref janus_detect. New templates are constructed with \ref janus_create_template and provided
 * Templates can be freed after use with \ref janus_delete_template. I/O
 * is handled by \ref janus_serialize_template and \ref janus_deserialize_template
 * which serialize templates to byte arrays that can be stored.
 *
 * Templates can be used for verification with \ref janus_verify, or
 * search with \ref janus_search.
 *
 * Galleries are created with \ref janus_create_gallery managed with
 * \ref janus_gallery_insert and \ref janus_gallery_remove and erased
 * with \ref janus_delete_gallery. I/O is handled in the same manner as
 * templates with \ref janus_serialize_gallery and \ref janus_deserialize_gallery.
 *
 * Galleries can be used for search with \ref janus_search.
 *
 * A Janus application ends with a call to \ref janus_finalize.
 *
 * \section thread_safety Thread Safety
 * All functions are marked one of:
 * - \anchor thread_safe \par thread-safe
 *   Can be called simultaneously from multiple threads, even when the
 *   invocations use shared data.
 * - \anchor reentrant \par reentrant
 *   Can be called simultaneously from multiple threads, but only if each
 *   invocation uses its own data.
 * - \anchor thread_unsafe \par thread-unsafe
 *   Can not be called simultaneously from multiple threads.
 *
 * \section implementer_notes Implementer Notes
 * Define \c JANUS_LIBRARY during compilation to export Janus symbols.
 *
 * \addtogroup janus
 * @{
 */

/*!
 * \brief Return type for functions that indicate an error status.
 *
 * All error values are positive integers, with the exception of #JANUS_SUCCESS
 * = 0 which indicates no error.
 */
typedef enum janus_error
{
    JANUS_SUCCESS         = 0, /*!< No error */
    JANUS_UNKNOWN_ERROR      , /*!< Catch-all error code */
    JANUS_OUT_OF_MEMORY      , /*!< Memorry allocation failed */
    JANUS_INVALID_SDK_PATH   , /*!< Incorrect location provided to
                                    #janus_initialize */
    JANUS_OPEN_ERROR         , /*!< Failed to open a file */
    JANUS_READ_ERROR         , /*!< Failed to read from a file */
    JANUS_WRITE_ERROR        , /*!< Failed to write to a file */
    JANUS_PARSE_ERROR        , /*!< Failed to parse file */
    JANUS_INVALID_MEDIA      , /*!< Could not decode media file */
    JANUS_MISSING_TEMPLATE_ID, /*!< Expected a missing template ID */
    JANUS_MISSING_FILE_NAME  , /*!< Expected a missing file name */
    JANUS_NULL_ATTRIBUTES    , /*!< Null #janus_attributes */
    JANUS_MISSING_ATTRIBUTES , /*!< Not all required attributes were
                                    provided */
    JANUS_FAILURE_TO_DETECT  , /*!< Could not localize a face within the
                                    provided image */
    JANUS_FAILURE_TO_ENROLL  , /*!< Could not construct a template from the
                                    provided image and attributes */
    JANUS_FAILURE_TO_SERIALIZE, /*!< Could not serialize a template or
                                     gallery */
    JANUS_FAILURE_TO_DESERIALIZE, /*!< Could not deserialize a template
                                       or gallery */
    JANUS_NOT_IMPLEMENTED    , /*!< Optional functions may return this value in
                                    lieu of a meaninful implementation */
    JANUS_NUM_ERRORS         /*!< Idiom to iterate over all errors */
} janus_error;

/*!
 * \brief Data buffer type.
 */
typedef uint8_t janus_data;

/*!
 * \brief Supported image formats.
 */
typedef enum janus_color_space
{
    JANUS_GRAY8, /*!< \brief 1 channel grayscale, 8-bit depth. */
    JANUS_BGR24  /*!< \brief 3 channel color (BGR order), 8-bit depth. */
} janus_color_space;

/*!
 * \brief Common representation for still images and videos.
 *
 * Pixels are stored in _row-major_ order.
 * In other words, pixel layout with respect to decreasing memory spatial
 * locality is \a channel, \a column, \a row \a frames.
 * Thus pixel intensity can be retrieved as follows:
 *
\code
janus_data get_intensity(janus_image image, size_t channel, size_t column,
                                            size_t row, size_t frame)
{
    const size_t columnStep = (image.color_space == JANUS_BGR24 ? 3 : 1);
    const size_t rowStep    = image.width * columnStep;
    const size_t index      = frame * image.step + row * rowStep
                                                 + column * columnStep
                                                 + channel;
    return image.data[index];
}
\endcode
 *
 * Coordinate (0, 0) corresponds to the top-left corner of the image.
 * Coordinate (width-1, height-1) corresponds to the bottom-right corner of the image.
 */
typedef struct janus_media
{
    std::vector<janus_data*> data; /*! < \brief A collection of image data of size N,
                                                where is the number of frames in a video
                                                or 1 in the case of a still image. */
    size_t width;     /*!< \brief Column count in pixels. */
    size_t height;    /*!< \brief Row count in pixels. */
    size_t step;      /*!< \brief Bytes per frame, including padding. */
    janus_color_space color_space; /*!< \brief Arrangement of #data. */
} janus_media;

/*!
 * \brief Attributes for a particular object in an image.
 *
 * Attributes associated with an object.
 * Values of \c NaN ([isnan](http://www.cplusplus.com/reference/cmath/isnan/))
 * indicate that the attribute value is unknown.
 *
 * \section attribute_definitions Attribute Definitions
 * Attributes collected for images in the Janus program were annotated using
 * crowd sourcing.
 * As a result, attributes are defined in layman's terms, instead of strict
 * scientific definitions.
 * Below are the instructions given to workers for each attribute, which should
 * act as a de-facto definition of the attribute.
 * In some cases instructions evolved slightly over time, reflecting lessons
 * learned from communicating with the workers.
 *
 * As implementations of the Janus API leverage divergent algorithms and data
 * sets, the precise semantics of each attribute may differ by implementation.
 *
 * \subsection face Face
 * [Instructions with images.]
 * (https://s3.amazonaws.com/TurkAnnotator/boundingboxesinstructions.html)
 *
 * We strongly prefer tight bounding boxes over loose ones.
 * There's a bit of leeway here, especially because it is also important to
 * capture the whole head, but if the bounding boxes are too loose we will have
 * to reject the work.
 * For the case of people wearing hats or headgear, or who just have large hair,
 * try to approximate where the head would be (it's alright if you're not
 * exact).
 * As a general rule of thumb, if you can see a feature on a face (an eye, nose,
 * or mouth) or if you can tell it's a forward facing head (but it's too far
 * away to make out individual features), box it!
 * Please err on the side of boxing heads, the exceptions being heads that are
 * completely facing away and heads that are blocked so you cannot see any
 * features.
 * It's okay if boxes overlap!
 * But, try to only box the parts of the head you can see.
 * Non-living faces (portraits, faces on screens, etc.) are fair game, too!
 * If they look realistic, box them.
 * We understand that some images aren't worth your time.
 * In the case of image shown below, where there are many small, blurry heads in
 * the background, just box the person in focus (or press "Not visible" if no
 * one is in focus).
 * In cases like this one, where many faces are distinguishable, please press
 * the "Crowd" button.
 * We ask that you only do this in cases where there truly are crowds - at least
 * 25 heads or so.
 * These images will definitely be reviewed, and wrongly pressing this button
 * will result in a rejection.
 *
 * \subsection right_eye Right Eye
 * For this task, click on the middle of the right eye of the subject shown.
 * Please note that the right eye of the subject will usually appear on the
 * left.
 * Also note that the middle of the eye is not necessarily the pupil,
 * but rather the midpoint between outer edges of the eye.
 * If the right eye is not visible or too blurry to identify, press
 * "Not Visible" - unless the eye is obscured by dark glasses, in which case we
 * ask that you estimate where the eye would be.
 *
 * \subsection left_eye Left Eye
 * For this task, click on the middle of the left eye of the subject shown.
 * Please note that the left eye of the subject will usually appear on the
 * right.
 * Also note that the middle of the eye is not necessarily the pupil,
 * but rather the midpoint between outer edges of the eye.
 * If the left eye is not visible or too blurry to identify, press
 * "Not Visible" - unless the eye is obscured by dark glasses, in which case we
 * ask that you estimate where the eye would be.
 *
 * \subsection nose_base Nose Base
 * For this task, click on the middle of the nose base of the subject shown.
 * This is considered to be the middle of where the nose meets the face.
 * If the nose base is not visible or too blurry to identify, press
 * "Not Visible”.
 *
 * \subsection forehead_visible Forehead Occlusion
 * For this task, choose "Forehead Covered" when:
 * - A scarf, hat, or hair covers at least 25% of the area between eyebrows
 *   and hairline.
 * - The area of the forehead is not contained in the image (because the
 *   edge of the image cuts off this area).
 * Choose "Not Covered" when:
 * - More than 75% of the area between eyebrows and hairline is visible.
 * - Another part of the person's face is blocking part of the forehead
 *   (e.g. the person's face is tilted upwards or to the side).
 * - The image is too blurry to see the forehead region well.
 *
 * \subsection eyes_visible Eye Occlusion
 * For this task, choose "Eyes Covered" when:
 * - Something is in front of either eye. This will often be glasses
 *   (even clear glasses) or hair, but can be anything in the image.
 * - The eye area is not contained in the image.
 * Choose "Not Covered" when:
 * - Both eyes are uncovered (even if they are closed).
 * - Another part of the person's face is blocking either eye
 *   (e.g. the person's face is tilted upwards or turned to the side).
 * - The image is too blurry to see the eye region well
 *
 * \subsection nose_mouth_visible Nose and Mouth Occlusion
 * For this task, choose "Covered" when:
 * - Something is in front of the nose or the mouth, or both.
 *   This is often a microphone or a hand, but can be anything in the image.
 *   If any type of glasses are over the lower part of the nose, this should be
 *   marked as "covered."
 * - The area of the nose and mouth is not contained in the image
 *   (the edge of the image cuts off this area).
 * Choose "Not Covered" when:
 * - Both the nose and mouth are uncovered (facial hair is not considered
 *   'covering').
 * - Another part of the person's face is blocking the nose or mouth
 *   (e.g. the person's face is tilted downwards or turned to the side).
 * - The image is too blurry to see the nose and mouth region well.
 *
 * \subsection indoor Indoor or Outdoor
 * For this task, choose "Indoor" when:
 * - The scene appears to be indoors, and is inside a building or other
 *   covered structure, including any vehicle with a roof.
 * - The scene might be indoors or outdoors, and there seems to be artificial
 *   light present.
 * Choose "Outdoor" when:
 * - The scene appears to be outdoors, and is not under a roof or inside a
 *   building, vehicle or structure.
 * - The scene might be indoors or outdoors, and the photo appears to have
 *   only natural light (sunlight).
 *
 * \subsection age Age
 * For this task, estimate the approximate age of the person shown.
 * The age categories in years and corresponding values in the #janus_metadata
 * , are:
 *
 * | 0 - 19 | 20 - 34 | 35 - 49 | 50-64 | 65+ | Unknown |
 * |--------|---------|---------|-------|-----|---------|
 * | 1      | 2       | 3       | 4     | 5   | 0       |
 *
 * If image quality is low, please take your best guess.
 *
 * \subsection skin_tone Skin Tone
 * Skin tone is generalized into 6 categories described below, along with
 * their corresponding values in the #janus_metadata.
 *
 * | Light Pink | Light Yellow | Medium Pink/Brown | Medium Yellow/Brown | Medium-Dark Brown | Dark Brown |
 * |------------|--------------|-------------------|---------------------|-------------------|------------|
 * | 1          | 2            | 3                 | 4                   | 5                 | 6          |
 *
 * \subsection facial_hair Facial Hair
 * For this task, there are four possible types of facial hair.  Select the facial hair type that is closest to the description below.
 * | No Facial Hair | Moustache | Goatee | Beard |
 * |----------------|-----------|--------|-------|
 * | 0              | 1         | 2      | 3     |
 *
 */
typedef struct janus_attributes
{
    double face_x; /*!< \brief Horizontal offset to top-left corner of face
                               (pixels) \see \ref face. */
    double face_y; /*!< \brief Vertical offset to top-left corner of face
                               (pixels) \see \ref face. */
    double face_width; /*!< \brief Face horizontal size (pixels)
                                   \see \ref face. */
    double face_height; /*!< \brief Face vertical size (pixels)
                                    \see \ref face. */
    double right_eye_x; /*!< \brief Face landmark (pixels)
                                    \see \ref right_eye. */
    double right_eye_y; /*!< \brief Face landmark (pixels)
                                    \see \ref right_eye. */
    double left_eye_x; /*!< \brief Face landmark (pixels) \see \ref left_eye. */
    double left_eye_y; /*!< \brief Face landmark (pixels) \see \ref left_eye. */
    double nose_base_x; /*!< \brief Face landmark (pixels)
                                    \see \ref nose_base. */
    double nose_base_y; /*!< \brief Face landmark (pixels)
                                    \see \ref nose_base. */
    double face_yaw; /*!< \brief Face yaw estimation (degrees). */
    bool forehead_visible; /*!< \brief Visibility of forehead
                                  \see forehead_visible. */
    bool eyes_visible; /*!< \brief Visibility of eyes
                                     \see \ref eyes_visible. */
    bool nose_mouth_visible; /*!< \brief Visibility of nose and mouth
                                    \see nouse_mouth_visible. */
    bool indoor; /*!< \brief Image was captured indoors \see \ref indoor. */

    double frame_number; /*!< \brief Frame number or NAN for images. In the case of videos
                                     the frame number is a 0-based unsigned integer index. */
} janus_attributes;

/*!
 * \brief A list of janus_attributes representing a single identity in a
 *        \ref janus_media instance.
 */
typedef struct janus_track
{
    std::vector<janus_attributes> track;

    double detection_confidence; /*!< \brief A higher value indicates greater
                                             detection confidence. */
    double gender; /*!< \brief Gender of subject of interest, 1 for male, 0 for
                        female. */
    double age; /*!< \brief Approximate age of subject (years) \see \ref age. */
    double skin_tone; /*!< \brief Skin tone of subject \see \ref skin_tone. */

    double frame_rate; /*!< \brief Frames per second, or 0 for images. */
} janus_track;

/*!
 * \brief An association between a piece of media and metadata.
 *
 * All metadata in an association can be assumed to belong to a
 * single subject.
 */
typedef struct janus_association
{
    janus_media media;
    janus_track metadata;
} janus_association;

/*!
 * \brief Call once at the start of the application, before making any other
 * calls to the API.
 *
 * \param[in] sdk_path Path to the \em read-only directory containing the
 *                     janus-compliant SDK as provided by the implementer.
 * \param[in] temp_path Path to an existing empty \em read-write directory for
 *                      use as temporary file storage by the implementation.
 *                      This path is guaranteed until \ref janus_finalize.
 * \param[in] algorithm An empty string indicating the default algorithm, or an
 *                      implementation-defined string indicating an alternative
 *                      configuration.
 * \param[in] gpu_dev The GPU device number to be used by all subsequent
 * 			          implementation function calls
 * \remark This function is \ref thread_unsafe and should only be called once.
 * \see janus_finalize
 */
JANUS_EXPORT janus_error janus_initialize(const std::string &sdk_path,
                                          const std::string &temp_path,
                                          const std::string &algorithm,
                                          const int gpu_dev);

/*!
 * \brief Detect objects in a #janus_media.
 *
 * Each object is represented by a #janus_track. Detected objects
 * can then be used to create #janus_assocation and passed to
 * \ref janus_create_template.
 *
 * \note The number of attributes in a track can never exceed the
 * number of frames in a janus_media instance. Still images should
 * return tracks of length 1.
 *
 * \section face_size Minimum Face Size
 * A minimum size of faces to be detected may be specified by the
 * caller of \ref janus_detect. This size will be given as a pixel
 * value and corresponds to the width of the face.
 *
 * \section detection_guarantees Detection Guarantees
 * The returned tracks will be ordered by decreasing
 * janus_track::detection_confidence.
 *
 * Each of the tracks will have values for at least janus_track::detection_confidence.
 * Each of the janus_attributes within a track will have values for
 *  - janus_attributes::face_x
 *  - janus_attributes::face_y
 *  - janus_attributes::face_width
 *  - janus_attributes::face_height
 *  - janus_attributes::frame_number
 *
 * Any attribute of the track or a janus_attributes within the track
 * without a value will be set to \c NaN.
 *
 * \param[in] media Media to detect objects in.
 * \param[in] min_face_size The minimum width of detected faces that should be returned. The value is in pixels.
 * \param[out] tracks Empty vector to be filled with detected objects.
 * \remark This function is \ref thread_safe.
 */
JANUS_EXPORT janus_error janus_detect(const janus_media &media,
                                      const size_t min_face_size,
                                      std::vector<janus_track> &tracks);

/*!
 * \brief Contains the recognition information for an object.
 */
typedef struct janus_template_type *janus_template;

typedef enum janus_template_role {
    ENROLLMENT_11 = 0,
    VERIFICATION_11 = 1,
    ENROLLMENT_1N = 2,
    IDENTIFICATION = 3,
    CLUSTERING = 4
} janus_template_role;

/*!
 * \brief Build a template from a list of janus_associations
 *
 * All media necessary to build a complete template will be passed in at
 * one time and the constructed template is expected to be suitable for
 * verification and search.
 *
 * \note In the case of an input video it is often the case that a ground truth
 *       location of a subject of interest is specified only in one frame. This
 *       is represented in the API as a track with only one entry provided along
 *       with a media object representing a video. In this case, tracking can be
 *       done by the internal algorithm if they wish to extend the single frame
 *       annotation to subsequent frames. If tracking is done, the single frame
 *       ground truth track should be extended with any additional information
 *       so that the calling function can make use of it. Note also that the returned
 *       track may be sparsely populated (i.e it might skip frames). In all cases
 *       the \ref janus_attributes::frame_number field should accurately represent
 *       the frame the janus_attributes location information originated from.
 *
 * \param[in,out] associations A vector of associations between a piece of media
 *                             and relevant metadata. All of the associations provided
 *                             are guaranteed to be of a single subject. This is not const
 *                             to allow additional tracking if the implementation supports it.
 * \param[in] role An enumeration describing the intended function for the created template.
 *                 Implementors are not required to have different types of templates for any/all
 *                 of the roles specified but can if they choose.
 * \param[out] template_ The template to contain the subject's recognition information.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_create_template(std::vector<janus_association> &associations,
                                               const janus_template_role role,
                                               janus_template &template_);

/*!
 * \brief Build a list of templates from a single piece of janus_media
 *
 * There is no guarantee on the number of individual subjects appearing in the media
 * and implementors should create as many templates as necessary to represent all of
 * the people they find.
 *
 * Implementors must return a single janus_track for each template they create. The track
 * must have all of the metadata fields specified in \ref detection_guarantees.
 *
 * \param[in] media An image or a video containing an unknown number of identities
 * \param[in] role An enumeration describing the intended function for the created template.
 *                 Implementors are not required to have different types of templates for any/all
 *                 of the roles specified but can if they choose.
 * \param[out] templates A list of templates containing recognition information for all of the
 *                       identities discovered in the media.
 * \param[out] tracks A list of metadata corresponding to the return templates.
 */
JANUS_EXPORT janus_error janus_create_template(const janus_media &media,
                                               const janus_template_role role,
                                               std::vector<janus_template> &templates,
                                               std::vector<janus_track> &tracks);

/*!
 * \brief Serialize a template to a stream
 *
 * \param[in] template_ The template to serialize
 * \param[in, out] stream Output stream to store the template.
 * \remark This function is \ref reentrant
 */
JANUS_EXPORT janus_error janus_serialize_template(const janus_template &template_,
                                                  std::ostream &stream);

/*!
 * \brief Load a janus_template from a stream
 *
 * \param[out] template_ An unallocated template to store the loaded data
 * \param[in, out] stream Input stream to deserialize the template from
 * \remark This function is \ref reentrant
 */
JANUS_EXPORT janus_error janus_deserialize_template(janus_template &template_,
                                                    std::istream &stream);

/*!
 * \brief Delete a template
 *
 * Call this function on a template after it is no longer needed.
 *
 * \param[in,out] template_ The template to delete.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_delete_template(janus_template &template_);

/*!
 * \brief Return a similarity score for two templates.
 *
 * Higher scores indicate greater similarity.
 *
 * The returned \p similarity score can be \em asymmetric.
 *
 * \param[in] reference The reference template to compare against. This template was enrolled with the ENROLLMENT_11 template role.
 * \param[in] verification The verification template to compare with the reference. This template was enrolled with the VERIFICATION_11 template role.
 * \param[out] similarity Higher values indicate greater similarity.
 * \remark This function is \ref thread_safe.
 * \see janus_search
 */
JANUS_EXPORT janus_error janus_verify(const janus_template &reference,
                                      const janus_template &verification,
                                      double &similarity);

/*!
 * \brief Unique identifier for a \ref janus_template.
 *
 * Associate a template with a unique identifier during
 * \ref janus_create_gallery.
 * Retrieve the unique identifier from \ref janus_search and \ref janus_cluster.
 */
typedef size_t janus_template_id;

/*!
 * \brief A collection of templates for search
 */
typedef struct janus_gallery_type *janus_gallery;

/*!
 * \brief Create a gallery from a list of templates.
 *
 * The created gallery does not need to be suitable for search. \ref janus_prepare_gallery
 * will be called on this gallery before it used in any search.
 *
 * \param[in] templates List of templates to construct the gallery
 * \param[in] ids list of unique ids to associate with the templates in the gallery
 * \param[out] gallery The created gallery
 * \remark This function is \ref thread_safe
 */
JANUS_EXPORT janus_error janus_create_gallery(const std::vector<janus_template> &templates,
                                              const std::vector<janus_template_id> &ids,
                                              janus_gallery &gallery);

/*!
 * \brief Serialize a gallery to a stream
 *
 * \param[in] gallery The gallery to serialize
 * \param[in, out] stream The output stream to store the serialized gallery
 * \remark This function is \ref reentrant
 */
JANUS_EXPORT janus_error janus_serialize_gallery(const janus_gallery &gallery,
                                                 std::ostream &stream);

/*!
 * \brief Load a janus_gallery from a stream
 *
 * \param[out] gallery An unallocated gallery to store the loaded data
 * \param[in, out] stream The input stream to load data from
 * \remark This function is \ref thread_safe
 */
JANUS_EXPORT janus_error janus_deserialize_gallery(janus_gallery &gallery,
                                                   std::istream &stream);

/*!
 * \brief Prepare a gallery to be searched against.
 *
 * \param[in,out] gallery The gallery to prepare.
 */
JANUS_EXPORT janus_error janus_prepare_gallery(janus_gallery &gallery);

/*!
 * \brief Insert a template into a gallery. After insertion the gallery does
 * not need to be suitable for search.
 *
 * \param[in,out] gallery The gallery to insert the template into
 * \param[in] template_ The template to insert
 * \param[in] id Unique id for the new template
 * \remark This function \ref reentrant
 */
JANUS_EXPORT janus_error janus_gallery_insert(janus_gallery &gallery,
                                              const janus_template &template_,
                                              const janus_template_id id);

/*!
 * \brief Remove a template from a gallery. After removal the gallery does
 * not need to be suitable for search.
 *
 * \param[in,out] gallery The gallery to remove a template from
 * \param[in] id Unique id for the template to delete
 * \remark This function \ref reentrant
 */
JANUS_EXPORT janus_error janus_gallery_remove(janus_gallery &gallery,
                                              const janus_template_id id);

/*!
 * \brief Delete a gallery
 *
 * Call this function on a gallery after it is no longer needed.
 *
 * \param[in,out] gallery The gallery to delete.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_delete_gallery(janus_gallery &gallery);

/*!
 * \brief Ranked search for a template against a gallery.
 *
 * Prior to be passed to search the gallery must be prepared with \ref janus_prepare_gallery.
 *
 * \p template_ids and \p similarities are empty vectors to hold the return
 * scores. The number of returns should be less than or equal to \p num_requested_returns,
 * depending on the contents of the gallery.
 *
 * The returned \p similarities \em may be normalized by the implementation
 * based on the contents of the \p gallery. Therefore, similarity scores
 * returned from searches against different galleries are \em not guaranteed to
 * be comparable.
 *
 * \param[in] probe Probe to search for.
 * \param[in] gallery Gallery to search against.
 * \param[in] num_requested_returns The desired number of returned results.
 * \param[out] template_ids Empty vector to contain the \ref janus_template_id
 *                          of the top matching gallery templates.
 * \param[out] similarities Empty vector to contain the similarity scores of
 *                          the top matching templates.
 * \remark This function is \ref thread_safe.
 * \see janus_verify
 */
JANUS_EXPORT janus_error janus_search(const janus_template &probe,
                                      const janus_gallery &gallery,
                                      const size_t num_requested_returns,
                                      std::vector<janus_template_id> &template_ids,
                                      std::vector<double> &similarities);

/*!
 * \brief Cluster a collection of templates into unique identities.
 *
 * \section clusters Clusters
 * Clusters are represented as a list of <int, double> pairs. Each
 * pairing consists of the cluster id and the cluster confidence for an
 * input template. In general, there should be as many unique cluster IDs
 * as there are unique faces in the input.
 *
 * \section clustering_hint Clustering Hint
 * Clustering is generally considered to be an ill-defined problem, and most
 * algorithms require some help determining the appropriate number of clusters.
 * The \p hint parameter helps influence the number of clusters, though the
 * implementation is free to ignore it.
 * The goal of the hint is to provide an order of magnitude upper bound for the
 * number of identities that appear in a set of media. As such it will be a
 * multiple of 10 (10, 100, 1000 etc.).
 *
 * \note The implementation of this function is optional, and may return
 *       #JANUS_NOT_IMPLEMENTED.
 *
 * \param[in] templates The collection of templates to cluster.
 * \param[in] hint A hint to the clustering algorithm, see \ref clustering_hint.
 * \param[out] clusters A list of cluster pairs, see \ref clusters.
 * \remark This function is \ref thread_safe.
 */
typedef std::pair<int, double> cluster_pair;
JANUS_EXPORT janus_error janus_cluster(const std::vector<janus_template> &templates,
                                       const size_t hint,
                                       std::vector<cluster_pair> &clusters);

/*!
 * \brief Call once at the end of the application, after making all other calls
 * to the API.
 * \remark This function is \ref thread_unsafe and should only be called once.
 * \see janus_initialize
 */
JANUS_EXPORT janus_error janus_finalize();

/*! @}*/

#endif /* IARPA_JANUS_H */
