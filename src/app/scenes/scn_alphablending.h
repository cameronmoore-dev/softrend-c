#pragma once

#include "renderer/_math.h"
#include "renderer/shader.h"
#include "app/app_context.h"
#include "app/camera.h"

#include "app/shaders/shd_window.h"
#include "app/shaders/shd_grass.h"
#include "app/shaders/shd_cube.h"

typedef struct object
{
    mat4 model;
    mesh_ mesh;
    image_ albedo;
    shader_ shader;
} object_;

typedef struct scn_alphablending_data
{
    app_context_ *app;
    camera_ camera;

    object_ window_object;
    object_ grass_object;
    object_ cube_object;

    window_shader_data_ window_data;
    grass_shader_data_ grass_data;
    cube_shader_data_ cube_data;

    f64 *delta_time;
} scn_alphablending_data_;

void scn_alphablending_init(scene_funcs_ *funcs);
void scn_alphablending_start(scn_alphablending_data_ *data);
void scn_alphablending_process_events(scn_alphablending_data_ *data);
void scn_alphablending_update(scn_alphablending_data_ *data);
void scn_alphablending_render(scn_alphablending_data_ *data);
void scn_alphablending_shutdown(scn_alphablending_data_ *data);