#include "scn_alphablending.h"

void scn_alphablending_init(scene_funcs_ *funcs)
{
    funcs->start = (void *)scn_alphablending_start;
    funcs->process_events = (void *)scn_alphablending_process_events;
    funcs->update = (void *)scn_alphablending_update;
    funcs->render = (void *)scn_alphablending_render;
    funcs->shutdown = (void *)scn_alphablending_shutdown;
}

void scn_alphablending_start(scn_alphablending_data_ *data)
{
    data->window_object.shader = shader_init(
        &data->window_data, 
        (void *)window_vertex_shader,
        (void *)window_fragment_shader
    );

    data->grass_object.shader = shader_init(
        &data->grass_data, 
        (void *)grass_vertex_shader,
        (void *)grass_fragment_shader
    );

    data->cube_object.shader = shader_init(
        &data->cube_data, 
        (void *)cube_vertex_shader,
        (void *)cube_fragment_shader
    );

    data->camera = camera_init();
    
    data->window_object.mesh = mesh_load("./res/models/plane.obj");
    data->window_object.albedo = image_load("./res/textures/texture_window.tga");
    data->window_data.projection = &data->camera.projection;
    data->window_data.view = &data->camera.view;
    data->window_data.model = &data->window_object.model;
    data->window_data.albedo = &data->window_object.albedo;

    data->grass_object.mesh = mesh_load("./res/models/plane.obj");
    data->grass_object.albedo = image_load("./res/textures/texture_grass.tga");
    data->grass_data.projection = &data->camera.projection;
    data->grass_data.view = &data->camera.view;
    data->grass_data.model = &data->grass_object.model;
    data->grass_data.albedo = &data->grass_object.albedo;

    data->cube_object.mesh = mesh_load("./res/models/cube.obj");
    data->cube_object.albedo = image_load("./res/textures/texture_dirtcube.tga");
    data->cube_data.projection = &data->camera.projection;
    data->cube_data.view = &data->camera.view;
    data->cube_data.model = &data->cube_object.model;
    data->cube_data.albedo = &data->cube_object.albedo;

    vec3f axis = { 1.0f, 0.0f, 0.0f };
    mat4 transform = mat4_identity();
    mat4 rotation = matrix_rotate(180.0f, &axis);
    mat4 scale = mat4_identity();
    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);
    data->window_object.model = trs;

    vec3f grass_pos = (vec3f){ .x = 0.0f, .y = 0.0f, .z = 1.0f };
    transform = matrix_translate(&grass_pos);
    rs  = mat4_mul(&rotation, &scale);
    trs = mat4_mul(&transform, &rs);
    data->grass_object.model = trs;

    data->window_object.albedo.scale.x = 0.5f;
    data->window_object.albedo.scale.y = 0.5f;
    data->grass_object.albedo.wrap_type = MIRRORED_REPEAT;
}

void scn_alphablending_process_events(scn_alphablending_data_ *data)
{
    window_pump_messages(data->app->window);

    input_ *input = &data->app->window->input;
    if (is_key_pressed(input, ESC))
    {
        scn_alphablending_shutdown(data);
    }

    camera_update_position(&data->camera, input, *data->delta_time);
    camera_update_rotation(&data->camera, input, *data->delta_time);
    camera_update_view(&data->camera);
}

void scn_alphablending_update(scn_alphablending_data_ *data)
{
    data->window_object.albedo.offset.x -= *data->delta_time;
    data->window_object.albedo.offset.y -= *data->delta_time;
    data->grass_object.albedo.offset.x += *data->delta_time;
    data->grass_object.albedo.offset.y += *data->delta_time;

    static f32 angle;
    angle += 40.0f * (*data->delta_time);
    if (angle > 360.f) angle -= 360.0f;
    
    vec3f axis = { -1.0f, 1.0f, 2.0f };
    vec3f cube_pos = (vec3f){ .x = 0.0f, .y = 0.0f, .z = 4.0f };
    mat4 transform = matrix_translate(&cube_pos);
    mat4 rotation = matrix_rotate(angle, &axis);
    mat4 scale = mat4_identity();
    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);

    data->cube_object.model = trs;
}

void scn_alphablending_render(scn_alphablending_data_ *data)
{
    renderer_clear(&data->app->renderer);

    u32 w = data->app->window->width;
    u32 h = data->app->window->height;
    renderer_set_viewport(&data->app->renderer, w/2, h/2, w/2, h/2);
    renderer_draw(&data->app->renderer, &data->cube_object.mesh, &data->cube_object.shader);

    renderer_set_viewport(&data->app->renderer, 0, 0, w/2, h/2);
    renderer_draw(&data->app->renderer, &data->grass_object.mesh, &data->grass_object.shader);
    renderer_draw(&data->app->renderer, &data->window_object.mesh, &data->window_object.shader);

    window_swap_buffers(data->app->window);
}

void scn_alphablending_shutdown(scn_alphablending_data_ *data)
{
    u32 w = data->app->window->width;
    u32 h = data->app->window->height;
    renderer_set_viewport(&data->app->renderer, 0, 0, w, h);

    image_free(&data->window_object.albedo);
    mesh_free(&data->window_object.mesh);

    scene_remove(&data->app->scene_manager);
}