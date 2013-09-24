#include <opencv2/highgui/highgui.hpp>

#include "janus.h"

using namespace cv;

int main(int argc, char *argv[])
{
    if (argc > 2) {
        printf("Usage: attribute [sdk_path]");
        return 1;
    }

    const char *sdk_path;
    if (argc > 1) sdk_path = argv[1];
    else          sdk_path = "../";

    janus_error error = janus_initialize(sdk_path);
    if (error != JANUS_SUCCESS) {
        printf("Failed with error: %d\n", error);
        abort();
    }

    const char *file_name = "../data/Kirchner0.jpg";
    Mat image = imread(file_name);
    if (!image.data) {
        printf("Failed to open: %s\n", file_name);
        abort();
    }

    janus_finalize();
    return 0;
}
