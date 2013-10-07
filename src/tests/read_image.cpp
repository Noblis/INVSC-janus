#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

// Makes assumptions that are tested by `initialize`
int main(int argc, char *argv[])
{
    if ((argc < 2) || (argc > 3)) {
        printf("Usage: read_image sdk_path [file_name]\n");
        return 1;
    }

    janus_initialize(argv[1]);

    const char *file_name = (argc >= 3 ? argv[2] : "../data/Kirchner0.jpg");
    janus_image image = janus_read_image(file_name);
    if (image == NULL) {
        printf("Failed to read image: %s\n", file_name);
        abort();
    }

    janus_free_image(image);
    janus_finalize();
    return 0;
}
