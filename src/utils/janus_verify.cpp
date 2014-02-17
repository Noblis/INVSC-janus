#include <stdlib.h>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if ((argc < 4) || (argc > 5)) {
        printf("Usage: janus_verify sdk_path target_metadata_file query_metadata_file [algorithm]\n");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1], argc >= 5 ? argv[4] : ""))

    janus_flat_template target = new janus_data[janus_max_template_size()];
    size_t target_bytes;
    JANUS_TRY(janus_create_template(argv[2], target, &target_bytes))
    printf("Target bytes: %zu\n", target_bytes);

    janus_flat_template query = new janus_data[janus_max_template_size()];
    size_t query_bytes;
    JANUS_TRY(janus_create_template(argv[3], query, &query_bytes))
    printf("Query bytes: %zu\n", query_bytes);

    double similarity;
    JANUS_TRY(janus_verify(target, target_bytes, query, query_bytes, &similarity))
    printf("Similarity: %g\n", similarity);

    delete[] target;
    delete[] query;

    JANUS_TRY(janus_finalize())
    return EXIT_SUCCESS;
}
