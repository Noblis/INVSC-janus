#include <stddef.h>

#include "janus.h"

janus_object_list janus_detect(const janus_media *media)
{
    (void) media;
    janus_object_list object_list;
    object_list.size = 0;
    object_list.objects = NULL;
    return object_list;
}
