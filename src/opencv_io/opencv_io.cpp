#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "iarpa_janus_io.h"

using namespace std;
using namespace cv;

janus_error janus_load_media(const string &filename, janus_media &media)
{
    Mat img = imread(filename);
    if (!img.data) { // Couldn't load as an image maybe it's a video
        VideoCapture video(filename);
        if (!video.isOpened()) {
            fprintf(stderr, "Fatal - Janus failed to read: %s\n", filename.c_str());
            return JANUS_INVALID_MEDIA;
        }

        Mat frame;
        bool got_frame = video.read(frame);
        if (!got_frame)
            return JANUS_INVALID_MEDIA;

        media.width = frame.cols;
        media.height = frame.rows;
        media.color_space = frame.channels() == 3 ? JANUS_BGR24 : JANUS_GRAY8;

        do {
            janus_data *data = new janus_data[media.width * media.height * (media.color_space == JANUS_BGR24 ? 3 : 1)];
            memcpy(data, frame.data, media.width * media.height * (media.color_space == JANUS_BGR24 ? 3 : 1));
            media.data.push_back(data);
        } while (video.read(frame));
        return JANUS_SUCCESS;
    }

    media.width = img.cols;
    media.height = img.rows;
    media.color_space = (img.channels() == 3 ? JANUS_BGR24 : JANUS_GRAY8);

    janus_data *data = new janus_data[media.width * media.height * (media.color_space == JANUS_BGR24 ? 3 : 1)];
    memcpy(data, img.data, media.width * media.height * (media.color_space == JANUS_BGR24 ? 3 : 1));
    media.data.push_back(data);

    return JANUS_SUCCESS;
}

janus_error janus_free_media(janus_media &media)
{
    for (size_t i = 0; i < media.data.size(); i++)
        delete media.data[i];
    return JANUS_SUCCESS;
}
