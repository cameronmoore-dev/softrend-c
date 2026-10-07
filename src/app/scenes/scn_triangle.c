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

void scn_triangle_start(scn_triangle_data_ *data)
{
    data->shader = shader_init(
        (void *)&data->shader_data, 
        (void *)triangle_vertex_shader, (void *)triangle_fragment_shader
    );

    data->triangle[0].pos = (vec4f){ -0.5f, -0.5f, 0.0f, 1.0f };
    data->triangle[0].colour.packed = 0xFF0000;
    data->triangle[1].pos = (vec4f){ 0.5f, -0.5f, 0.0f, 1.0f };
    data->triangle[1].colour.packed = 0x00FF00;
    data->triangle[2].pos = (vec4f){ 0.0f, 0.5f, 0.0f, 1.0f };
    data->triangle[2].colour.packed = 0x0000FF;
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
    renderer_clear(&data->app->renderer);

    // renderer_draw(&data->app->renderer, data->triangle, &data->shader);

    window_swap_buffers(&data->app->window);
}

void scn_triangle_shutdown(scn_triangle_data_ *data)
{
    printf("Shutting Down\n");
}