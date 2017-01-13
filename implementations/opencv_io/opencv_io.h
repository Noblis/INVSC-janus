#ifndef OPENCV_IO_H
#define OPENCV_IO_H

#include <iarpa_janus.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

/*!
 * \brief A media object that loads images and videos in a lazy fashion
 */
struct JANUS_EXPORT janus_media_type
{
    JANUS_EXPORT janus_media_type();
    JANUS_EXPORT ~janus_media_type();

    /*!
     * \brief Create a media reader object
     * \param filename The filename of an image or video
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT janus_media_type(const std::string &filename);

    /*!
     * \brief Determines if media is loadable and valid
     * \return True if the media is valid (width > 0, height > 0, channels > 0)
     *         False otherwise
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT bool valid() const;

    /*!
     * \brief Retrieve the image if the media is an image or the next frame from
     *        the video if the image is a video.
     * \note The current behavior of the implementation is to reset the media
     *       automatically after the final call to next(). If the media is an
     *       image this means that successive calls to next() will store the same
     *       data in \ref img and return true each time. If the media is a video
     *       calling next() again after the last frame is retrieved will cause the
     *       video to loop back to the beginning and store the first frame in
     *       \ref img and return false.
     * \param img An empty container to store the retrieved information. The data in
     *            the image is unsigned 8 bit.
     * \return True if the media is an image or if the last frame was retrieved,
     *         false otherwise.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT bool next(cv::Mat &img);

    /*!
     * \brief Seek through the video to a particular location. If the media is an
     *        image this function can be considered a no-op.
     * \param frame The frame to seek to
     * \return True if the media is a video and the frame is valid (i.e within the
     *         bounds of the video), false if the frame is invalid or the media is
     *         an image.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT bool seek(uint32_t frame);

    /*!
     * \brief Get a specific frame
     * \param img An empty container to store the retrieved information. The data in
     *            the image is unsigned 8 bit.
     * \param frame The frame to extract
     * \return True if the media is a video and the frame is valid (i.e within the
     *         bounds of the video), false if the frame is invalid or the media is
     *         an image.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT bool get_frame(cv::Mat &img, uint32_t frame);

    /*!
     * \brief Get the next frame number
     * \return frame number
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT uint32_t tell();

    // data members below
    std::string filename; /*!< The filename of the image or video */
    uint32_t channels; /*!< The number of channels in the media.
                            Valid options are 1 (Grayscale) or 3 (Color) */
    uint32_t height; /*!< The number of rows in the media */
    uint32_t width; /*!< The number of cols in the media */
    uint32_t frames; /*!< The number of frames in the media.
                          If the media is an image this will be set to 1 */
    bool image; /*!< True if media is an image */

private:
    cv::VideoCapture video;
};

#endif // OPENCV_IO_H
