#pragma once

#include "renderer/_math.h"
#include "renderer/vertex.h"
#include "app/resource/image.h"

typedef struct grass_shader_data
{
    mat4 *projection;
    mat4 *view;
    mat4 *model;
    image_ *albedo;
} grass_shader_data_;

vertex_ grass_vertex_shader(grass_shader_data_ *data, vertex_ *vertex);
bool grass_fragment_shader(grass_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);