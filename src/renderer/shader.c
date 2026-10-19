#include "shader.h"

#include <math.h>

vec2i get_perspective_correct_uv(vertex_ *face, image_ *texture, vec3f *barycentric)
{
    f32 inv_w0 = 1.0f / face[0].pos.w;
    f32 inv_w1 = 1.0f / face[1].pos.w;
    f32 inv_w2 = 1.0f / face[2].pos.w;
    vec2f uv0 = { .x = face[0].uv.x / face[0].pos.w, .y = face[0].uv.y / face[0].pos.w };
    vec2f uv1 = { .x = face[1].uv.x / face[1].pos.w, .y = face[1].uv.y / face[1].pos.w };
    vec2f uv2 = { .x = face[2].uv.x / face[2].pos.w, .y = face[2].uv.y / face[2].pos.w };

    f32 inverse = inv_w0 * barycentric->x + 
                  inv_w1 * barycentric->y + 
                  inv_w2 * barycentric->z;
                  
    vec2f uv = 
    {
        .x = uv0.x * barycentric->x + 
             uv1.x * barycentric->y + 
             uv2.x * barycentric->z,

        .y = uv0.y * barycentric->x + 
             uv1.y * barycentric->y + 
             uv2.y * barycentric->z
    };

    uv.x /= inverse;
    uv.y /= inverse;

    u32 x = (u32)(uv.x * texture->width);
    u32 y = (u32)(uv.y * texture->height);

    return (vec2i){ x, y };
}

u32 texture_wrap(vec2i uv, image_ *texture)
{
    vec2f _uv = 
    { 
        .x = (f32)uv.x / (f32)texture->width, 
        .y = (f32)uv.y / (f32)texture->height
    };

    _uv.x += texture->offset.x;
    _uv.y += texture->offset.y;
    _uv.x /= texture->scale.x;
    _uv.y /= texture->scale.y;

    // if (_uv.x > 0.0f || _uv.x < 1.0f ||
    //     _uv.y > 0.0f || _uv.y < 1.0f)
    // {
    //     return uv.x + uv.y * texture->width;
    // }

    f32 ufrac = fabsf(_uv.x - (s32)_uv.x);
    f32 vfrac = fabsf(_uv.y - (s32)_uv.y);
    u32 x = ufrac * texture->width;
    u32 y = vfrac * texture->height;

    switch (texture->wrap_type)
    {
        case REPEAT:
        {
        }break;

        case MIRRORED_REPEAT:
        {
            x = fabs((2.0f * ufrac - 1.0f) * texture->width); 
            y = fabs((2.0f * vfrac - 1.0f) * texture->height);
        } break;

        case CLAMP_TO_EDGE:
        {
            _uv.x = clampf(_uv.x, 0.0f, 1.0f);
            _uv.y = clampf(_uv.y, 0.0f, 1.0f);
            x = _uv.x * (texture->width - 1);
            y = _uv.y * (texture->height - 1);
        } break;

        case CLAMP_TO_BORDER:
        {
            if (_uv.x < 0.0f || _uv.x > 1.0f ||
                _uv.y < 0.0f || _uv.y > 1.0f)
            {
                return 0xFFFF00FF;
            }
        } break;

        default: break;
    }

    return texture->pixels[x + y * texture->width];
}

shader_ shader_init(void *shader_data, vertex_(*vertex)(void*, vertex_*), bool(*fragment)(void*, vec3f*, vertex_*, colour_*))
{
    shader_ s;
    s.vertex = vertex;
    s.fragment = fragment;
    s.data = shader_data;

    return s;
}

colour_ sample_texture(vertex_ *face, image_ *texture, vec3f *barycentric)
{
    vec2i uv = get_perspective_correct_uv(face, texture, barycentric);
    u32 texture_texel = texture_wrap(uv, texture);

    return colour(texture_texel);
}

colour_ sample_texture_affine(vertex_ *face, image_ *texture, vec3f *barycentric)
{
    vec2f linear_uv = 
    {
        .x = face[0].uv.x * barycentric->x + 
             face[1].uv.x * barycentric->y + 
             face[2].uv.x * barycentric->z,

        .y = face[0].uv.y * barycentric->x + 
             face[1].uv.y * barycentric->y + 
             face[2].uv.y * barycentric->z
    };

    u32 x = (u32)(linear_uv.x * texture->width);
    u32 y = (u32)(linear_uv.y * texture->height);

    return colour(texture->pixels[x + y * texture->width]);
}