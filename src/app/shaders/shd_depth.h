#pragma once

#include "renderer/_math.h"
#include "renderer/vertex.h"

typedef struct depth_shader_data
{
    mat4 *model;
    mat4 *light_space_matrix;
} depth_shader_data_;

vertex_ depth_vertex_shader(depth_shader_data_ *data, vertex_ *vertex);
bool depth_fragment_shader(depth_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);