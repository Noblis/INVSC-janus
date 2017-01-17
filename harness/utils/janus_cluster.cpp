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
    printf("Usage: janus_cluster sdk_path temp_path templates_list_file list_type(T) hint clusters_output_list -algorithm <algorithm>] [-verbose]\n");
    printf("Usage: janus_cluster sdk_path temp_path templates_list_file list_type(M) data_path hint clusters_output_list -algorithm <algorithm>] [-verbose]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 7;

    if ((argc < requiredArgs) || (argc > 10)) {
        printUsage();
        return 1;
    }

    char *sdk_path = argv[1];
    char *temp_path = argv[2];
    char *data_path = "";
    char *templates_list_file = argv[3];
    char *output_list = ""; argv[7];
    char *algorithm = "";

    bool verbose = false;
    bool is_template_list;
    size_t hint = 0;

    if (strlen(argv[4]) == 1) {
        switch (argv[4][0]) {
            case 'T':
            case 't': is_template_list = true;
                      hint = static_cast<size_t>(atoi(argv[5]));
                      output_list = argv[6]; break;
            case 'M':
            case 'm': is_template_list = false;
                      data_path = argv[5];
		      hint = static_cast<size_t>(atoi(argv[6]));
                      output_list = argv[7];
                      requiredArgs++; break;
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

    JANUS_ASSERT(janus_initialize(sdk_path, temp_path, algorithm, 0))
    JANUS_ASSERT(janus_harness_cluster(templates_list_file, is_template_list, data_path, hint, output_list, verbose))
    JANUS_ASSERT(janus_finalize())

    return EXIT_SUCCESS;
}
