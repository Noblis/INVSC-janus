#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("Usage: janus_enroll sdk_path gallery_csv gallery_file");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1], ""))
    JANUS_TRY(janus_enroll_gallery(argv[2], argv[3]))
    JANUS_TRY(janus_finalize())

    return EXIT_SUCCESS;
}
