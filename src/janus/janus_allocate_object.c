#include "janus.h"

janus_object janus_allocate_object(janus_size size)
{
    janus_object object;
    object.size = size;
    object.attributes = malloc(size * sizeof(janus_attribute));
    object.values = malloc(size * sizeof(janus_value));
    return object;
}
