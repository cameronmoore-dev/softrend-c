#pragma once

#include "app/app_context.h"
#include "app/shaders/shd_triangle.h"

typedef struct
{
    app_context_ *app;

    vertex_ triangle[3];

    triangle_shader_data_ shader_data;
    shader_ shader;
} scn_triangle_data_;

void scn_triangle_init(scene_funcs_ *funcs);
void scn_triangle_start(scn_triangle_data_ *data);
void scn_triangle_process_events(scn_triangle_data_ *data);
void scn_triangle_update(scn_triangle_data_ *data);
void scn_triangle_render(scn_triangle_data_ *data);
void scn_triangle_shutdown(scn_triangle_data_ *data);