#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 6) || (argc > 7)) {
        printf("Usage: janus_evaluate sdk_path gallery_metadata probe_metadata mask simmat [algorithm]\n");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argc >= 8 ? argv[7] : ""))
    JANUS_ASSERT(janus_create_mask(argv[2], argv[3], argv[4]))
    JANUS_ASSERT(janus_create_simmat(argv[2], argv[3], argv[5]))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
