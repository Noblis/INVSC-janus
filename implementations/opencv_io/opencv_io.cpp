#include <iarpa_janus.h>
#include <opencv_io.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

bool janus_media_type::next(Mat &img)
{
    if (image) { // image
        img = imread(filename, IMREAD_ANYCOLOR);
        return true;
    }

    // video - a little bit more complicated
    bool got_frame = video.read(img);
    if (!got_frame) { // Something went unexpectedly wrong (maybe a corrupted video?). Print a warning, set img to empty and return true.
        fprintf(stderr, "Fatal - Unexpectedly unable to collect next frame from video.");
        img = Mat();
        return false;
    }

    if (video.get(CV_CAP_PROP_POS_FRAMES) == frames) { // end of the video. Reset it and return true
        video.set(CV_CAP_PROP_POS_FRAMES, 0);
        return true;
    }

    return true;
}

uint32_t janus_media_type::tell()
{
    return video.get(CV_CAP_PROP_POS_FRAMES);
}

bool janus_media_type::seek(uint32_t frame)
{
    if (image)          // can't seek in an image
        return false;

    if (frame > frames) // invalid index
        return false;

    // Set the video to the desired frame
    video.set(CV_CAP_PROP_POS_FRAMES, frame);
    return true;
}

janus_media_type::janus_media_type()
    : channels(0),
      height(0),
      width(0),
      frames(0), 
      image(false) {}

janus_media_type::janus_media_type(const string &filename) 
    : filename(filename),
      channels(0),
      height(0),
      width(0),
      frames(0),
      image(false)
{
    // To get the media dimensions we temporalily load either the image or
    // first frame. This also checks if the filename is valid
    Mat img = imread(filename, IMREAD_ANYCOLOR); // We use ANYCOLOR to load either BGR or Grayscale images
    if (!img.data) { // Couldn't load as an image maybe it's a video
        video = VideoCapture(filename);
        if (!video.isOpened())
            return;

        bool got_frame = video.read(img);
        if (got_frame) {
            // get number of frames
            frames = (uint32_t) video.get(CV_CAP_PROP_FRAME_COUNT);

            // Reset the video to frame 0
            video.set(CV_CAP_PROP_POS_FRAMES, 0);
            image = false;
        } else
            return;
    } else {
        image = true;
        frames = 1;
    }

    // Set the dimensions
    channels = (uint32_t) img.channels();
    height   = (uint32_t) img.rows;
    width    = (uint32_t) img.cols;
}

janus_media_type::~janus_media_type()
{
    if (video.isOpened())
        video.release();
}

bool janus_media_type::valid() const {
    return (channels > 0) & (height > 0) & (width > 0); 
}

bool janus_media_type::get_frame(Mat &img, uint32_t frame)
{
    if (this->seek(frame))
        if (this->next(img))
            return true;

    return false;
}

janus_error janus_load_media(const string &filename, janus_media &media) {
    media = new janus_media_type(filename);
    return JANUS_SUCCESS;
}

janus_error janus_delete_media(janus_media &media)
{
    delete media;
    return JANUS_SUCCESS;
}


