#include "janus.h"

struct janus_template_impl
{
    // Template implementation goes here,
    // or cast proprietary template pointer to janus_template.
};

janus_error janus_initialize(const char *sdk_path)
{
    (void) sdk_path;
    return janus_success;
}

void janus_finalize()
{
}
