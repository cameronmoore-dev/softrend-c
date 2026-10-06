#include "renderer/shader.h"

shader_ shader_init(void *data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*))
{
    shader_ s;
    s.vertex = vertex;
    s.fragment = fragment;
    s.data = data;

    return s;
}