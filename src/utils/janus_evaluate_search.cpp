#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "iarpa_janus.h"
#include "iarpa_janus_io.h"
using namespace std;

const char *get_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

int main(int argc, char *argv[])
{
    if ((argc < 10) || (argc > 11)) {
        printf("Usage: janus_evaluate_search sdk_path temp_path target_gallery query_gallery target_metadata query_metadata simmat mask num_returns [algorithm]\n");
        return 1;
    }
    const char *ext1 = get_ext(argv[3]);
    const char *ext2 = get_ext(argv[4]);
    const char *ext3 = get_ext(argv[6]);
    const char *ext4 = get_ext(argv[6]);
    const char *ext5 = get_ext(argv[7]);
    const char *ext6 = get_ext(argv[8]);

    if (strcmp(ext1, "gal") != 0 || strcmp(ext2, "gal") != 0) {
        printf("Gallery files must be \".gal\" format.\n");
        return 1;
    } else if (strcmp(ext3, "csv") != 0 || strcmp(ext4, "csv") != 0) {
        printf("Metadata files must be \".csv\" format. \n");
        return 1;
    } else if (strcmp(ext5, "mtx") != 0) {
        printf("Similarity matrix files should be \".mtx\" format.");
        return 1;
    } else if (strcmp(ext6, "mask") != 0) {
        printf("Mask matrix files should be \".mask\" format.");
        return 1;
    }

    JANUS_ASSERT(janus_initialize(argv[1], argv[2], argc >= 11 ? argv[10] : ""))
    int num_requested_returns = atoi(argv[9]);

    std::ifstream target;
    target.open(argv[3], std::ios::binary | std::ios::ate);
    size_t target_bytes = target.tellg();
    target.seekg(0, ios::beg);

    janus_flat_gallery target_flat = new janus_data[target_bytes];
    target.read((char*)target_flat, target_bytes);

    JANUS_ASSERT(janus_evaluate_search(target_flat, target_bytes, argv[4], argv[5], argv[6], argv[7], argv[8], num_requested_returns))
    JANUS_ASSERT(janus_finalize())

    janus_print_metrics(janus_get_metrics());
    return EXIT_SUCCESS;
}
