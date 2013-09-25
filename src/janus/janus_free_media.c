#include <stdlib.h>

#include "janus.h"

void janus_free_media(janus_media media)
{
    free(media);
}
