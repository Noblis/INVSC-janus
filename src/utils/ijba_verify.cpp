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

void printUsage()
{
    printf("Usage: ijba_verify sdk_path temp_path comparison_list templates match_scores [-algorithm <algorithm>]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 6;

    if ((argc < requiredArgs) || (argc > 8)) {
        printUsage();
        return 1;
    }
    const char *ext1 = get_ext(argv[3]);
    const char *ext2 = get_ext(argv[4]);
    const char *ext3 = get_ext(argv[5]);

    if (strcmp(ext1, "csv") != 0) {
        printf("Comparison list must be \".csv\" format.");
        return 1;
    } else if (strcmp(ext2, "gal") != 0) {
        printf("Templates file must be \".gal\" format.");
        return 1;
    } else if (strcmp(ext3, "matches") != 0) {
        printf("Match scores must be output in \".matches\" format.");
        return 1;
    }

    char *algorithm = NULL;
    for (int i=0; i<argc-requiredArgs; i++)
        if (strcmp(argv[requiredArgs+i], "-algorithm") == 0)
            algorithm = argv[requiredArgs+(++i)];
        else {
            fprintf(stderr, "Unrecognized flag: %s\n", argv[requiredArgs+i]);
            return 1;
        }

    JANUS_ASSERT(janus_initialize(argv[1], argv[2], algorithm))
    JANUS_ASSERT(janus_verify_pairwise(argv[3], argv[4], argv[5]))
    JANUS_ASSERT(janus_finalize())

    janus_print_metrics(janus_get_metrics());
    return EXIT_SUCCESS;
}
