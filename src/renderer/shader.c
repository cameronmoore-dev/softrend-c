#include "shader.h"

u32 clamp_u32(u32 x, u32 min, u32 max)
{
    if (x < min)        return min;
    else if (x > max)   return max;
    else                return x;
}

f32 clamp_f32(f32 x, f32 min, f32 max)
{
    if (x < min)        return min;
    else if (x > max)   return max;
    else                return x;
}

shader_ shader_init(void *data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*))
{
    shader_ s;
    s.vertex = vertex;
    s.fragment = fragment;
    s.data = data;

    return s;
}

u32 sample_texture_affine(vertex_ *face, image_ *texture, vec3f *barycentric)
{
    vec2f linear_uv = 
    {
        .x = face[0].uv.x * barycentric->x + face[1].uv.x * barycentric->y + face[2].uv.x * barycentric->z,
        .y = face[0].uv.y * barycentric->x + face[1].uv.y * barycentric->y + face[2].uv.y * barycentric->z
    };

    // f32 ufraction = linear_uv.x - (u32)linear_uv.x;
    // f32 vfraction = linear_uv.y - (u32)linear_uv.y;
    u32 x = (u32)(linear_uv.x * texture->width);
    u32 y = (u32)(linear_uv.y * texture->height);

    return texture->pixels[x + y * texture->width];
}