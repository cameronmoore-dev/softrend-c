#include "scn_cube.h"

void scn_cube_init(scene_funcs_ *funcs)
{
    funcs->start = (void *)scn_cube_start;
    funcs->process_events = (void *)scn_cube_process_events;
    funcs->update = (void *)scn_cube_update;
    funcs->render = (void *)scn_cube_render;
    funcs->shutdown = (void *)scn_cube_shutdown;
}

void scn_cube_start(scn_cube_data_ *data)
{

}

void scn_cube_process_events(scn_cube_data_ *data)
{
    window_pump_messages(&data->app->window);
}

void scn_cube_update(scn_cube_data_ *data)
{

}

void scn_cube_render(scn_cube_data_ *data)
{
    renderer_clear(&data->app->renderer);

    window_swap_buffers(&data->app->window);
}

void scn_cube_shutdown(scn_cube_data_ *data)
{

}