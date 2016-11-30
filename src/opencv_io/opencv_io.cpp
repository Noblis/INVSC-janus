#include <opencv_io.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

bool janus_media_type::next(Mat &img)
{
    if (!video.isOpened()) { // image
        img = imread(filename, IMREAD_ANYCOLOR);
        return true;
    }

    // video - a little bit more complicated
    bool got_frame = video.read(img);
    if (!got_frame) { // Something went unexpectedly wrong (maybe a corrupted video?). Print a warning, set img to empty and return true.
        fprintf(stderr, "Fatal - Unexpectedly unable to collect next frame from video.");
        img = Mat();
        return true;
    }

    if (video.get(CV_CAP_PROP_POS_FRAMES) == frames) { // end of the video. Reset it and return true
        video.set(CV_CAP_PROP_POS_FRAMES, 0);
        return true;
    }

    return false;
}

bool janus_media_type::seek(uint32_t frame)
{
    if (!video.isOpened()) // image
        return false;

    if (frame > frames) // invalid index
        return false;

    // Set the video to the desired frame
    video.set(CV_CAP_PROP_POS_FRAMES, frame);
    return true;
}

janus_error janus_load_media(const string &filename, janus_media &media)
{
    media = new janus_media_type();
    media->filename = filename;
    media->frames = 1; // Initialize this to the image default to avoid an extra conditional later

    // To get the media dimensions we temporalily load either the image or
    // first frame. This also checks if the filename is valid
    Mat img = imread(filename, IMREAD_ANYCOLOR); // We use ANYCOLOR to load either BGR or Grayscale images
    if (!img.data) { // Couldn't load as an image maybe it's a video
        media->video = VideoCapture(filename);
        if (!video.isOpened()) {
            fprintf(stderr, "Fatal - Janus failed to read: %s\n", filename.c_str());
            delete media; // Delete the media object before returning an error
            return JANUS_INVALID_MEDIA;
        }

        bool got_frame = media->video(img);
        if (!got_frame) {
            delete media; // Delete the media object before returning an error
            return JANUS_INVALID_MEDIA;
        }
        media->frames = (uint32_t) media->video.get(CV_CAP_PROP_FRAME_COUNT);

        // Reset the video to frame 0
        media->video.set(CV_CAP_PROP_POS_FRAMES, 0);
    }

    // Set the dimensions
    media->channels = (uint32_t) img.channels();
    media->rows     = (uint32_t) img.rows;
    media->cols     = (uint32_t) img.cols;

    return JANUS_SUCCESS;
}

janus_error janus_free_media(janus_media &media)
{
    delete media;
    return JANUS_SUCCESS;
}
