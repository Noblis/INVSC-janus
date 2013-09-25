#include <stdlib.h>

#include "janus.h"

janus_object_list janus_allocate_object_list(janus_size size)
{
    // Initializing the object_list this way requires just one call to malloc
    // and simplifies the implementation of janus_free_object_list.
    janus_object_list object_list = malloc(sizeof(struct janus_object_list_type) +
                                           sizeof(janus_object) * size);
    object_list->size = size;
    object_list->objects = (janus_object*)(object_list + 1);
    return object_list;
}
