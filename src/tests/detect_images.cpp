#include <stdlib.h>
#include <vector>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: detect_images sdk_path [image_file_name ... image_file_name]\n");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1]))

    janus_context context;
    JANUS_TRY(janus_initialize_context(&context))

    const char *default_file_names[4] = { "../data/Kirchner0.jpg",
                                          "../data/Kirchner1.jpg",
                                          "../data/Kirchner2.jpg",
                                          "../data/Kirchner3.jpg" };
    const int default_file_count = 4;

    const char **file_names;
    int file_count;
    if (argc >= 3) {
        file_names = (const char **) &argv[2];
        file_count = argc - 3;
    } else {
        file_names = default_file_names;
        file_count = default_file_count;
    }

    for (int i=0; i<file_count; i++) {
        janus_image image;
        JANUS_TRY(janus_read_image(file_names[i], &image))

        janus_object_list faces;
        JANUS_TRY(janus_detect(context, image, &faces))
        fprintf(stderr, "Found: %d faces in:%s\n", faces->size, file_names[i]);

        janus_free_image(image);
        janus_free_object_list(faces);
    }

    janus_finalize_context(context);
    janus_finalize();
    return 0;
}
