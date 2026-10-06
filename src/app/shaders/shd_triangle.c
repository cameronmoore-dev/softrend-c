#include "app/shaders/shd_triangle.h"

vertex_ triangle_vertex_shader(triangle_shader_data_ *data, vertex_ *vertex)
{
    vertex_ v = *vertex;
    return v;
}

bool triangle_fragment_shader(triangle_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    vec4f c0 = packed_to_rgba(mesh[0].colour);
    vec4f c1 = packed_to_rgba(mesh[1].colour);
    vec4f c2 = packed_to_rgba(mesh[2].colour);

    vec4f final;
    final.x = (point->x * c0.x) + (point->y * c1.x) + (point->z * c2.x);
    final.y = (point->x * c0.y) + (point->y * c1.y) + (point->z * c2.y);
    final.z = (point->x * c0.z) + (point->y * c1.z) + (point->z * c2.z);
    final.w = (point->x * c0.w) + (point->y * c1.w) + (point->z * c2.w);

    *colour = rgba_to_packed(final);
    return true;
}