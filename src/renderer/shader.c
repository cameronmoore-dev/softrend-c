#include "shader.h"

shader_ shader_init(void *data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*))
{
    shader_ s;
    s.vertex = vertex;
    s.fragment = fragment;
    s.data = data;

    return s;
}

u32 sample_texture(vertex_ *face, image_ *texture, vec3f *barycentric)
{
    f32 inv_w0 = 1.0f / face[0].pos.w;
    f32 inv_w1 = 1.0f / face[1].pos.w;
    f32 inv_w2 = 1.0f / face[2].pos.w;
    vec2f uv0 = { .x = face[0].uv.x / face[0].pos.w, .y = face[0].uv.y / face[0].pos.w };
    vec2f uv1 = { .x = face[1].uv.x / face[1].pos.w, .y = face[1].uv.y / face[1].pos.w };
    vec2f uv2 = { .x = face[2].uv.x / face[2].pos.w, .y = face[2].uv.y / face[2].pos.w };

    f32 inverse = inv_w0 * barycentric->x + inv_w1 * barycentric->y + inv_w2 * barycentric->z;
    vec2f uv = 
    {
        .x = uv0.x * barycentric->x + uv1.x * barycentric->y + uv2.x * barycentric->z,
        .y = uv0.y * barycentric->x + uv1.y * barycentric->y + uv2.y * barycentric->z
    };

    vec2f corrected_uv = 
    {
        .x = uv.x / inverse,
        .y = uv.y / inverse
    };

    u32 x = (u32)(corrected_uv.x * texture->width);
    u32 y = (u32)(corrected_uv.y * texture->height);

    return texture->pixels[x + y * texture->width];
}

u32 sample_texture_affine(vertex_ *face, image_ *texture, vec3f *barycentric)
{
    vec2f linear_uv = 
    {
        .x = face[0].uv.x * barycentric->x + face[1].uv.x * barycentric->y + face[2].uv.x * barycentric->z,
        .y = face[0].uv.y * barycentric->x + face[1].uv.y * barycentric->y + face[2].uv.y * barycentric->z
    };

    u32 x = (u32)(linear_uv.x * texture->width);
    u32 y = (u32)(linear_uv.y * texture->height);

    return texture->pixels[x + y * texture->width];
}