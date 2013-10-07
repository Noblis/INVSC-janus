#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

// Makes assumptions that are tested by `initialize`
int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("Usage: read_video sdk_path [file_name]\n");
        return 1;
    }

    janus_initialize(argv[1]);

    const char *file_name = (argc >= 3 ? argv[2] : "../data/Kirchner.flv");
    janus_video video = janus_open_video(file_name);
    if (video == NULL) {
        printf("Failed to open video: %s\n", file_name);
        abort();
    }

    janus_image frame = janus_read_frame(video);
    if (frame == NULL) {
        printf("Failed to read frame from video: %s\n", file_name);
        abort();
    }

    janus_free_image(frame);
    janus_close_video(video);
    janus_finalize();
    return 0;
}
