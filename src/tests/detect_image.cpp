#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

// Doesn't bother checking assumptions tested by `read_image`
int main(int argc, char *argv[])
{
    (void) argc;
    janus_initialize(argv[1]);

    janus_media image = janus_read_image("../data/Kirchner0.jpg");
    janus_object_list faces = janus_detect(image);
    janus_free_object_list(faces);
    janus_free_media(image);

    janus_finalize();
    return 0;
}
