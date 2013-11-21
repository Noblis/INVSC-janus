#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("Usage: janus_verify sdk_path target_csv query_csv\n");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1], ""))

    janus_finalize();
    return 0;
}
