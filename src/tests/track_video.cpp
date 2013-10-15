#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 5)) {
        printf("Usage: track_video sdk_path [video_file_name [start_frame stop_frame]]\n");
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

    int start_frame;
    if (argc >= 4) start_frame = atoi(argv[3]);
    else           start_frame = 0;

    int stop_frame;
    if (argc >= 5) stop_frame = atoi(argv[4]);
    else           stop_frame = INT32_MAX;

    int i = 0;
    while (frame != NULL) {
        fprintf(stderr, "\rFrame: %d", i);
        if (i >= start_frame)
            JANUS_TRY(janus_track_frame(context, frame, track))
        janus_free_image(frame);

        i++;
        if (i > stop_frame) break;
        else                frame = janus_read_frame(video);
    }

    janus_object_list object_list;
    JANUS_TRY(janus_finalize_track(track, &object_list))
    fprintf(stderr, "\rFaces tracks found: %d\n", object_list->size);

    janus_free_object_list(object_list);
    janus_close_video(video);
    janus_finalize_context(context);
    janus_finalize();
    return 0;
}
