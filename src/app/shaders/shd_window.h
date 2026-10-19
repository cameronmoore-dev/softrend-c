#pragma once

#include "renderer/_math.h"
#include "renderer/vertex.h"
#include "app/resource/image.h"

typedef struct window_shader_data
{
    mat4 *projection;
    mat4 *view;
    mat4 *model;
    image_ *albedo;
} window_shader_data_;

vertex_ window_vertex_shader(window_shader_data_ *data, vertex_ *vertex);
bool window_fragment_shader(window_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);