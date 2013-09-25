#include <stdlib.h>

#include "janus.h"

janus_object janus_allocate_object(janus_size size)
{
    // Initializing the object this way requires just one call to malloc and
    // simplifies the implementation of janus_free_object.
    janus_object object = malloc(sizeof(struct janus_object_type) +
                                 sizeof(janus_attribute) * size +
                                 sizeof(janus_value) * size);
    object->size = size;
    object->attributes = (janus_attribute*)(object + 1);
    object->values = (janus_value*)(object->attributes + size);
    return object;
}
