#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <janus_harness.h>

using namespace std;

const char *get_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

void printUsage()
{
    printf("Usage: janus_cluster sdk_path temp_path templates_list_file list_type(T|M) hint clusters_output_list -algorithm <algorithm>] [-verbose]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 7;

    if ((argc < requiredArgs) || (argc > 10)) {
        printUsage();
        return 1;
    }

    const char *ext1 = get_ext(argv[3]);
    if (strcmp(ext1, "csv") != 0) {
        printf("list_file must be \".csv\" format.\n");
        return 1;
    }

    ext1 = get_ext(argv[6]);
    if (strcmp(ext1, "csv") != 0) {
        printf("clusters_output_list must be \".csv\" format.\n");
        return 1;
    }

    char *algorithm = "";
    bool verbose = false;
    bool is_template_list;

    if (strlen(argv[4]) == 1) {
        switch (argv[4][0]) {
            case 'T':
            case 't': is_template_list = true;  break;
            case 'M':
            case 'm': is_template_list = false; break;
            default:
                printf("Invalid list type.  Must be T for template list or M for media list.\n");
                return 1;
        }
    } else {
       printf("Invalid list type.  Must be T for template list or M for media list.\n");
       return 1;
    }

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
    JANUS_ASSERT(janus_harness_cluster(argv[3], is_template_list, static_cast<size_t>(atoi(argv[5])), argv[6], verbose))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
