#include <stdlib.h>

#include "janus.h"

void janus_free_object(janus_object object)
{
    free(object);
}
