#include <stdio.h>
#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

// Doesn't bother checking assumptions tested by `initialize`
int main(int argc, char *argv[])
{
    (void) argc;
    janus_initialize(argv[1]);

    const char *file = "/Users/m29396/openbr/data/MEDS/img/S001-01-t10_01.jpg";
    janus_media image = janus_read_image(file);
    if (image == NULL) {
        printf("Failed to read image: %s\n", file);
        abort();
    }
    janus_free_media(image);

    janus_finalize();
    return 0;
}
