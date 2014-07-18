#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 5) || (argc > 7)) {
        printf("Usage: janus_enroll sdk_path data_path metadata_file gallery_file [[algorithm] verbose]\n");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argc >= 6 ? argv[5] : ""))
    JANUS_ASSERT(janus_create_gallery(argv[2], argv[3], argv[4], argc >= 7 ? atoi(argv[6]) : 0))
    JANUS_ASSERT(janus_finalize())

    janus_print_metrics(janus_get_metrics());
    return EXIT_SUCCESS;
}
