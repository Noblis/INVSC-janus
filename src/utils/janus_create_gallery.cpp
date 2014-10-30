#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "iarpa_janus.h"
#include "iarpa_janus_io.h"

const char *get_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

void printUsage()
{
    printf("Usage: janus_create_gallery sdk_path temp_path data_path metadata_file gallery_file [-algorithm <algorithm>] [-verbose]\n");
}

int main(int argc, char *argv[])
{
    int requiredArgs = 6;

    if ((argc < requiredArgs) || (argc > 9)) {
        printUsage();
        return 1;
    }

    const char *ext1 = get_ext(argv[4]);
    const char *ext2 = get_ext(argv[5]);
    if (strcmp(ext1, "csv") != 0) {
        printf("metadata_file must be \".csv\" format.\n");
        return 1;
    } else if (strcmp(ext2, "gal") != 0) {
        printf("gallery_file must be \".gal\" format. \n");
        return 1;
    }

    char *algorithm = NULL;
    int verbose = 0;

    for (int i=0; i<argc-requiredArgs; i++)
        if (strcmp(argv[requiredArgs+i],"-algorithm") == 0)
            algorithm = argv[requiredArgs+(++i)];
        else if (strcmp(argv[requiredArgs+i],"-verbose") == 0)
            verbose = 1;
        else {
            fprintf(stderr, "Unrecognized flag: %s\n", argv[requiredArgs+i]);
            return 1;
        }

    JANUS_ASSERT(janus_initialize(argv[1], argv[2], algorithm))

    janus_gallery gallery;
    JANUS_ASSERT(janus_allocate_gallery(&gallery))

    JANUS_ASSERT(janus_create_gallery(argv[3], argv[4], gallery, verbose))

    janus_metrics metrics = janus_get_metrics();
    size_t size = metrics.janus_initialize_template_speed.count;
    janus_flat_gallery flat_gallery = new janus_data[size*janus_max_template_size()];
    size_t bytes;
    JANUS_ASSERT(janus_flatten_gallery(gallery, flat_gallery, &bytes))
    std::ofstream file;
    file.open(argv[5], std::ios::out | std::ios::binary);
    file.write((char*)flat_gallery, bytes);
    file.close();
    JANUS_ASSERT(janus_finalize())

    janus_print_metrics(metrics);
    return EXIT_SUCCESS;
}
