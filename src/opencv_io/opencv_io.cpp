#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "janus_io.h"

using namespace cv;

static janus_image janusFromOpenCV(const Mat &mat)
{
    if (!mat.data)
        return NULL;

    janus_image image;
    janus_allocate_image(mat.channels(), mat.cols, mat.rows, &image);
    assert(mat.isContinuous());
    memcpy(image->data, mat.data, image->channels * image->width * image->height * sizeof(janus_data));
    return image;
}

janus_image janus_read_image(const char *file_name)
{
    return janusFromOpenCV(imread(file_name, IMREAD_UNCHANGED));
}

janus_video janus_open_video(const char *file_name)
{
    return reinterpret_cast<janus_video>(new VideoCapture(file_name));
}

janus_image janus_read_frame(janus_video video)
{
    if (!video)
        return NULL;

    Mat mat;
    reinterpret_cast<VideoCapture*>(video)->read(mat);
    return janusFromOpenCV(mat);
}

void janus_close_video(janus_video video)
{
    delete reinterpret_cast<VideoCapture*>(video);
}
