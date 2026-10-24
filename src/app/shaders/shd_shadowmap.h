#pragma once

#include "renderer/_math.h"
#include "renderer/vertex.h"
#include "app/resource/image.h"

typedef struct shadowmap_shader_data
{
    mat4 *projection;
    mat4 *view;
    mat4 *model;
    mat4 *light_space_matrix;

    image_ *albedo;
    f32 *shadow_map;
    u32 shadow_map_size;
} shadowmap_shader_data_;

vertex_ shadowmap_vertex_shader(shadowmap_shader_data_ *data, vertex_ *vertex);
bool shadowmap_fragment_shader(shadowmap_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);