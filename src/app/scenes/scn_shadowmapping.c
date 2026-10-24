#include "scn_shadowmapping.h"

#include <stdlib.h>
#include <math.h>

#define SHADOW_MAP_SIZE 1024

void draw_floor(scn_shadowmapping_data_ *data, shader_ *shader);
void draw_suzanne(scn_shadowmapping_data_ *data, shader_ *shader);

void scn_shadowmapping_init(scene_funcs_ *funcs)
{
    funcs->start = (void *)scn_shadowmapping_start;
    funcs->process_events = (void *)scn_shadowmapping_process_events;
    funcs->update = (void *)scn_shadowmapping_update;
    funcs->render = (void *)scn_shadowmapping_render;
    funcs->shutdown = (void *)scn_shadowmapping_shutdown;
}

void scn_shadowmapping_start(scn_shadowmapping_data_ *data)
{
    data->depth_shader = shader_init(
        (void *)&data->depth_shader_data,
        (void *)depth_vertex_shader,
        (void *)depth_fragment_shader
    );

    data->shadowmap_shader = shader_init(
        (void *)&data->shadowmap_shader_data,
        (void *)shadowmap_vertex_shader,
        (void *)shadowmap_fragment_shader
    );

    data->suzanne_mesh = mesh_load("./res/models/suzanne.obj");
    data->floor_mesh   = mesh_load("./res/models/plane.obj");
    data->albedo = image_load("./res/textures/suzanne_albedo.tga");
    data->camera = camera_init();
    data->model_matrix = mat4_identity();

    data->light_source.projection = orthographic_matrix(-10.f, 10.0f, -10.0f, 10.0f, 1.0f, 20.f);
    data->light_source.position = (vec3f){ .x = 4.972013, .y = -4.985215, .z = -5.545801 };
    data->light_source.forward  = (vec3f){ .x = 0.549471, .y = -0.576577, .z = -0.604682 };
    data->light_source.yaw   = -47.800663;
    data->light_source.pitch = -35.199978;
    camera_update_view(&data->light_source);

    data->light_space_matrix = mat4_mul(&data->light_source.projection, &data->light_source.view);
    data->depth_shader_data.model = &data->model_matrix;
    data->depth_shader_data.light_space_matrix = &data->light_space_matrix;

    data->shadowmap_shader_data.projection = &data->camera.projection;
    data->shadowmap_shader_data.view = &data->camera.view;
    data->shadowmap_shader_data.model = &data->model_matrix;
    data->shadowmap_shader_data.albedo = &data->albedo;
    data->shadowmap_shader_data.light_space_matrix = &data->light_space_matrix;
    data->shadowmap_shader_data.shadow_map_size = SHADOW_MAP_SIZE;

    data->depth_map  = renderer_create_rendertarget(&data->app->renderer, 0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    data->default_rt = renderer_create_rendertarget(&data->app->renderer, 0, 0, 640, 480);
    data->shadowmap_shader_data.shadow_map = data->depth_map.depthbuffer;

    // renderer_set_renderpass(&data->app->renderer, RENDERPASS_DEPTH);
    // renderer_set_rendertarget(&data->app->renderer, data->depth_map);
    // renderer_clear(&data->app->renderer);
    // draw_floor(data, &data->depth_shader);
    // draw_suzanne(data, &data->depth_shader);

    // renderer_set_renderpass(&data->app->renderer, RENDERPASS_COLOUR);
    // renderer_set_rendertarget(&data->app->renderer, data->default_rt);
}

void scn_shadowmapping_process_events(scn_shadowmapping_data_ *data)
{
    window_pump_messages(data->app->window);

    input_ *input = &data->app->window->input;
    if (is_key_pressed(input, ESC))
    {
        scn_shadowmapping_shutdown(data);
    }

    camera_update_position(&data->camera, input, *data->delta_time);
    camera_update_rotation(&data->camera, input, *data->delta_time);
    camera_update_view(&data->camera);
}

void scn_shadowmapping_update(scn_shadowmapping_data_ *data)
{
}

void scn_shadowmapping_render(scn_shadowmapping_data_ *data)
{
renderer_set_renderpass(&data->app->renderer, RENDERPASS_DEPTH);
    renderer_set_rendertarget(&data->app->renderer, data->depth_map);
    renderer_clear(&data->app->renderer);
    draw_floor(data, &data->depth_shader);
    draw_suzanne(data, &data->depth_shader);

    renderer_set_renderpass(&data->app->renderer, RENDERPASS_COLOUR);
    renderer_set_rendertarget(&data->app->renderer, data->default_rt);

    renderer_clear(&data->app->renderer);
    draw_floor(data, &data->shadowmap_shader);
    draw_suzanne(data, &data->shadowmap_shader);

    renderer_end_draw(&data->app->renderer);
    window_swap_buffers(data->app->window);
}

void scn_shadowmapping_shutdown(scn_shadowmapping_data_ *data)
{
    image_free(&data->albedo);
    mesh_free(&data->suzanne_mesh);
    mesh_free(&data->floor_mesh);

    free(data->default_rt.colourbuffer);
    free(data->default_rt.depthbuffer);
    free(data->depth_map.colourbuffer);
    free(data->depth_map.depthbuffer);

    scene_remove(&data->app->scene_manager);
}

void draw_floor(scn_shadowmapping_data_ *data, shader_ *shader)
{
    vec3f pos  = { 0.0f, 1.0f, 0.0f };
    vec3f axis = { 1.0f, 0.0f, 0.0f };
    vec3f scl  = { 4.0f, 4.0f, 4.0f };
    mat4 transform = matrix_translate(&pos);
    mat4 rotation = matrix_rotate(90.0f, &axis);
    mat4 scale = matrix_scale(&scl);
    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);
    data->model_matrix = trs;

    renderer_draw(&data->app->renderer, &data->floor_mesh, shader);
}

void draw_suzanne(scn_shadowmapping_data_ *data, shader_ *shader)
{
    static f32 angle;
    angle += 20.0f * (*data->delta_time);
    if (angle > 360.f) angle -= 360.0f;
    
    vec3f axis = { 1.0f, 1.0f, 1.0f };
    mat4 transform = mat4_identity();
    mat4 rotation = matrix_rotate(angle, &axis);
    mat4 scale = mat4_identity();
    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);
    data->model_matrix = trs;

    renderer_draw(&data->app->renderer, &data->suzanne_mesh, shader);
}