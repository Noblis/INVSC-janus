#include <stdlib.h>

#include "janus.h"

janus_media janus_allocate_media(janus_size channels, janus_size columns, janus_size rows, janus_size frames)
{
    // Initializing the media this way requires just one call to malloc and
    // simplifies the implementation of janus_free_media.
    janus_media media = malloc(sizeof(struct janus_media_type) +
                               sizeof(janus_data) * channels * columns * rows * frames);
    media->channels = channels;
    media->columns = columns;
    media->rows = rows;
    media->frames = frames;
    media->data = (janus_data*)(media + 1);
    return media;
}
