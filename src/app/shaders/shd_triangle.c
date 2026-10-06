#include "app/shaders/shd_triangle.h"

vertex_ triangle_vertex_shader(triangle_shader_data_ *data, vertex_ *vertex)
{
    vertex_ v = *vertex;
    return v;
}

bool triangle_fragment_shader(triangle_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    colour->packed = 0xFF00FF;
    return true;
}