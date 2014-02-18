#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 5) || (argc > 6)) {
        printf("Usage: janus_simmat sdk_path gallery_metadata probe_metadata simmat [algorithm]\n");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argc >= 7 ? argv[6] : ""))
    JANUS_ASSERT(janus_create_simmat(argv[2], argv[3], argv[4]))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
