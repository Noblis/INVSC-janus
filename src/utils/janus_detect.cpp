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
    printf("Usage: janus_detect sdk_path temp_path data_path images_file min_face_size detection_list_file [-algorithm <algorithm>] [-verbose]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 7;

    if ((argc < requiredArgs) || (argc > 10)) {
        printUsage();
        return 1;
    }

    const char *ext1 = get_ext(argv[4]);
    if (strcmp(ext1, "csv") != 0) {
        printf("images_file must be \".csv\" format.\n");
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
    JANUS_ASSERT(janus_detect_helper(argv[3], argv[4], atoi(argv[5]), argv[6], verbose))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
