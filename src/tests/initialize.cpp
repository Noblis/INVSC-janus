#include <stdlib.h>
#include <stdio.h>

#include "janus.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: initialize sdk_path");
        return 1;
    }

    janus_error error = janus_initialize(argv[1]);
    if (error != JANUS_SUCCESS) {
        printf("Failed with error: %d\n", error);
        abort();
    }

    janus_finalize();
    return 0;
}
