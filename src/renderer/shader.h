#pragma once

#include "renderer/vertex.h"
#include "app/resource/image.h"

typedef struct
{
    vertex_(*vertex)(void*, vertex_*);
    bool(*fragment)(void*, vec3f*, vertex_*, colour_*);
    void *data;
} shader_;

shader_ shader_init(void *data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*));
colour_ sample_texture(vertex_ *face, image_ *texture, vec3f *barycentric);
colour_ sample_texture_affine(vertex_ *face, image_ *texture, vec3f *barycentric);