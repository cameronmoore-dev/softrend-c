#pragma once

#include "app/app_context.h"
#include "app/mesh_loader.h"
#include "app/img_loader.h"
#include "app/shaders/shd_cube.h"

typedef struct scn_cube_data
{
    app_context_ *app;

    image_ albedo;
    mesh_ cube;
    cube_shader_data_ shader_data;
    shader_ shader;
    mat4 projection;
    mat4 view;
    mat4 model;

    f32 *delta_time;
} scn_cube_data_;

void scn_cube_init(scene_funcs_ *funcs);
void scn_cube_start(scn_cube_data_ *data);
void scn_cube_process_events(scn_cube_data_ *data);
void scn_cube_update(scn_cube_data_ *data);
void scn_cube_render(scn_cube_data_ *data);
void scn_cube_shutdown(scn_cube_data_ *data);