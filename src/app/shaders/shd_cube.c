#include "shd_cube.h"

#include "renderer/shader.h"

vertex_ cube_vertex_shader(cube_shader_data_ *data, vertex_ *vertex)
{
    vertex_ vtx;

    vec4f m = mat4_mul_vec4(data->model, &vertex->pos);
    vec4f v = mat4_mul_vec4(data->view, &m);
    vec4f p = mat4_mul_vec4(data->projection, &v);
    vtx.pos = p;
    vtx.uv = vertex->uv;

    return vtx;
}

bool cube_fragment_shader(cube_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    colour->packed = sample_texture_affine(mesh, data->albedo, point);
    return true;
}