#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: read_image sdk_path");
        return 1;
    }

    janus_error error = janus_initialize(argv[1]);
    if (error != JANUS_SUCCESS) {
        printf("Failed with error: %d\n", error);
        abort();
    }

    janus_media image = janus_read_image("../data/Kirchner0.jpg");
    janus_free_media(image);

    janus_finalize();
    return 0;
}
