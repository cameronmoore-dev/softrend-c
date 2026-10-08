#pragma once

#include "renderer/_math.h"
#include "renderer/vertex.h"
#include "app/img_loader.h"

typedef struct
{
    // Matrices
    mat4 *projection;
    mat4 *view;
    mat4 *model;
    
    // Texture
    image_ *albedo;
} cube_shader_data_;

vertex_ cube_vertex_shader(cube_shader_data_ *data, vertex_ *vertex);
bool cube_fragment_shader(cube_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);