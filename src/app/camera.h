#pragma once

#include "renderer/_math.h"
#include "os/input.h"

typedef struct camera
{
    mat4 projection;
    mat4 view;
    vec3f position;
    vec3f forward;
    f32 yaw;
    f32 pitch;
    f32 sensitivity;
    f32 move_speed;
} camera_;

camera_ camera_init();
void camera_update_view(camera_ *camera);
void camera_update_position(camera_ *camera, input_ *input, f32 delta_time);
void camera_update_rotation(camera_ *camera, input_ *input, f32 delta_time);