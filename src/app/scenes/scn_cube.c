#include "scn_cube.h"

#include <stdio.h>

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
    data->shader = shader_init(
        (void *)&data->shader_data, 
        (void *)cube_vertex_shader, (void *)cube_fragment_shader
    );

    vec3f p = { 0, 0, -10 };
    vec3f f = { 0, 0, -1 };
    vec3f u = { 0, 1, 0 };
    vec3f d = { p.x + f.x, p.y + f.y, p.z + f.z };

    data->cube = mesh_load("./res/models/cube.obj");
    data->projection = perspective_matrix(45.0f, 640/480, 0.5f, 100.f);
    lookat_matrix(&data->view, &p, &d, &u);
    data->model = mat4_identity();

    data->shader_data.projection = &data->projection;
    data->shader_data.view = &data->view;
    data->shader_data.model = &data->model;
}

void scn_cube_process_events(scn_cube_data_ *data)
{
    window_pump_messages(&data->app->window);
}

void scn_cube_update(scn_cube_data_ *data)
{
    static f32 angle;
    angle+=0.128f;
    if (angle > 360.f)
    {
        angle = (angle-360.f);
    }
    vec3f axis = { 0, 1, 1 };
    mat4 transform = mat4_identity();
    mat4 rotation = matrix_rotate(angle, &axis);
    mat4 scale = mat4_identity();

    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);

    data->model = trs;

}

void scn_cube_render(scn_cube_data_ *data)
{
    renderer_clear(&data->app->renderer);

    renderer_draw(&data->app->renderer, &data->cube, &data->shader);

    window_swap_buffers(&data->app->window);
}

void scn_cube_shutdown(scn_cube_data_ *data)
{
    mesh_free(&data->cube);
}