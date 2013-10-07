#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

// Makes assumptions that are tested by `read_image`
int main(int argc, char *argv[])
{
    (void) argc;
    janus_initialize(argv[1]);

    janus_image image = janus_read_image("../data/Kirchner0.jpg");
    janus_object_list faces = janus_detect(image);
    if (faces->size != 1) {
        printf("Expected 1 face detection, got: %d", faces->size);
        abort();
    }
    janus_free_object_list(faces);
    janus_free_image(image);

    janus_finalize();
    return 0;
}
