#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>

#include <janus_harness.h>

using namespace std;

void printUsage()
{
    printf("Usage: janus_verify sdk_path temp_path probe_data_path reference_data_path templates_matches_file scores_file [-algorithm <algorithm>] [-verbose]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 7;

    if ((argc < requiredArgs) || (argc > 10)) {
        printUsage();
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
    JANUS_ASSERT(janus_harness_verify(argv[3], argv[4], argv[5], argv[6], verbose));
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
