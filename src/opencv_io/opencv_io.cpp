#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "janus_io.h"

using namespace cv;

janus_media janus_read_image(const char *file)
{
    Mat mat = imread(file, IMREAD_UNCHANGED);
    if (!mat.data)
        return NULL;

    janus_media media = janus_allocate_media(mat.channels(), mat.cols, mat.rows);
    assert(mat.isContinuous());
    memcpy(media->data, mat.data, media->channels * media->width * media->height * sizeof(janus_data));
    return media;
}
