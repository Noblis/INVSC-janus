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

    janus_template target, query;
    size_t target_bytes, query_bytes;
    JANUS_TRY(janus_enroll_template(argv[2], &target, &target_bytes))
    JANUS_TRY(janus_enroll_template(argv[3], &query, &query_bytes))

    float similarity;
    JANUS_TRY(janus_verify(target, target_bytes, query, query_bytes, &similarity))
    printf("Similarity: %g\n", similarity);

    janus_finalize();
    return 0;
}
