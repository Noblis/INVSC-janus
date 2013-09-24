#include <stdlib.h>

#include "janus.h"

void janus_free_object(janus_object *object)
{
    free(object->attributes);
    free(object->values);
    object->size = 0;
    object->attributes = NULL;
    object->values = NULL;
}
