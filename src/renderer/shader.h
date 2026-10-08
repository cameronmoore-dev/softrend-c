#pragma once

#include "renderer/vertex.h"
#include "app/img_loader.h"

typedef struct
{
    vertex_(*vertex)(void*, vertex_*);
    bool(*fragment)(void*, vec3f*, vertex_*, colour_*);
    void *data;
} shader_;

shader_ shader_init(void *data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*));
u32 sample_texture_affine(vertex_ *face, image_ *texture, vec3f *barycentric);