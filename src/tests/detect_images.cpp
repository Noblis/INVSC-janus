#include <stdlib.h>
#include <vector>

#include "janus.h"
#include "janus_io.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: detect_images sdk_path [image_file_name ... image_file_name]\n");
        return 1;
    }

    JANUS_TRY(janus_initialize(argv[1]))

    janus_context context;
    JANUS_TRY(janus_initialize_context(&context))

    const char *default_file_names[4] = { "../data/Kirchner0.jpg",
                                          "../data/Kirchner1.jpg",
                                          "../data/Kirchner2.jpg",
                                          "../data/Kirchner3.jpg" };
    const int default_file_count = 4;

    const char **file_names;
    int file_count;
    if (argc >= 3) {
        file_names = (const char **) &argv[2];
        file_count = argc - 3;
    } else {
        file_names = default_file_names;
        file_count = default_file_count;
    }

    std::vector<janus_object_list> object_lists;
    for (int i=0; i<file_count; i++) {
        janus_image image;
        JANUS_TRY(janus_read_image(file_names[i], &image))

        janus_object_list faces;
        JANUS_TRY(janus_detect(context, image, &faces))
        fprintf(stderr, "Found: %d faces in:%s\n", faces->size, file_names[i]);

        object_lists.push_back(faces);
        janus_free_image(image);
    }

    std::vector<janus_attribute_list> all_attribute_lists;
    for (janus_size i=0; i<object_lists.size(); i++) {
        janus_object_list object_list = object_lists[i];
        for (janus_size j=0; j<object_lists[i]->size; j++) {
            janus_object object = object_list->objects[j];
            for (janus_size k=0; k<object->size; k++)
                all_attribute_lists.push_back(object->attribute_lists[k]);
        }
    }

    janus_object all_faces;
    JANUS_TRY(janus_allocate_object(all_attribute_lists.size(), &all_faces))
    all_faces->attribute_lists = all_attribute_lists.data();

    janus_object selected_faces;
    JANUS_TRY(janus_downsample(all_faces, &selected_faces))
    fprintf(stderr, "Downsampled from: %d to: %d faces.\n", all_faces->size, selected_faces->size);

    janus_free_object(selected_faces);
    all_faces->size = 0;
    janus_free_object(all_faces);

    for (size_t i=0; i<object_lists.size(); i++)
        janus_free_object_list(object_lists[i]);

    janus_finalize_context(context);
    janus_finalize();
    return 0;
}
