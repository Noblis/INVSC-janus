#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("Usage: detect_image sdk_path [file_name]\n");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1]));

    janus_context context;
    JANUS_TRY(janus_initialize_context(&context));

    const char *file_name = (argc >= 3 ? argv[2] : "../data/Kirchner0.jpg");
    janus_image image = janus_read_image(file_name);
    if (image == NULL) {
        printf("Failed to read image: %s\n", file_name);
        abort();
    }

    janus_object_list faces;
    JANUS_TRY(janus_detect(context, image, &faces));
    printf("Faces found: %d\n", faces->size);

    janus_free_object_list(faces);
    janus_free_image(image);
    janus_finalize_context(context);
    janus_finalize();
    return 0;
}
