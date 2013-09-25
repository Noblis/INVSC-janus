#include <stdlib.h>

#include "janus.h"

void janus_free_object_list(janus_object_list object_list)
{
    for (janus_size i=0; i<object_list->size; i++)
        janus_free_object(object_list->objects[i]);
    free(object_list);
}
