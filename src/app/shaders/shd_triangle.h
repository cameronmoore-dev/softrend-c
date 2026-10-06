#pragma once

#include "_math.h"
#include "renderer/vertex.h"

typedef struct
{
    
} triangle_shader_data_;

vertex_ triangle_vertex_shader(triangle_shader_data_ *data, vertex_ *vertex);
bool triangle_fragment_shader(triangle_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);