#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iarpa_janus.h"
#include "iarpa_janus_io.h"

using namespace std;

const char *get_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

void printUsage()
{
    vector<janus_template> templates(100);
    vector<cluster_pair> pairs;
    printf("Usage: janus_cluster sdk_path temp_path templates_list_file hint clusters_output_list -algorithm <algorithm>] [-verbose]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 6;

    if ((argc < requiredArgs) || (argc > 10)) {
        printUsage();
        return 1;
    }

    const char *ext1 = get_ext(argv[3]);
    if (strcmp(ext1, "csv") != 0) {
        printf("templates_list_file must be \".csv\" format.\n");
        return 1;
    }

    ext1 = get_ext(argv[5]);
    if (strcmp(ext1, "csv") != 0) {
        printf("clusters_output_list must be \".csv\" format.\n");
        return 1;
    }

    char *algorithm = "";
    bool verbose = false;

    for (int i = 0; i < argc - requiredArgs; i++) {
        if (strcmp(argv[requiredArgs+i],"-algorithm") == 0)
            algorithm = argv[requiredArgs+(++i)];
        else if (strcmp(argv[requiredArgs+i],"-verbose") == 0)
            verbose = true;
        else {
            fprintf(stderr, "Unrecognized flag: %s\n", argv[requiredArgs+i]);
            return 1;
        }
    }

    JANUS_ASSERT(janus_initialize(argv[1], argv[2], algorithm, 0))
    JANUS_ASSERT(janus_cluster_helper(argv[3], static_cast<size_t>(atoi(argv[4])), argv[5], verbose))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
