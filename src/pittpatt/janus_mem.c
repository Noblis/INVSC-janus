#include <stdlib.h>

#include "janus.h"

// These implementations of janus_allocate_* require just one call to malloc
// and simplify the implementations of janus_free_*.

janus_media janus_allocate_media(janus_size channels, janus_size columns, janus_size rows)
{
    janus_media media = malloc(sizeof(struct janus_media_type) +
                               sizeof(janus_data) * channels * columns * rows);
    media->channels = channels;
    media->width = columns;
    media->height = rows;
    media->data = (janus_data*)(media + 1);
    return media;
}

janus_object janus_allocate_object(janus_size size)
{
    janus_object object = malloc(sizeof(struct janus_object_type) +
                                 sizeof(janus_attribute) * size +
                                 sizeof(janus_value) * size);
    object->size = size;
    object->attributes = (janus_attribute*)(object + 1);
    object->values = (janus_value*)(object->attributes + size);
    return object;
}

janus_object_list janus_allocate_object_list(janus_size size)
{
    janus_object_list object_list = malloc(sizeof(struct janus_object_list_type) +
                                           sizeof(janus_object) * size);
    object_list->size = size;
    object_list->objects = (janus_object*)(object_list + 1);
    return object_list;
}

void janus_free_media(janus_media media)
{
    free(media);
}

void janus_free_object(janus_object object)
{
    free(object);
}

void janus_free_object_list(janus_object_list object_list)
{
    for (janus_size i=0; i<object_list->size; i++)
        janus_free_object(object_list->objects[i]);
    free(object_list);
}
