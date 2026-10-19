#include "app/app_context.h"

#include <stdlib.h>

void app_context_close(app_context_ *app)
{
    window_cleanup(app->window);
    renderer_close(&app->renderer);
    scene_manager_close(&app->scene_manager);
}