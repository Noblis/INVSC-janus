#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "janus_io.h"

using namespace cv;

janus_image janus_read_image(const char *file)
{
    Mat mat = imread(file, IMREAD_UNCHANGED);
    if (!mat.data)
        return NULL;

    janus_image image = janus_allocate_image(mat.channels(), mat.cols, mat.rows);
    assert(mat.isContinuous());
    memcpy(image->data, mat.data, image->channels * image->width * image->height * sizeof(janus_data));
    return image;
}
