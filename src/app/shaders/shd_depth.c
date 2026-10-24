#include "shd_depth.h"

vertex_ depth_vertex_shader(depth_shader_data_ *data, vertex_ *vertex)
{
    vec4f m  = mat4_mul_vec4(data->model, &vertex->pos);
    vec4f ls = mat4_mul_vec4(data->light_space_matrix, &m);

    vertex_ vtx = *vertex;
    vtx.pos = ls;

    return vtx;
}

bool depth_fragment_shader(depth_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    return true;
}