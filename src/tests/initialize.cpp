#include <stdlib.h>
#include <stdio.h>

#include "janus.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: initialize sdk_path\n");
        return 1;
    }

    janus_error error = janus_initialize(argv[1]);
    if (error != JANUS_SUCCESS) {
        printf("Failed to initialize: %s\n", janus_error_to_string(error));
        abort();
    }

    janus_finalize();
    return 0;
}
