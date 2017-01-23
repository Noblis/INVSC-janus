#ifndef IARPA_JANUS_IO_H
#define IARPA_JANUS_IO_H

/*!
 * \brief A struct representing an image or a single frame. This structure
 *        assumes the underlying data is a continous row-major array of the
 *        uint8_t type.
 */
struct JANUS_EXPORT janus_image
{
    /*!
     * \brief Construct an image from a set of dimensions. It is anticipated
     *        that \ref data is allocated during. Images allocated with this
     *        constructor own \ref data and \ref owner should be set to true.
     * \param channels The number of channels in the image.
     * \param rows The number of rows in the image.
     * \param cols The number of columns in the image.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT janus_image(uint32_t channels, 
                             uint32_t rows, 
                             uint32_t cols) noexcept;
    /*!
     * \brief Construct an image from a set of dimensions and a data pointer.
     *        The length of \ref data must equal \ref channels * \ref rows *
     *        \ref cols * \ref frames. Images allocated with this constructor
     *        do NOT own the \ref data and \ref owner should be set to false.
     * \param channels The number of channels in the image.
     * \param rows The number of rows in the image.
     * \param cols The number of columns in the image.
     * \param data An array of data.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT janus_image(uint32_t channels,
                             uint32_t rows,
                             uint32_t cols,
                             uint8_t* data) noexcept;

    /*!
     * \brief Free any memory allocated for the image. This function must
     *        respect the value of \ref owner.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT ~janus_image() noexcept;

    /*!
     * \brief A utility function to facilitate data access. Note that image
     *        assumes contiguous row-major data.
     * \param channel The channel to index to.
     * \param row The row to index to.
     * \param col The column to index to.
     * \param value A pointer to hold the value at the computed index.
     * 
     * \remark This function is \ref reentrant.
     */
    JANUS_EXPORT janus_error at(uint32_t channel,
                                uint32_t row,
                                uint32_t col,
                                uint8_t& value) const;

    uint32_t channels; /*!< The number of channels in the image. */
    uint32_t rows; /*!< The number of rows in the image. */
    uint32_t cols; /*!< The number of columns in the image. */

    uint8_t* data; /*!< An array of the image pixel values. */
    bool owner; /*!< A flag indicating if the data is owned by the image or
                     not. */
};

/*!
 * \brief An opaque pointer to hold implementation-specific state.
 */
typedef struct janus_media_iterator_state* janus_media_iterator_state_type;

/*!
 * \brief The type of media
 */
enum janus_media_type
{
    Image = 0,
    Video = 1
};

/*!
 * \brief An iterator class that iterates over the frames of a video
 */
struct JANUS_EXPORT janus_media_iterator
{
    /*!
     * \brief Construct a media iterator from a filename
     * \param filename The filename of an image or video.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT janus_media_iterator(const std::string& filename) noexcept;

    /*!
     * \brief Delete any memory associated with a media iterator
     */
    JANUS_EXPORT ~janus_media_iterator() noexcept;

    /*!
     * \brief Determine if the media is loadable adn valid
     * \return JANUS_SUCCESS if the media is valid. An error code otherwise.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT janus_error valid() const;

    /*!
     * \brief Seek to and load the next frame.
     * \param img A pointer to an image to hold the next frame
     * \return JANUS_SUCCESS if the next frame is accessible and retrieved
     *         successfully. An error code otherwise.
     * \remark This function is \ref thread_unsafe.
     */ 
    JANUS_EXPORT janus_error next(janus_image& img);

    /*!
     * \brief Seek to a specific frame.
     * \param frame The frame to seek to.
     * \return JANUS_SUCCESS if the iterator successfully moved to the frame.
     *         An error code otherwise.
     * \remark This function is \ref thread_unsafe.
     */
    JANUS_EXPORT janus_error seek(uint32_t frame);

    /*!
     * \brief Seek to and retrieve a frame.
     * \param img A pointer to an image to hold the desired frame.
     * \param frame The frame to retrieve.
     * \return JANUS_SUCCESS if the frame was retrieved successfully. An error
     *         code otherwise.
     * \remark This function is \ref thread_unsafe.
     */ 
    JANUS_EXPORT janus_error get(janus_image& img, uint32_t frame);

    /*!
     * \brief Get the next frame number.
     * \param frame A pointer to hold the next frame number
     * \return JANUS_SUCCESS if the next frame was extracted successfully. An
     *         error code otherwise.
     * \remark This function is \ref thread_unsafe.
     */ 
    JANUS_EXPORT janus_error tell(uint32_t& frame);

    std::string filename; /*!< The filename of the image or video. */
    janus_media_type type; /*!< The type of media. */

    uint32_t channels; /*!< The number of the channels in the media. */
    uint32_t rows; /*!< The number of rows in the media. */
    uint32_t cols; /*!< The number of columns in the media */
    uint32_t frames; /*!< The number of frames in the media. In the case of an
                          image this should be 1. */

private:
    janus_media_iterator_state _internal; /*!< An opaque pointer to hold
                                               implementation specific internal
                                               state. */
};

#endif // IARPA_JANUS_IO_H

