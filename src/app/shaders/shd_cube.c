#include "shd_cube.h"

vertex_ cube_vertex_shader(cube_shader_data_ *data, vertex_ *vertex)
{
    vertex_ vtx;

    vec4f m = mat4_mul_vec4(data->model, &vertex->pos);
    vec4f v = mat4_mul_vec4(data->view, &m);
    vec4f p = mat4_mul_vec4(data->projection, &v);
    vtx.pos = p;

    return vtx;
}

bool cube_fragment_shader(cube_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    // TODO: sample perspective correct texture
    *colour = (colour_){ 0x44, 0x77, 0xFF, 0xFF };
    return true;
}