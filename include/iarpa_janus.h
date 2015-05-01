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

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \mainpage
 * \section overview Overview
 *
 * *libjanus* is a *C* API for the IARPA Janus program consisting of three
 * header files:
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
 * \subsection atribution Atribution
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
#define JANUS_VERSION_MINOR 4
#define JANUS_VERSION_PATCH 0

/*!
 * \defgroup janus Janus
 * \brief Mandatory interface for Phase 2.
 *
 * All Janus performers should adhere to this interface.
 *
 * \section Overview
 * A Janus application begins with a call to \ref janus_initialize. New
 * templates are constructed with \ref janus_allocate_template and provided
 * image data with \ref janus_detect followed by \ref janus_augment. Templates
 * are freed after use with \ref janus_free_template.
 *
 * Templates can be used for verification with \ref janus_flatten_template and
 * \ref janus_verify, or search with \ref janus_enroll,
 * \ref janus_flatten_gallery and \ref janus_search.
 *
 * All Janus applications end with a call to \ref janus_finalize.
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
 * - Define \c JANUS_LIBRARY during compilation to export Janus symbols and
 *   compile a Unix implementation with \c \-fvisibility=hidden.
 * - Follow the <a href="http://www.pathname.com/fhs/">Filesystem Hierarchy
 *   Standard</a> by organizing the implementation into \c bin, \c include,
 *   \c lib, \c share and \c src sub-folders.
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
    JANUS_INVALID_IMAGE      , /*!< Could not decode image file */
    JANUS_INVALID_VIDEO      , /*!< Could not decode video file */
    JANUS_MISSING_TEMPLATE_ID, /*!< Expected a missing template ID */
    JANUS_MISSING_FILE_NAME  , /*!< Expected a missing file name */
    JANUS_NULL_ATTRIBUTES    , /*!< Null #janus_attributes */
    JANUS_MISSING_ATTRIBUTES , /*!< Not all required attributes were
                                    provided */
    JANUS_FAILURE_TO_ENROLL  , /*!< Could not construct a template from the
                                    provided image and attributes */
    JANUS_NOT_IMPLEMENTED    , /*!< Optional functions may return this value in
                                    lieu of a meaninful implementation */
    JANUS_NUM_ERRORS           /*!< Idiom to iterate over all errors */
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
 * \brief Common representation for still images and video frames.
 *
 * Pixels are stored \em continuously in the janus_image::data buffer.
 * Pixel layout with respect to decreasing spatial locality is \a channel,
 * \a column, \a row.
 * Thus pixel intensity can be retrieved as follows:
 *
\code
janus_data get_intensity(janus_image image, size_t channel, size_t column,
                                                                     size_t row)
{
    const size_t columnStep = (image.image_format == JANUS_COLOR ? 3 : 1);
    const size_t rowStep = image.columns * columnStep;
    const size_t index = row*rowStep + column*columnStep + channel;
    return image.data[index];
}
\endcode
 *
 * (0, 0) corresponds to the top-left corner of the image.
 * (width-1, height-1) corresponds to the bottom-right corner of the image.
 */
typedef struct janus_image
{
    janus_data *data;              /*!< \brief Data buffer. */
    size_t width;                  /*!< \brief Column count in pixels. */
    size_t height;                 /*!< \brief Row count in pixels. */
    janus_color_space color_space; /*!< \brief Arrangement of #data. */
} janus_image;

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
 * | 1      | 2       | 3       | 4     | 5   | 0       |
 *
 * If image quality is low, please take your best guess.
 *
 * \subsection skin_tone Skin Tone
 * Skin tone is generalized into 6 categories described below, along with
 * their corresponding values in the #janus_metadata.
 *
 * | Light Pink-toned | Light Yellow-toned | Medium Pink/Brown-toned | Medium Yellow/Brown-toned | Medium-Dark Brown | Dark Brown |
 * | 1                | 2                  | 3                       | 4                         | 5                 | 6          |
 */
typedef struct janus_attributes
{
    double detection_confidence; /*!< \brief A higher value indicates greater
                                             detection confidence. */
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
    double forehead_visible; /*!< \brief Visibility of forehead
                                  \see forehead_visible. */
    double eyes_visible; /*!< \brief Visibility of eyes
                                     \see \ref eyes_visible. */
    double nose_mouth_visible; /*!< \brief Visibility of nose and mouth
                                    \see nouse_mouth_visible. */
    double indoor; /*!< \brief Image was captured indoors \see \ref indoor. */
    double gender; /*!< \brief Gender of subject of interest, 1 for male, 0 for
                        female. */
    double age; /*!< \brief Approximate age of subject (years) \see \ref age. */
    double skin_tone; /*!< \brief Skin tone of subject \see \ref skin_tone. */
    double frame_rate; /*!< \brief Frames per second, or 0 for images. */
} janus_attributes;

/*!
 * \brief Detect objects in a #janus_image.
 *
 * Each object is represented by a #janus_attributes. In the case that the
 * number of detected objects is greater than \p num_requested, the
 * implementation may choose which detections to exclude, potentially returning
 * early before scanning the entire image. Detected objects can then be used in
 * \ref janus_augment.
 *
 * \section detection_guarantees Detection Guarantees
 * The first \p num_actual elements of \p attributes_array will be populated by
 * decreasing janus_attributes::detection_confidence.
 *
 * Each of the \p num_actual detections will have values for at least the
 * following attributes:
 *  - janus_attributes::detection_confidence
 *  - janus_attributes::face_x
 *  - janus_attributes::face_y
 *  - janus_attributes::face_width
 *  - janus_attributes::face_height
 *
 * Any attribute of the \p num_actual detections without a value will be set to
 * \c NaN.
 *
 * \param[in] image Image to detect objects in.
 * \param[out] attributes_array Pre-allocated array of uninitialized
 *                              #janus_attributes. Expected to be at least
 *                              \p num_requested * \c sizeof(#janus_attributes)
 *                              bytes long.
 * \param[in] num_requested Length of \p attributes_array.
 * \param[out] num_actual Length of \p attributes_array actually populated with
 *                        detected objects by the implementation.
 * \remark This function is \ref thread_safe.
 */
JANUS_EXPORT janus_error janus_detect(const janus_image image,
                                      janus_attributes *attributes_array,
                                      const size_t num_requested,
                                      size_t *num_actual);

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
 * \remark This function is \ref thread_unsafe and should only be called once.
 * \see janus_finalize
 */
JANUS_EXPORT janus_error janus_initialize(const char *sdk_path,
                                          const char *temp_path,
                                          const char *algorithm);

/*!
 * \brief Call once at the end of the application, after making all other calls
 * to the API.
 * \remark This function is \ref thread_unsafe and should only be called once.
 * \see janus_initialize
 */
JANUS_EXPORT janus_error janus_finalize();

/*!
 * \brief Contains the recognition information for an object.
 *
 * Create a new template with \ref janus_allocate_template.
 * Add images and videos to the template using \ref janus_augment.
 * Finalize the template for comparison with \ref janus_flatten_template.
 * \see janus_flat_template
 */
typedef struct janus_template_type *janus_template;

/*!
 * \brief Allocate memory for an empty template.
 *
 * Memory is managed by the implementation and guaranteed until
 * \ref janus_free_template.
 *
 * Add images to the template with \ref janus_augment.
 *
 * \code
 * janus_template template_;
 * janus_error error = janus_allocate_template(&template_);
 * assert(!error);
 * \endcode
 *
 * \param[in] template_ An uninitialized template.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_allocate_template(janus_template *template_);

/*!
 * \brief Add an image to the template.
 *
 * The \p attributes should be provided from a prior call to \ref janus_detect.
 * As a special case, if janus_attributes::frame_rate is greater than zero, the
 * \p image should be treated as the first frame in a video sequence. Subsequent
 * calls to this function may then pass \c NULL for \p attributes, in which case
 * the implementation should use the \p attributes from a previous call to this
 * function as the seed for tracking, or as a prior for localizing the object
 * in the current frame. In this way, \ref janus_detect need only be called once
 * for videos, at the start of the frame sequence.
 *
 * This function may write to \p attributes, reflecting additional information
 * gained during augmentation.
 *
 * Augmented templates can then be passed to \ref janus_flatten_template for
 * verification or \ref janus_enroll for gallery construction.
 *
 * \param[in] image The image containing the detected object to be recognized.
 * \param[in,out] attributes Location and metadata associated with a single
 *                          detected object to recognize.
 * \param[in,out] template_ The template to contain the object's recognition
 *                          information.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_augment(const janus_image image,
                                       janus_attributes *attributes,
                                       janus_template template_);

/*!
 * \brief Free memory for a template previously allocated by
 * \ref janus_allocate_template.
 *
 * Call this function on a template after it is no longer needed.
 * \param[in] template_ The template to deallocate.
 * \remark This function is \ref reentrant.
 * \see janus_allocate_template
 */
 JANUS_EXPORT janus_error janus_free_template(janus_template template_);

/*!
 * \brief A finalized representation of a template suitable for comparison.
 *
 * Ideally comparison should occur directly against the janus_flat_template.
 * Alternatively, the implementation may temporarily unmarshall this buffer into
 * a more suitable data structure.
 * \see janus_template
 */
typedef janus_data *janus_flat_template;

/*!
 * \brief The maximum size of templates generated by \ref janus_flatten_template
 *        and \ref janus_flatten_gallery.
 *
 * Should be less than or equal to 32 MB.
 * \remark This function is \ref thread_safe.
 */
JANUS_EXPORT size_t janus_max_template_size();

/*!
 * \brief Create a finalized template representation for verification with
 *        \ref janus_verify.
 * \param[in] template_ The recognition information to construct the
 *                      finalized template from.
 * \param[in,out] flat_template A pre-allocated buffer provided by the calling
 *                              application no smaller than
 *                              \ref janus_max_template_size to contain the
 *                              finalized template.
 * \param[out] bytes Size of the buffer actually used to store the template.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_flatten_template(const janus_template template_,
                                                janus_flat_template
                                                                  flat_template,
                                                size_t *bytes);

/*!
 * \brief Return a similarity score for two templates.
 *
 * Higher scores indicate greater similarity.
 *
 * The returned \p similarity score is \em symmetric. In other words, swapping
 * the order of \p a and \p b will not change \p similarity.
 *
 * \param[in] a The first template to compare.
 * \param[in] a_bytes Size of template a.
 * \param[in] b The second template to compare.
 * \param[in] b_bytes Size of template b.
 * \param[out] similarity Higher values indicate greater similarity.
 * \remark This function is \ref thread_safe.
 * \see janus_search
 */
JANUS_EXPORT janus_error janus_verify(const janus_flat_template a,
                                      const size_t a_bytes,
                                      const janus_flat_template b,
                                      const size_t b_bytes,
                                      float *similarity);

/*!
 * \brief Unique identifier for a \ref janus_template.
 *
 * Associate a template with a unique id using \ref janus_enroll.
 * Retrieve the unique id from a search using \ref janus_search.
 */
typedef int janus_template_id;

/*!
 * \brief A set of \ref janus_template.
 *
 * Can be extended with additional templates using \ref janus_enroll.
 */
typedef struct janus_gallery_type *janus_gallery;

/*!
 * \brief Allocate memory for an empty gallery.
 *
 * Memory is managed by the implementation and guaranteed until
 * \ref janus_free_gallery.
 *
 * Add templates to the gallery with \ref janus_enroll.
 *
 * \code
 * janus_gallery gallery;
 * janus_error error = janus_allocate_gallery(&gallery);
 * assert(!error);
 * \endcode
 *
 * \param[in] gallery An uninitialized gallery.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_allocate_gallery(janus_gallery *gallery);

/*!
 * \brief A finalized representation of a gallery suitable for comparison.
 *
 * Ideally comparison should occur directly against the janus_flat_gallery.
 * Alternatively, the implementation may temporarily unmarshall this buffer into
 * a more suitable data structure.
 * \see janus_gallery
 */
typedef janus_data *janus_flat_gallery;

/*!
 * \brief Add a template to the gallery.
 *
 * Use \ref janus_search for searching against the gallery.
 *
 * It is up to the calling application to provide unique \p template_id values.
 * The implementation may assume that multiple templates with the same
 * \p template_id belong to the same identity.
 *
 * \section gallery_size Gallery Size
 * Several functions depend on the notion of a "gallery size". The \em size of
 * a \p gallery is the number of templates in it, which is equal to the number
 * of calls made to \ref janus_enroll with it.
 *
 * \param[in] template_ The template to add.
 * \param[in] template_id A unique identifier for the template.
 * \param[in] gallery The gallery to take ownership of the template.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_enroll(const janus_template template_,
                                      const janus_template_id template_id,
                                      janus_gallery gallery);

/*!
 * \brief Free memory for a gallery previously allocated by
 * \ref janus_allocate_gallery.
 *
 * Call this function on a gallery after it is no longer needed.
 * \param[in] gallery The gallery to deallocate.
 * \remark This function is \ref reentrant.
 * \see janus_allocate_gallery
 */
 JANUS_EXPORT janus_error janus_free_gallery(janus_gallery gallery);

/*!
 * \brief Create a finalized gallery representation for search with
 *        \ref janus_search.
 * \param[in] gallery The recognition information to construct the
 *                    finalized gallery from.
 * \param[in,out] flat_gallery A pre-allocated buffer provided by the calling
 *                             application no smaller than
 *                             \ref janus_max_template_size * \ref gallery_size.
 * \param[out] bytes Size of the buffer actually used to store the gallery.
 * \remark This function is \ref reentrant.
 */
JANUS_EXPORT janus_error janus_flatten_gallery(const janus_gallery gallery,
                                               janus_flat_gallery flat_gallery,
                                               size_t *bytes);

/*!
 * \brief Ranked search for a template against a gallery.
 *
 * \p template_ids and \p similarities should be pre-allocated buffers large
 * enough to contain \p requested_returns elements. \p actual_returns will be
 * less than or equal to requested_returns, depending on the contents of the
 * gallery.
 *
 * The returned \p similarities \em may be normalized by the implementation
 * based on the contents of the \p gallery. Therefore, similarity scores
 * returned from searches against different galleries are \em not guaranteed to
 * be comparable.
 *
 * \param[in] probe Probe to search for.
 * \param[in] probe_bytes Size of probe.
 * \param[in] gallery Gallery to search against.
 * \param[in] gallery_bytes Size of gallery.
 * \param[in] num_requested_returns The desired number of returned results.
 * \param[out] template_ids Buffer to contain the \ref janus_template_id of the
 *                          top matching gallery templates.
 * \param[out] similarities Buffer to contain the similarity scores of the top
 *                          matching templates.
 * \param[out] num_actual_returns The number of populated elements in
 *                                template_ids and similarities. This value
 *                                could be zero.
 * \remark This function is \ref thread_safe.
 * \see janus_verify
 */
JANUS_EXPORT janus_error janus_search(const janus_flat_template probe,
                                      const size_t probe_bytes,
                                      const janus_flat_gallery gallery,
                                      const size_t gallery_bytes,
                                      const size_t num_requested_returns,
                                      janus_template_id *template_ids,
                                      float *similarities,
                                      size_t *num_actual_returns);

/*!
 * \brief Cluster a gallery into a set of identities.
 *
 * The output of this function is two arrays, \p template_ids and \p cluster_ids
 * of equal length, serving as a mapping between templates and clusters.
 *
 * \section clustering_hint Clustering Hint
 * Clustering is generally considered to be an ill-defined problem, and most
 * algorithms require some help determining the appropriate number of clusters.
 * The \p hint parameter helps influence the number of clusters, though the
 * implementation is free to ignore it.
 * - If \p hint is in the range [-1, 1] then it is a clustering
 * \em aggressiveness, with \c -1 favoring more clusters (fewer templates per
 * cluster), and \c 1 favoring fewer clusters (more templates per cluster).
 * - If \p hint is greater than 1 then it is a clustering \em count, indicating
 * the suggested number of clusters.
 * - The suggested default value for \p hint is \c 0.
 *
 * \note The implementation of this function is optional, and may return
 *       #JANUS_NOT_IMPLEMENTED.
 *
 * \param[in] gallery The gallery to cluster.
 * \param[in] hint A hint to the clustering algorithm, see \ref clustering_hint.
 * \param[out] template_ids A pre-allocated array provided by the calling
 *                          application large enough to hold \ref gallery_size
 *                          elements.
 * \param[out] cluster_ids A pre-allocated array provided by the calling
 *                         application large enough to hold \red gallery_size
 *                         elements.
 * \remark This function is \ref thread_safe.
 */
JANUS_EXPORT janus_error janus_cluster(const janus_flat_gallery gallery,
                                       const double hint,
                                       janus_template_id *template_ids,
                                       int *cluster_ids);
/*! @}*/

#ifdef __cplusplus
}
#endif

#endif /* IARPA_JANUS_H */
