#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

// Makes assumptions that are tested by `read_image`
int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("Usage: detect_image sdk_path [file_name]\n");
        return 1;
    }

    janus_initialize(argv[1]);
    janus_context context;
    janus_initialize_context(&context);

    janus_image image = janus_read_image(argc >= 3 ? argv[2] : "../data/Kirchner0.jpg");
    janus_object_list faces;
    janus_error error = janus_detect(context, image, &faces);
    if (error != JANUS_SUCCESS) {
        printf("Failed to detect faces with error: %d\n", error);
        abort();
    }
    printf("Faces found: %d", faces->size);

    janus_free_object_list(faces);
    janus_free_image(image);
    janus_finalize_context(context);
    janus_finalize();
    return 0;
}
