#include <stddef.h>
#include <string.h>

#include <pittpatt_raw_image_io.h>

#include "janus_io.h"

janus_media janus_read_image(const char *file)
{
    ppr_raw_image_type image;
    ppr_raw_image_error_type error = ppr_raw_image_io_read(file, &image);
    if (error != PPR_RAW_IMAGE_SUCCESS)
        return NULL;

    if ((image.color_space != PPR_RAW_IMAGE_GRAY8) &&
        (image.color_space != PPR_RAW_IMAGE_BGR24))
        ppr_raw_image_convert(&image, PPR_RAW_IMAGE_BGR24);

    janus_media media = janus_allocate_media(image.color_space == PPR_RAW_IMAGE_GRAY8 ? 1 : 3, image.width, image.height);
    const janus_size elements_per_row = media->channels * media->width * sizeof(janus_data);
    for (int i=0; i<image.height; i++)
        memcpy(media->data + i*elements_per_row, image.data + i*image.bytes_per_line, elements_per_row);

    ppr_raw_image_free(image);
    return media;
}
