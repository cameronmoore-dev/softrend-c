#include "camera.h"

#include <math.h>

camera_ camera_init()
{
    camera_ camera = 
    {
        .projection = perspective_matrix(45.0f, 640/480, 0.5f, 15.f),
        .position = { .x = 0.0f, .y = 0.0f, .z = -10.0f },
        .forward  = { .x = 0.0f, .y = 0.0f, .z = -1.0f },
        .yaw = -90.0f,
        .sensitivity = 20.0f,
        .move_speed = 5.0f,
    };

    camera_update_view(&camera);

    return camera;
}

void camera_update_view(camera_ *camera)
{
    vec3f up  = { .x = 0.0f, .y = 1.0f, .z = 0.0f };
    vec3f dir = 
    { 
        .x = camera->position.x + camera->forward.x,
        .y = camera->position.y + camera->forward.y,
        .z = camera->position.z + camera->forward.z
    };

    lookat_matrix(&camera->view, &camera->position, &dir, &up);
}

void camera_update_position(camera_ *camera, input_ *input, f32 delta_time)
{
    vec3f world_up = { .x = 0.0f, .y = 1.0f, .z = 0.0f };
    vec3f c = cross(&camera->forward, &world_up);
    vec3f right = normalize(&c);

    vec3f f = vec3_mul_flt(&camera->forward, (f32)((s32)is_key_pressed(input, KEY_W) - (s32)is_key_pressed(input, KEY_S)));
    vec3f r = vec3_mul_flt(&right,           (f32)((s32)is_key_pressed(input, KEY_D) - (s32)is_key_pressed(input, KEY_A)));
    vec3f u = vec3_mul_flt(&world_up,        (f32)((s32)is_key_pressed(input, KEY_Q) - (s32)is_key_pressed(input, KEY_E)));

    f = vec3_mul_flt(&f, camera->move_speed * delta_time);
    r = vec3_mul_flt(&r, camera->move_speed * delta_time);
    u = vec3_mul_flt(&u, camera->move_speed * delta_time);

    camera->position = vec3_sub(&camera->position, &f);
    camera->position = vec3_sub(&camera->position, &r);
    camera->position = vec3_sub(&camera->position, &u);
}

void camera_update_rotation(camera_ *camera, input_ *input, f32 delta_time)
{
    camera->yaw   += input->mouse_dx * camera->sensitivity * delta_time;
    camera->pitch += input->mouse_dy * camera->sensitivity * delta_time;
    camera->pitch = clampf(camera->pitch, -89.0f, 89.0f);

    // NOTE: The mouse delta only gets updated when a mouse event has been triggered by the OS,
    //       this means that the mouse delta never goes back to 0, and the rotation will drift
    input->mouse_dx = 0;
    input->mouse_dy = 0;

    f32 ryaw    = degToRad(camera->yaw);
    f32 rpitch  = degToRad(camera->pitch);

    vec3f dir = 
    {
        .x = cosf(ryaw) * cosf(rpitch),
        .y = sinf(rpitch),
        .z = sinf(ryaw) * cosf(rpitch)
    };

    camera->forward = normalize(&dir);
}