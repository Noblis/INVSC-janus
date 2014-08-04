#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 8) || (argc > 9)) {
        printf("Usage: janus_evaluate sdk_path target_gallery query_gallery target_metadata query_metadata simmat mask [algorithm]\n");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argc >= 9 ? argv[8] : ""))
    JANUS_ASSERT(janus_evaluate(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]))
    JANUS_ASSERT(janus_finalize())

    janus_print_metrics(janus_get_metrics());
    return EXIT_SUCCESS;
}
