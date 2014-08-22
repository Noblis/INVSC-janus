#include <stdlib.h>
#include <string.h>

#include "janus.h"
#include "janus_io.h"

const char *get_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

static janus_flat_template getFlatTemplate(janus_metadata metadata, size_t *bytes)
{
    janus_template template_;
    janus_template_id template_id;
    JANUS_ASSERT(janus_create_template(metadata, &template_, &template_id))
    janus_flat_template flat_template = new janus_data[janus_max_template_size()];
    JANUS_ASSERT(janus_flatten(template_, flat_template, bytes))
    JANUS_ASSERT(janus_free(template_))
    return flat_template;
}

int main(int argc, char *argv[])
{
    if ((argc < 4) || (argc > 5)) {
        printf("Usage: janus_verify sdk_path target_metadata_file query_metadata_file [algorithm]\n");
        return 1;
    }

    const char *ext1 = get_ext(argv[2]);
    const char *ext2 = get_ext(argv[3]);

    if (strcmp(ext1, "csv") != 0 || strcmp(ext2, "csv") != 0) {
        printf("Metadata files must be \".csv\" format\n");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argc >= 5 ? argv[4] : ""))

    size_t target_bytes;
    janus_flat_template target_flat = getFlatTemplate(argv[2], &target_bytes);
    printf("Target bytes: %zu\n", target_bytes);

    size_t query_bytes;
    janus_flat_template query_flat = getFlatTemplate(argv[3], &query_bytes);
    printf("Query bytes: %zu\n", query_bytes);

    float similarity;
    JANUS_ASSERT(janus_verify(target_flat, target_bytes, query_flat, query_bytes, &similarity))
    printf("Similarity: %g\n", similarity);

    delete[] target_flat;
    delete[] query_flat;

    JANUS_ASSERT(janus_finalize())
    return EXIT_SUCCESS;
}
