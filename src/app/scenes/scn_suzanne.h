#pragma once

#include "app/app_context.h"
#include "app/camera.h"
#include "app/shaders/shd_suzanne.h"

typedef struct scn_suzanne_data
{
    app_context_ *app;
    camera_ camera;

    mesh_ mesh;
    image_ albedo;
    image_ specular;
    //image_ normal_map
    suzanne_shader_data_ shader_data;
    shader_ shader;
    mat4 model;

    f64 *delta_time;
} scn_suzanne_data_;

void scn_suzanne_init(scene_funcs_ *funcs);
void scn_suzanne_start(scn_suzanne_data_ *data);
void scn_suzanne_process_events(scn_suzanne_data_ *data);
void scn_suzanne_update(scn_suzanne_data_ *data);
void scn_suzanne_render(scn_suzanne_data_ *data);
void scn_suzanne_shutdown(scn_suzanne_data_ *data);