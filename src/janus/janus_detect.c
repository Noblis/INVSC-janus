#include <stddef.h>

#include "janus.h"

janus_object_list janus_detect(const janus_media media)
{
    (void) media;
    return janus_allocate_object_list(0);
}
