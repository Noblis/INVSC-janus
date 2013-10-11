#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("Usage: track_video sdk_path [video_file_name]\n");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1]));

    janus_context context;
    JANUS_TRY(janus_initialize_context(&context));

    const char *file_name = (argc >= 3 ? argv[2] : "../data/Kirchner.flv");
    janus_video video = janus_open_video(file_name);
    if (video == NULL) {
        printf("Failed to open video: %s\n", file_name);
        abort();
    }

    janus_track track;
    JANUS_TRY(janus_initialize_track(&track))

    janus_image frame = janus_read_frame(video);
    if (frame == NULL) {
        printf("Failed to read frame from video: %s\n", file_name);
        abort();
    }

    int i = 0;
    while (frame != NULL) {
        printf("i = %d\n", i++);
        JANUS_TRY(janus_track_frame(context, frame, track))
        janus_free_image(frame);
        frame = janus_read_frame(video);
    }

    janus_object_list object_list;
    JANUS_TRY(janus_finalize_track(track, &object_list))
    printf("Faces tracks found: %d\n", object_list->size);

    janus_free_object_list(object_list);
    janus_close_video(video);
    janus_finalize_context(context);
    janus_finalize();
    return 0;
}
