#include "scn_suzanne.h"

#include "app/resource/image.h"
#include "app/resource/mesh.h"

#include <math.h>

vec3f rotate_lightsource(f32 delta_time);
mat4 rotate_suzanne(scn_suzanne_data_ *data);

void scn_suzanne_init(scene_funcs_ *funcs)
{
    funcs->start = (void *)scn_suzanne_start;
    funcs->process_events = (void *)scn_suzanne_process_events;
    funcs->update = (void *)scn_suzanne_update;
    funcs->render = (void *)scn_suzanne_render;
    funcs->shutdown = (void *)scn_suzanne_shutdown;
}

void scn_suzanne_start(scn_suzanne_data_ *data)
{
    data->shader = shader_init(
        &data->shader_data, 
        (void *)suzanne_vertex_shader, 
        (void *)suzanne_fragment_shader
    );
    
    data->mesh   = mesh_load("./res/models/suzanne.obj");
    data->albedo = image_load("./res/textures/suzanne_albedo.tga");
    bilinear_filter(&data->albedo);
    data->specular = image_load("./res/textures/suzanne_specular.tga");
    data->normal_map = image_load("./res/textures/suzanne_normal.tga");
    data->camera = camera_init();
    data->model = mat4_identity();

    vec3f axis = { 0.5f, 0.0f, 1.0f };
    mat4 transform = mat4_identity();
    mat4 rotation = matrix_rotate(180.0f, &axis);
    mat4 scale = mat4_identity();
    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);
    data->model = trs;

    data->shader_data.projection = &data->camera.projection;
    data->shader_data.view = &data->camera.view;
    data->shader_data.model = &data->model;
    data->shader_data.albedo = &data->albedo;
    data->shader_data.specular = &data->specular;
    data->shader_data.normal_map = &data->normal_map;

    data->shader_data.light_dir = (vec3f){ .x = 0.0f, .y = 0.0f, .z = -1.0f };
}

void scn_suzanne_process_events(scn_suzanne_data_ *data)
{
    window_pump_messages(data->app->window);

    input_ *input = &data->app->window->input;
    if (is_key_pressed(input, ESC))
    {
        scn_suzanne_shutdown(data);
    }

    camera_update_position(&data->camera, input, *data->delta_time);
    camera_update_rotation(&data->camera, input, *data->delta_time);
    camera_update_view(&data->camera);
}

void scn_suzanne_update(scn_suzanne_data_ *data)
{
    data->shader_data.light_dir = rotate_lightsource(*data->delta_time);
    // data->model = rotate_suzanne(data);
}

void scn_suzanne_render(scn_suzanne_data_ *data)
{
    renderer_clear(&data->app->renderer);
    renderer_draw(&data->app->renderer, &data->mesh, &data->shader);
    renderer_end_draw(&data->app->renderer);
    window_swap_buffers(data->app->window);
}

void scn_suzanne_shutdown(scn_suzanne_data_ *data)
{
    image_free(&data->albedo);
    mesh_free(&data->mesh);

    scene_remove(&data->app->scene_manager);
}

vec3f rotate_lightsource(f32 delta_time)
{
    static f32 light_dir_theta;
    light_dir_theta += 50.0f * delta_time;
    if (light_dir_theta > 360.0f) light_dir_theta -= 360.0f;

    f32 trad = degrees_to_radians(light_dir_theta);
    f32 c = cosf(trad);
    f32 s = sinf(trad);
    vec3f k = { .x = 1.0f, .y = 1.0f, .z = 0.0f };

    // Light direction when rotating around the k axis
    vec3f v = { .x = 0.0f, .y = 0.0f, .z = -1.0f };
    vec3f cr = cross(&k, &v);
    f32 scalar = k.x * v.x + k.y * v.y + k.z * v.z;
    f32 scalar_angle = scalar * (1.0f - c);

    vec3f result = 
    {
        .x = v.x * c + cr.x * s + k.x * scalar_angle,
        .y = v.y * c + cr.y * s + k.y * scalar_angle,
        .z = v.z * c + cr.z * s + k.z * scalar_angle
    };

    return result;
}

mat4 rotate_suzanne(scn_suzanne_data_ *data)
{
    static f32 angle;
    angle += 40.0f * (*data->delta_time);
    if (angle > 360.f) angle -= 360.0f;
    
    vec3f axis = { -1.0f, 1.0f, 2.0f };
    mat4 transform = mat4_identity();
    mat4 rotation = matrix_rotate(angle, &axis);
    mat4 scale = mat4_identity();

    mat4 rs  = mat4_mul(&rotation, &scale);
    mat4 trs = mat4_mul(&transform, &rs);

    return trs;
}