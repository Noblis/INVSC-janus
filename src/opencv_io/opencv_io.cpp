#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "janus_io.h"

using namespace cv;

static janus_image janusFromOpenCV(const Mat &mat)
{
    if (!mat.data)
        return NULL;

    janus_image image = janus_allocate_image(mat.channels(), mat.cols, mat.rows);
    assert(mat.isContinuous());
    memcpy(image->data, mat.data, image->channels * image->width * image->height * sizeof(janus_data));
    return image;
}

janus_image janus_read_image(const char *file)
{
    return janusFromOpenCV(imread(file, IMREAD_UNCHANGED));
}

struct janus_video_type
{
    VideoCapture videoCapture;
    janus_video_type(const char *file)
        : videoCapture(file) {}
};

janus_video janus_open_video(const char *file)
{
    return new janus_video_type(file);
}

janus_image janus_read_frame(janus_video video)
{
    if (!video)
        return NULL;

    Mat mat;
    video->videoCapture.read(mat);
    return janusFromOpenCV(mat);
}

void janus_close_video(janus_video video)
{
    delete video;
}
