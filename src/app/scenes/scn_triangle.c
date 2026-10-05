#include "app/scenes/scn_triangle.h"

#include <stdio.h>

void scn_triangle_init(scene_funcs_ *funcs)
{
    funcs->start = (void *)scn_triangle_start;
    funcs->process_events = (void *)scn_triangle_process_events;
    funcs->update = (void *)scn_triangle_update;
    funcs->render = (void *)scn_triangle_render;
    funcs->shutdown = (void *)scn_triangle_shutdown;
}

void scn_triangle_start(scn_triangle_data_ *scene_context)
{
}

void scn_triangle_process_events(scn_triangle_data_ *data)
{
    window_pump_messages(&data->app->window);
}

void scn_triangle_update(scn_triangle_data_ *data)
{
}

void scn_triangle_render(scn_triangle_data_ *data)
{
    window_swap_buffers(&data->app->window);
}

void scn_triangle_shutdown(scn_triangle_data_ *data)
{
    printf("Shutting Down\n");
}