#include <janus_io.h>

janus_media janus_read_image(const char *file)
{
    (void) file;
    return janus_allocate_media(0, 0, 0, 0);
}
