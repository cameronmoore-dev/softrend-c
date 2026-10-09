#include "scn_cube.h"

#include <stdio.h>
#include <math.h>

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

    data->albedo = image_load("./res/textures/texture_dirtcube.tga");
    data->cube = mesh_load("./res/models/cube.obj");
    data->camera = camera_init();
    data->model = mat4_identity();

    data->shader_data.projection = &data->camera.projection;
    data->shader_data.view = &data->camera.view;
    data->shader_data.model = &data->model;
    data->shader_data.albedo = &data->albedo;
}

void scn_cube_process_events(scn_cube_data_ *data)
{
    window_pump_messages(&data->app->window);

    if (is_key_pressed(&data->app->window.input, ESC))
    {
        scn_cube_shutdown(data);
    }

    camera_update_position(&data->camera, &data->app->window.input, *data->delta_time);

    s32 ox, oy;
    ox = data->app->window.input.mouse_dx;
    oy = data->app->window.input.mouse_dy;
    camera_update_rotation(&data->camera, &data->app->window.input, (f32)ox, (f32)oy, *data->delta_time);
    camera_update_view(&data->camera);
}

void scn_cube_update(scn_cube_data_ *data)
{
    static f32 angle;
    angle += 40.f * (*data->delta_time);
    if (angle > 360.f) angle -= 360.0f;
    
    vec3f axis = { -1.0f, 1.0f, 2.0f };
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
    image_free(&data->albedo);

    window_close(&data->app->window);
}