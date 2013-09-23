#include <stdlib.h>
#include <stdio.h>

#include "janus.h"

int main(int argc, char *argv[])
{
    if (argc > 2) {
        printf("Usage: initialize [sdk_path]");
        return 1;
    }

    const char *sdk_path;
    if (argc > 1) sdk_path = argv[1];
    else          sdk_path = "../"; // Assumes it is run from <sdk_path>/bin

    janus_error error = janus_initialize(sdk_path);
    if (error != JANUS_SUCCESS) {
        printf("janus_initialize failed with error: %d\n", error);
        abort();
    }

    janus_finalize();

    return 0;
}
