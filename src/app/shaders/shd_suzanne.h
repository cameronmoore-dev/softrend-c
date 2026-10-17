#pragma once

#include "renderer/_math.h"
#include "renderer/vertex.h"
#include "app/resource/image.h"

typedef struct suzanne_shader_data
{
    mat4 *projection;
    mat4 *view;
    mat4 *model;

    image_ *albedo;
    image_ *specular;
    image_ *normal_map;

    vec3f light_dir;
} suzanne_shader_data_;

vertex_ suzanne_vertex_shader(suzanne_shader_data_ *data, vertex_ *vertex);
bool suzanne_fragment_shader(suzanne_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour);