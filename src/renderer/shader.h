#pragma once

#include "renderer/vertex.h"

typedef struct
{
    vertex_(*vertex)(void*, vertex_*);
    bool(*fragment)(void*, vec3f*, vertex_*, colour_*);
    void *data;
} shader_;

shader_ shader_init(void *data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*));