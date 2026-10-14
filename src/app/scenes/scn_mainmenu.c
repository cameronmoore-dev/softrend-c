#include "scn_mainmenu.h"

#include "scn_triangle.h"
#include "scn_cube.h"

void scn_mainmenu_init(scene_funcs_ *funcs)
{
    funcs->start = (void *)scn_mainmenu_start;
    funcs->process_events = (void *)scn_mainmenu_process_events;
    funcs->update = (void *)scn_mainmenu_update;
    funcs->render = (void *)scn_mainmenu_render;
    funcs->shutdown = (void *)scn_mainmenu_shutdown;
}

void scn_mainmenu_start(scn_mainmenu_data_ *data)
{
}

void scn_mainmenu_process_events(scn_mainmenu_data_ *data)
{
    input_ *input = &data->app->window->input;
    if (is_key_pressed(input, ESC))
    {
        scn_mainmenu_shutdown(data);
    }

    else if (is_key_pressed(input, NUM1))
    {
        scn_triangle_data_ triangle_data = {};
        triangle_data.app = data->app;
        
        scene_ triangle;
        triangle.data = (void *)&triangle_data;
        scn_triangle_init(&triangle.funcs);
        scene_add(&data->app->scene_manager, &triangle, sizeof(scn_triangle_data_));
    }

    else if (is_key_pressed(input, NUM2))
    {
        scn_cube_data_ cube_data;
        cube_data.app = data->app;
        cube_data.delta_time = data->delta_time;

        scene_ scn_cube;
        scn_cube.data = (void *)&cube_data;
        scn_cube_init(&scn_cube.funcs);
        scene_add(&data->app->scene_manager, &scn_cube, sizeof(scn_cube_data_));
    }
}

void scn_mainmenu_update(scn_mainmenu_data_ *data)
{
}

void scn_mainmenu_render(scn_mainmenu_data_ *data)
{
    renderer_clear(&data->app->renderer);

    window_swap_buffers(data->app->window);
}

void scn_mainmenu_shutdown(scn_mainmenu_data_ *data)
{
    window_close(data->app->window);
}