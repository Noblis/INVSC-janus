#include <stddef.h>
#include <string.h>

#include <pittpatt_raw_image_io.h>

#include "janus_io.h"

janus_image janus_read_image(const char *file)
{
    ppr_raw_image_type ppr_image;
    ppr_raw_image_error_type error = ppr_raw_image_io_read(file, &ppr_image);
    if (error != PPR_RAW_IMAGE_SUCCESS)
        return NULL;

    if ((ppr_image.color_space != PPR_RAW_IMAGE_GRAY8) &&
        (ppr_image.color_space != PPR_RAW_IMAGE_BGR24))
        ppr_raw_image_convert(&ppr_image, PPR_RAW_IMAGE_BGR24);

    janus_image image = janus_allocate_image(ppr_image.color_space == PPR_RAW_IMAGE_GRAY8 ? 1 : 3, ppr_image.width, ppr_image.height);
    const janus_size elements_per_row = image->channels * image->width * sizeof(janus_data);
    for (int i=0; i<ppr_image.height; i++)
        memcpy(image->data + i*elements_per_row, ppr_image.data + i*ppr_image.bytes_per_line, elements_per_row);

    ppr_raw_image_free(ppr_image);
    return image;
}
