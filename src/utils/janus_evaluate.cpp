#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 7) || (argc > 8)) {
        printf("Usage: janus_evaluate sdk_path data_path gallery_metadata probe_metadata mask simmat [algorithm]\n");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argc >= 9 ? argv[8] : ""))
    JANUS_ASSERT(janus_create_mask(argv[3], argv[4], argv[5]))
    JANUS_ASSERT(janus_create_simmat(argv[3], argv[4], argv[6], argv[2]))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
