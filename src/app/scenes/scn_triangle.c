#include "app/scenes/scn_triangle.h"
#include "app/resource/mesh_loader.h"
#include "data_structures/darr.h"

#include <stdlib.h>

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
    data->shader = shader_init((void *)&data->shader_data, (void *)triangle_vertex_shader, (void *)triangle_fragment_shader);

    data->triangle.vertices = darr_create(sizeof(vertex_), 3);
    data->triangle.num_vertices = 3;

    vertex_ *v0 = darr_at(data->triangle.vertices, 0);
    v0->pos = (vec4f){ 0.5f, 0.5f, 0.0f, 1.0f };
    v0->colour.packed = 0xFF0000;

    vertex_ *v1 = darr_at(data->triangle.vertices, 1);
    v1->pos = (vec4f){ -0.5f, 0.5f, 0.0f, 1.0f };
    v1->colour.packed = 0x00FF00;

    vertex_ *v2 = darr_at(data->triangle.vertices, 2);
    v2->pos = (vec4f){ 0.0f, -0.5f, 0.0f, 1.0f };
    v2->colour.packed = 0x0000FF;
}

void scn_triangle_process_events(scn_triangle_data_ *data)
{
    if (is_key_pressed(&data->app->window->input, ESC))
    {
        scn_triangle_shutdown(data);
    }
}

void scn_triangle_update(scn_triangle_data_ *data)
{
}

void scn_triangle_render(scn_triangle_data_ *data)
{
    renderer_clear(&data->app->renderer);
    renderer_draw(&data->app->renderer, &data->triangle, &data->shader);
    window_swap_buffers(data->app->window);
}

void scn_triangle_shutdown(scn_triangle_data_ *data)
{
    mesh_free(&data->triangle);
    scene_remove(&data->app->scene_manager);
}