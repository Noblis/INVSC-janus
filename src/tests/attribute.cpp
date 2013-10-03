#include <stdio.h>
#include <stdlib.h>

#include "janus.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: attribute sdk_path");
        return 1;
    }

    janus_error error = janus_initialize(argv[1]);
    if (error != JANUS_SUCCESS) {
        printf("Failed with error: %d\n", error);
        abort();
    }

    const char *file_name = "../data/Kirchner0.jpg";
    (void) file_name;

    janus_finalize();
    return 0;
}
